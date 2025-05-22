#include "dbf.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>


Dbf::Dbf() : isOpen_(false), readOnly_(true), isModified_(false) {
  std::memset(&header_, 0, sizeof(DB_HEADER));
}

Dbf::~Dbf() {
  if (isOpen_) {
    close();
  }
}

bool Dbf::open(const std::string& filename, bool readOnly) {
  close();

  auto mode = std::ios::binary |
              (readOnly ? std::ios::in : (std::ios::in | std::ios::out));
  file_.open(filename, mode);

  if (!file_) {
    setLastError("Cannot open file: " + filename);
    return false;
  }

  // Read header
  if (!readHeader()) {
    file_.close();
    return false;
  }

  // Read fields
  if (!readFields()) {
    file_.close();
    return false;
  }

  // Read records
  if (!readRecords()) {
    file_.close();
    return false;
  }

  filename_ = filename;
  readOnly_ = readOnly;
  isOpen_ = true;
  isModified_ = false;
  return true;
}

bool Dbf::readHeader() {
  if (!file_.read(reinterpret_cast<char*>(&header_), sizeof(DB_HEADER))) {
    setLastError("Failed to read DBF header");
    return false;
  }

  // Convert from little-endian to host byte order
  header_.records = leToHost32(header_.records);
  header_.header_length = leToHost16(header_.header_length);
  header_.record_length = leToHost16(header_.record_length);

  if (!validateHeader()) {
    setLastError("Invalid DBF header");
    return false;
  }

  return true;
}

bool Dbf::readFields() {
  fields_.clear();

  char terminator;
  DB_FIELD field;
  size_t maxFields =
      (header_.header_length - sizeof(DB_HEADER) - 1) / sizeof(DB_FIELD);

  file_.seekg(sizeof(DB_HEADER));

  for (size_t i = 0; i < maxFields; ++i) {
    // Try to read field descriptor
    if (!file_.read(reinterpret_cast<char*>(&field), sizeof(DB_FIELD))) {
      if (file_.eof()) {
        break;
      }
      setLastError("Failed to read field descriptor");
      return false;
    }

    // Convert field_address from little-endian to host byte order
    field.field_address = leToHost32(field.field_address);

    // Check for terminator
    if (field.field_type == FIELD_TERMINATOR) {
      break;
    }

    // Validate field type
    if (!isValidFieldType(field.field_type)) {
      setLastError("Invalid field type");
      return false;
    }

    fields_.push_back(field);
  }

  // Read terminator
  if (!file_.read(&terminator, 1) || terminator != FIELD_TERMINATOR) {
    setLastError("Missing or invalid field terminator");
    return false;
  }

  return !fields_.empty();
}

bool Dbf::readRecords() {
  records_.clear();
  records_.reserve(header_.records);

  std::vector<char> buffer(header_.record_length);
  file_.seekg(header_.header_length);

  for (uint32_t i = 0; i < header_.records; ++i) {
    if (!file_.read(buffer.data(), header_.record_length)) {
      setLastError("Failed to read record data at index " + std::to_string(i));
      return false;
    }

    Record record;
    record.reserve(fields_.size());

    size_t offset = 1;  // Skip deletion flag
    for (const auto& field : fields_) {
      // Read raw field data
      std::string value(buffer.data() + offset, field.field_length);

      // Process data according to field type
      switch (field.field_type) {
        case 'C':  // Character field - Trim trailing spaces
        case 'N':  // Numeric field - Trim leading and trailing spaces
        case 'F':  // Float field - Trim leading and trailing spaces
        {
          // Remove the trailing spaces first
          auto lastChar = value.find_last_not_of(' ');
          value = (lastChar == std::string::npos)
                      ? ""
                      : value.substr(0, lastChar + 1);

          // For numeric types, you also need to remove leading spaces
          if (field.field_type == 'N' || field.field_type == 'F') {
            auto firstChar = value.find_first_not_of(' ');
            value = (firstChar == std::string::npos) ? "0"
                                                     : value.substr(firstChar);
          }
          break;
        }
        case 'L': {  // Logical field
          value = (value[0] == 'T' || value[0] == 'Y')   ? "T"
                  : (value[0] == 'F' || value[0] == 'N') ? "F"
                                                         : "";
          break;
        }
        default:  // Other types of fields also handle trailing spaces
        {
          auto lastChar = value.find_last_not_of(' ');
          value = (lastChar == std::string::npos)
                      ? ""
                      : value.substr(0, lastChar + 1);
          break;
        }
      }

      record.push_back(value);
      offset += field.field_length;
    }

    records_.push_back(std::move(record));
  }

  return true;
}

std::pair<bool, Dbf::Record> Dbf::getRecord(size_t index) const {
  if (!isOpen_ || index >= records_.size()) {
    return std::make_pair(false, Record());
  }
  return std::make_pair(true, records_[index]);
}

bool Dbf::addRecord(const Record& record) {
  if (!isOpen_ || readOnly_) {
    setLastError("File not open for writing");
    return false;
  }

  if (!validateRecord(record)) {
    setLastError("Invalid record format");
    return false;
  }

  records_.push_back(record);
  header_.records++;
  isModified_ = true;
  return true;
}

bool Dbf::updateRecord(size_t index, const Record& record) {
  if (!isOpen_ || readOnly_ || index >= records_.size()) {
    setLastError("Cannot update record");
    return false;
  }

  if (!validateRecord(record)) {
    setLastError("Invalid record format");
    return false;
  }

  records_[index] = record;
  isModified_ = true;
  return true;
}

bool Dbf::deleteRecord(size_t index) {
  if (!isOpen_ || readOnly_ || index >= records_.size()) {
    setLastError("Cannot delete record");
    return false;
  }

  records_.erase(records_.begin() + index);
  header_.records--;
  isModified_ = true;
  return true;
}

std::string Dbf::getFieldValue(size_t recordIndex, size_t fieldIndex) const {
  if (!isOpen_ || recordIndex >= records_.size() ||
      fieldIndex >= fields_.size()) {
    return std::string();
  }
  return records_[recordIndex][fieldIndex];
}

std::string Dbf::getFieldName(size_t fieldIndex) const {
  if (!isOpen_ || fieldIndex >= fields_.size()) {
    return std::string();
  }
  return std::string(
      reinterpret_cast<const char*>(fields_[fieldIndex].field_name),
      strnlen(reinterpret_cast<const char*>(fields_[fieldIndex].field_name),
              11));
}

char Dbf::getFieldType(size_t fieldIndex) const {
  if (!isOpen_ || fieldIndex >= fields_.size()) {
    return 0;
  }
  return static_cast<char>(fields_[fieldIndex].field_type);
}

uint8_t Dbf::getFieldLength(size_t fieldIndex) const {
  if (!isOpen_ || fieldIndex >= fields_.size()) {
    return 0;
  }
  return fields_[fieldIndex].field_length;
}

uint8_t Dbf::getFieldDecimals(size_t fieldIndex) const {
  if (!isOpen_ || fieldIndex >= fields_.size()) {
    return 0;
  }
  return fields_[fieldIndex].field_decimals;
}

bool Dbf::save() {
  if (!isOpen_ || readOnly_ || !isModified_) {
    return true;
  }

  updateLastUpdateDate();

  // Write header
  if (!writeHeader()) {
    return false;
  }

  // Write fields
  if (!writeFields()) {
    return false;
  }

  // Write records
  if (!writeRecords()) {
    return false;
  }

  // Write EOF marker
  char eof = EOF_MARKER;
  if (!file_.write(&eof, 1)) {
    setLastError("Failed to write EOF marker");
    return false;
  }

  file_.flush();
  isModified_ = false;
  return true;
}

void Dbf::close() {
  if (isOpen_) {
    if (isModified_ && !readOnly_) {
      save();
    }
    file_.close();
    fields_.clear();
    records_.clear();
    std::memset(&header_, 0, sizeof(DB_HEADER));
    isOpen_ = false;
    isModified_ = false;
    readOnly_ = true;
  }
}

bool Dbf::writeHeader() {
  // Convert to little-endian before writing
  DB_HEADER tempHeader = header_;
  tempHeader.records = hostToLe32(tempHeader.records);
  tempHeader.header_length = hostToLe16(tempHeader.header_length);
  tempHeader.record_length = hostToLe16(tempHeader.record_length);

  file_.seekp(0);
  if (!file_.write(reinterpret_cast<const char*>(&tempHeader),
                   sizeof(DB_HEADER))) {
    setLastError("Failed to write header");
    return false;
  }
  return true;
}

bool Dbf::writeFields() {
  file_.seekp(sizeof(DB_HEADER));

  // Write field descriptors
  for (const auto& field : fields_) {
    DB_FIELD tempField = field;
    tempField.field_address = hostToLe32(tempField.field_address);

    if (!file_.write(reinterpret_cast<const char*>(&tempField),
                     sizeof(DB_FIELD))) {
      setLastError("Failed to write field descriptor");
      return false;
    }
  }

  // Write field terminator
  char terminator = FIELD_TERMINATOR;
  if (!file_.write(&terminator, 1)) {
    setLastError("Failed to write field terminator");
    return false;
  }

  return true;
}

bool Dbf::writeRecords() {
  file_.seekp(header_.header_length);

  std::vector<char> buffer(header_.record_length);
  for (const auto& record : records_) {
    // Set deletion flag
    buffer[0] = VALID_RECORD;

    // Write field values
    size_t offset = 1;
    for (size_t i = 0; i < fields_.size(); ++i) {
      std::string formatted = formatField(record[i], i);
      std::memcpy(buffer.data() + offset, formatted.c_str(),
                  fields_[i].field_length);
      offset += fields_[i].field_length;
    }

    if (!file_.write(buffer.data(), header_.record_length)) {
      setLastError("Failed to write record");
      return false;
    }
  }

  return true;
}

void Dbf::updateLastUpdateDate() {
  time_t now = time(nullptr);
  tm* ltm = localtime(&now);
  header_.last_update[0] = ltm->tm_year - 100;  // Years since 1900
  header_.last_update[1] = ltm->tm_mon + 1;     // Month (1-12)
  header_.last_update[2] = ltm->tm_mday;        // Day (1-31)
}

bool Dbf::validateHeader() const {
  return (header_.header_length >= sizeof(DB_HEADER) &&
          header_.record_length > 1 &&  // At least deletion flag + 1 byte
          header_.version != 0);
}

bool Dbf::validateRecord(const Record& record) const {
  if (record.size() != fields_.size()) {
    return false;
  }

  for (size_t i = 0; i < record.size(); ++i) {
    if (record[i].length() > fields_[i].field_length) {
      return false;
    }
  }

  return true;
}

std::string Dbf::formatField(const std::string& value,
                             size_t fieldIndex) const {
  const auto& field = fields_[fieldIndex];
  std::string result(field.field_length, ' ');

  if (!value.empty()) {
    size_t copyLen =
        std::min(value.length(), static_cast<size_t>(field.field_length));
    std::memcpy(&result[0], value.c_str(), copyLen);
  }

  return result;
}

void Dbf::setLastError(const std::string& error) {
  lastError_ = error;
  std::cout << "DBF Error: " << error << std::endl;
}

bool Dbf::isValidFieldType(char type) {
  static const std::string validTypes = "CNFDLMBGPYTI@O+V";
  return validTypes.find(type) != std::string::npos;
}

uint16_t Dbf::calculateRecordLength(const std::vector<DB_FIELD>& fields) {
  uint16_t length = 1;  // Deletion flag
  for (const auto& field : fields) {
    length += field.field_length;
  }
  return length;
}

bool Dbf::create(const std::string& filename,
                 const std::vector<DB_FIELD>& fields) {
  if (fields.empty()) {
    setLastError("No fields specified");
    return false;
  }

  // Close the currently open file
  close();

  // Create a new file in write mode
  file_.open(filename, std::ios::binary | std::ios::out);
  if (!file_) {
    setLastError("Cannot create file: " + filename);
    return false;
  }

  // Initialize header data
  std::memset(&header_, 0, sizeof(DB_HEADER));
  header_.version = 0x03;  // dBASE III
  header_.records = 0;
  header_.header_length = sizeof(DB_HEADER) + fields.size() * sizeof(DB_FIELD) +
                          1;  // +1 for terminator
  header_.record_length = calculateRecordLength(fields);

  // Record field information
  fields_ = fields;

  // Calculate field offset
  uint32_t offset = 1;  // Skip deletion flag
  for (auto& field : fields_) {
    field.field_address = offset;
    offset += field.field_length;
  }

  // Write header
  if (!writeHeader()) {
    file_.close();
    return false;
  }

  // Write field descriptor
  if (!writeFields()) {
    file_.close();
    return false;
  }

  // Write EOF marker
  char eof = EOF_MARKER;
  if (!file_.write(&eof, 1)) {
    setLastError("Failed to write EOF marker");
    file_.close();
    return false;
  }

  // Update file status
  filename_ = filename;
  readOnly_ = false;
  isOpen_ = true;
  isModified_ = false;

  // Close and reopen the file in read-write mode
  file_.close();
  return open(filename, false);
}