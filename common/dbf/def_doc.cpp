#include "dbf_doc.h"
#include <ctime>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include <iostream>

DbfDoc::DbfDoc() : isModified_(false) {
    std::memset(&header_, 0, sizeof(DBFHeader));
}

DbfDoc::~DbfDoc() {}

bool DbfDoc::open(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) return false;

    ifs.read(reinterpret_cast<char*>(&header_), sizeof(DBFHeader));
    if (ifs.gcount() != sizeof(DBFHeader)) return false;

    if (!validateHeader()) return false;

    fields_.clear();
    char byte;
    ifs.seekg(sizeof(DBFHeader), std::ios::beg);
    while (ifs.read(&byte, 1) && byte != 0x0D) {
        ifs.seekg(-1, std::ios::cur);
        DBFField field;
        ifs.read(reinterpret_cast<char*>(&field), sizeof(DBFField));
        if (ifs.gcount() != sizeof(DBFField)) return false;
        fields_.push_back(field);
    }

    size_t expected_pos = sizeof(DBFHeader) + fields_.size() * sizeof(DBFField) + 1;
    if (header_.pos_first_data < expected_pos) return false;

    ifs.seekg(0, std::ios::end);
    size_t file_size = ifs.tellg();
    size_t data_size = header_.num_records * header_.record_length;
    if (header_.pos_first_data + data_size > file_size) return false;

    data_.resize(data_size);
    ifs.seekg(header_.pos_first_data, std::ios::beg);
    ifs.read(data_.data(), data_size);
    if (ifs.gcount() != static_cast<std::streamsize>(data_size)) return false;

    filename_ = filename;
    isModified_ = false;
    return true;
}

bool DbfDoc::save() {
    if (!isModified_ || filename_.empty()) return true;
    return saveAs(filename_);
}

bool DbfDoc::saveAs(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) return false;

    updateLastUpdate();
    if (!writeHeaderAndFields(ofs)) return false;

    ofs.write(data_.data(), data_.size());
    if (!ofs.good()) return false;

    filename_ = filename;
    isModified_ = false;
    return true;
}

std::vector<std::string> DbfDoc::readRecord(uint32_t index) const {
    std::vector<std::string> result;
    if (index >= header_.num_records) return result;

    const char* record = data_.data() + index * header_.record_length;
    if (record[0] == DBF_RECORD_DELETED) return result;

    size_t offset = 1;
    for (const auto& field : fields_) {
        std::string value(record + offset, field.field_length);
        if (field.field_type == DBF_FIELD_TYPE_CHARACTER) {
            size_t pos = value.find_last_not_of(' ');
            value = (pos == std::string::npos) ? "" : value.substr(0, pos + 1);
        }
        result.push_back(value);
        offset += field.field_length;
    }
    return result;
}

bool DbfDoc::modifyRecord(uint32_t index, const std::vector<std::string>& data) {
    if (index >= header_.num_records || !validateData(data)) return false;

    char* record = data_.data() + index * header_.record_length;
    if (record[0] == DBF_RECORD_DELETED) return false;

    size_t offset = 1;
    for (size_t i = 0; i < fields_.size(); ++i) {
        std::string value = data[i].substr(0, fields_[i].field_length);
        value.resize(fields_[i].field_length, ' ');
        std::memcpy(record + offset, value.data(), fields_[i].field_length);
        offset += fields_[i].field_length;
    }
    isModified_ = true;
    return true;
}

bool DbfDoc::addRecord(const std::vector<std::string>& data) {
    if (!validateData(data)) return false;

    // Recalculate expected length to ensure consistency
    uint16_t expected_length = calculateRecordLength() + 1;
    if (header_.record_length != expected_length) {
        std::cerr << "Record length mismatch: header=" << header_.record_length
                  << ", expected=" << expected_length << std::endl;
        header_.record_length = expected_length; // Fix it
    }

    std::vector<char> new_record(header_.record_length, ' ');
    new_record[0] = DBF_RECORD_NOT_DELETED;

    size_t offset = 1;
    for (size_t i = 0; i < fields_.size(); ++i) {
        if (offset + fields_[i].field_length > header_.record_length) {
            std::cerr << "Buffer overflow prevented in addRecord" << std::endl;
            return false;
        }
        std::string value = data[i].substr(0, fields_[i].field_length);
        value.resize(fields_[i].field_length, ' ');
        std::memcpy(new_record.data() + offset, value.data(), fields_[i].field_length);
        offset += fields_[i].field_length;
    }

    data_.insert(data_.end(), new_record.begin(), new_record.end());
    header_.num_records++;
    isModified_ = true;
    return true;
}

bool DbfDoc::deleteRecord(uint32_t index) {
    if (index >= header_.num_records) return false;

    char* record = data_.data() + index * header_.record_length;
    if (record[0] == DBF_RECORD_DELETED) return true;

    record[0] = DBF_RECORD_DELETED;
    isModified_ = true;
    return true;
}

bool DbfDoc::create(const std::string& filename, const std::vector<DBFField>& fields) {
    if (fields.empty()) return false;

    std::memset(&header_, 0, sizeof(DBFHeader));
    header_.file_type = DBF_FILE_TYPE_FOXBASE_PLUS_DBASE_III;
    fields_ = fields;
    header_.record_length = calculateRecordLength() + 1;
    header_.pos_first_data = sizeof(DBFHeader) + fields_.size() * sizeof(DBFField) + 1;
    header_.num_records = 0;
    data_.clear();

    // Validate fields_
    if (calculateRecordLength() == 0) {
        std::cerr << "Error: No valid fields in create" << std::endl;
        return false;
    }

    filename_ = filename;
    isModified_ = true;
    return save();
}

void DbfDoc::updateLastUpdate() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    header_.last_update[0] = static_cast<uint8_t>(t->tm_year - 100);
    header_.last_update[1] = static_cast<uint8_t>(t->tm_mon + 1);
    header_.last_update[2] = static_cast<uint8_t>(t->tm_mday);
}

uint16_t DbfDoc::calculateRecordLength() const {
    uint16_t length = 0;
    for (const auto& field : fields_) {
        length += field.field_length;
    }
    return length;
}

bool DbfDoc::writeHeaderAndFields(std::ofstream& ofs) const {
    ofs.write(reinterpret_cast<const char*>(&header_), sizeof(DBFHeader));
    for (const auto& field : fields_) {
        ofs.write(reinterpret_cast<const char*>(&field), sizeof(DBFField));
    }
    char terminator = 0x0D;
    ofs.write(&terminator, 1);
    return ofs.good();
}

bool DbfDoc::validateData(const std::vector<std::string>& data) const {
    return data.size() == fields_.size();
}

bool DbfDoc::validateHeader() const {
    if (header_.record_length < 1) return false;
    if (header_.pos_first_data < sizeof(DBFHeader)) return false;
    return true;
}

void DbfDoc::debugPrint() const {
    std::cout << "Debug Info:" << std::endl;
    std::cout << "  Filename: " << filename_ << std::endl;
    std::cout << "  Record Length: " << header_.record_length << std::endl;
    std::cout << "  Expected Length: " << (calculateRecordLength() + 1) << std::endl;
    std::cout << "  Num Records: " << header_.num_records << std::endl;
    std::cout << "  Fields: " << fields_.size() << std::endl;
    for (size_t i = 0; i < fields_.size(); ++i) {
        std::cout << "    " << fields_[i].field_name << ": " << (int)fields_[i].field_length << std::endl;
    }
}