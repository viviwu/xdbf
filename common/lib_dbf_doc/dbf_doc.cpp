#include "dbf_doc.h"
#include <algorithm>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace dbf {

namespace {
    // Utility functions for byte order conversion
    template<typename T>
    T swapEndian(T value) {
        union {
            T value;
            uint8_t bytes[sizeof(T)];
        } source, dest;
        
        source.value = value;
        for(size_t i = 0; i < sizeof(T); ++i)
            dest.bytes[i] = source.bytes[sizeof(T) - 1 - i];
            
        return dest.value;
    }

    // DBF version strings
    const char* getVersionString(uint8_t version) {
        switch(version) {
            case 0x02: return "FoxBase";
            case 0x03: return "dBase III Plus";
            case 0x04: return "dBase IV";
            case 0x05: return "dBase V";
            case 0x30: return "Visual FoxPro";
            case 0x31: return "Visual FoxPro with AutoIncrement";
            case 0x83: return "dBase III Plus with memo";
            case 0x8B: return "dBase IV with memo";
            case 0xF5: return "FoxPro with memo";
            default: return "Unknown";
        }
    }
}

DbfDoc::DbfDoc()
    : header_(nullptr)
    , isOpen_(false)
    , isModified_(false)
    , readOnly_(true) 
{}

DbfDoc::~DbfDoc() {
    close();
}

bool DbfDoc::open(const std::string& filename, bool readOnly) {
    close();
    
    readOnly_ = readOnly;
    auto mode = std::ios::binary | (readOnly ? std::ios::in : (std::ios::in | std::ios::out));
    
    file_.open(filename, mode);
    if (!file_)
        return false;

    if (!readHeader() || !readFields() || !loadAllRecords()) {
        close();
        return false;
    }

    isOpen_ = true;
    return true;
}

bool DbfDoc::create(const std::string& filename, const std::vector<DB_FIELD>& fields) {
    close();

    file_.open(filename, std::ios::binary | std::ios::out);
    if (!file_)
        return false;

    // Initialize header
    header_ = std::make_unique<DB_HEADER>();
    std::memset(header_.get(), 0, sizeof(DB_HEADER));

    // Calculate record length
    uint16_t recordLength = 1; // 1 byte for deletion flag
    for (const auto& field : fields)
        recordLength += field.field_length;

    // Set header values
    header_->version = 0x03;  // dBase III Plus
    header_->record_length = recordLength;
    header_->header_length = sizeof(DB_HEADER) + fields.size() * sizeof(DB_FIELD) + 1;
    header_->records = 0;

    // Set current date
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    header_->last_update[0] = ltm->tm_year - 100; // Years since 1900, subtract 100 for DBF
    header_->last_update[1] = ltm->tm_mon + 1;    // Month (0-11)
    header_->last_update[2] = ltm->tm_mday;       // Day of month

    // Save fields
    fields_ = fields;

    // Write to file
    if (!writeHeader() || !writeFields()) {
        close();
        return false;
    }

    isOpen_ = true;
    readOnly_ = false;
    return true;
}

void DbfDoc::close() {
    if (isOpen_ && isModified_ && !readOnly_) {
        writeHeader();
        saveAllRecords();
    }

    file_.close();
    header_.reset();
    fields_.clear();
    records_.clear();
    isOpen_ = false;
    isModified_ = false;
}

bool DbfDoc::readHeader() {
    header_ = std::make_unique<DB_HEADER>();
    
    file_.seekg(0);
    if (!file_.read(reinterpret_cast<char*>(header_.get()), sizeof(DB_HEADER)))
        return false;

    // Convert endianness if needed
    header_->records = swapEndian(header_->records);
    header_->header_length = swapEndian(header_->header_length);
    header_->record_length = swapEndian(header_->record_length);

    return true;
}

bool DbfDoc::readFields() {
    if (!header_)
        return false;

    size_t fieldCount = (header_->header_length - sizeof(DB_HEADER) - 1) / sizeof(DB_FIELD);
    fields_.resize(fieldCount);

    file_.seekg(sizeof(DB_HEADER));
    return file_.read(reinterpret_cast<char*>(fields_.data()), 
                     fieldCount * sizeof(DB_FIELD)).good();
}

bool DbfDoc::writeHeader() {
    if (!header_)
        return false;

    // Save current position
    auto pos = file_.tellg();
    file_.seekp(0);

    // Create temporary header with correct byte order
    DB_HEADER tempHeader = *header_;
    tempHeader.records = swapEndian(tempHeader.records);
    tempHeader.header_length = swapEndian(tempHeader.header_length);
    tempHeader.record_length = swapEndian(tempHeader.record_length);

    bool success = file_.write(reinterpret_cast<char*>(&tempHeader), 
                             sizeof(DB_HEADER)).good();

    // Restore position
    file_.seekg(pos);
    return success;
}

bool DbfDoc::writeFields() {
    if (fields_.empty())
        return false;

    file_.seekp(sizeof(DB_HEADER));
    return file_.write(reinterpret_cast<char*>(fields_.data()),
                      fields_.size() * sizeof(DB_FIELD)).good();
}

bool DbfDoc::loadAllRecords() {
    if (!header_)
        return false;

    records_.clear();
    records_.reserve(header_->records);

    std::vector<char> record(header_->record_length);
    file_.seekg(header_->header_length);

    for (size_t i = 0; i < header_->records; ++i) {
        if (!file_.read(record.data(), header_->record_length))
            return false;
        records_.push_back(record);
    }

    return true;
}

bool DbfDoc::saveAllRecords() {
    if (!header_ || records_.empty())
        return false;

    file_.seekp(header_->header_length);
    for (const auto& record : records_) {
        if (!file_.write(record.data(), header_->record_length))
            return false;
    }
    return true;
}

bool DbfDoc::readRecord(size_t index, std::vector<char>& record) const {
    if (index >= records_.size())
        return false;

    record = records_[index];
    return true;
}

bool DbfDoc::writeRecord(const std::vector<char>& record) {
    if (!isOpen_ || readOnly_ || record.size() != header_->record_length)
        return false;

    records_.push_back(record);
    header_->records++;
    isModified_ = true;

    return true;
}

bool DbfDoc::updateRecord(size_t index, const std::vector<char>& record) {
    if (index >= records_.size() || record.size() != header_->record_length)
        return false;

    records_[index] = record;
    isModified_ = true;
    return true;
}

bool DbfDoc::deleteRecord(size_t index) {
    if (index >= records_.size())
        return false;

    records_.erase(records_.begin() + index);
    header_->records--;
    isModified_ = true;
    return true;
}

std::string DbfDoc::getFieldValue(const std::vector<char>& record, size_t fieldIndex) const {
    if (fieldIndex >= fields_.size() || record.empty())
        return {};

    const auto& field = fields_[fieldIndex];
    size_t offset = field.field_address;
    
    std::string value(record.data() + offset, field.field_length);
    value.erase(std::find_if(value.rbegin(), value.rend(), 
                [](char c) { return c != ' '; }).base(), value.end());
                
    return value;
}

size_t DbfDoc::getFieldAddress(size_t fieldIndex) const {
    return fieldIndex < fields_.size() ? fields_[fieldIndex].field_address : 0;
}

size_t DbfDoc::getFieldLength(size_t fieldIndex) const {
    return fieldIndex < fields_.size() ? fields_[fieldIndex].field_length : 0;
}

char DbfDoc::getFieldType(size_t fieldIndex) const {
    return fieldIndex < fields_.size() ? fields_[fieldIndex].field_type : 0;
}

std::string DbfDoc::getFieldName(size_t fieldIndex) const {
    if (fieldIndex >= fields_.size())
        return {};

    return std::string(reinterpret_cast<const char*>(fields_[fieldIndex].field_name), 
                      strnlen(reinterpret_cast<const char*>(fields_[fieldIndex].field_name), 11));
}

int DbfDoc::getFieldDecimals(size_t fieldIndex) const {
    return fieldIndex < fields_.size() ? fields_[fieldIndex].field_decimals : 0;
}

size_t DbfDoc::getRecordCount() const {
    return header_ ? header_->records : 0;
}

size_t DbfDoc::getFieldCount() const {
    return fields_.size();
}

size_t DbfDoc::getRecordLength() const {
    return header_ ? header_->record_length : 0;
}

size_t DbfDoc::getHeaderLength() const {
    return header_ ? header_->header_length : 0;
}

std::string DbfDoc::getVersion() const {
    return header_ ? getDbVersion(header_->version) : "Unknown";
}

std::string DbfDoc::getLastUpdateDate() const {
    if (!header_)
        return {};

    std::ostringstream oss;
    oss << std::setfill('0') 
        << "20" << std::setw(2) << static_cast<int>(header_->last_update[0])
        << "-" << std::setw(2) << static_cast<int>(header_->last_update[1])
        << "-" << std::setw(2) << static_cast<int>(header_->last_update[2]);
    return oss.str();
}

bool DbfDoc::isMemo() const {
    return header_ ? (header_->version == 0x83 || header_->version == 0x8B || 
                     header_->version == 0xF5) : false;
}

} // namespace dbf