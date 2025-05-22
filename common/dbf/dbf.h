#ifndef DBF_HPP
#define DBF_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>

#pragma pack(push, 1)

// Keep original DBF structures from dbf.h
struct DB_HEADER {
  uint8_t version; // Byte: 0; dBase version
  uint8_t last_update[3]; // Byte: 1-3; date of last update
  uint32_t records; // Byte: 4-7; number of records
  uint16_t header_length; // Byte: 8-9; header length
  uint16_t record_length; // Byte: 10-11; record length
  uint8_t reserved01[2]; // Byte: 12-13; reserved
  uint8_t transaction; // Byte: 14; transaction flag
  uint8_t encryption; // Byte: 15; encryption flag
  uint8_t reserved02[12]; // Byte: 16-27; multi-user environment
  uint8_t mdx; // Byte: 28; MDX flag
  uint8_t language; // Byte: 29; language driver
  uint8_t reserved03[2]; // Byte: 30-31; reserved
};

struct DB_FIELD {
  uint8_t field_name[11]; // Byte: 0-10; field name
  uint8_t field_type; // Byte: 11; field type
  uint32_t field_address; // Byte: 12-15; field address
  uint8_t field_length; // Byte: 16; field length
  uint8_t field_decimals; // Byte: 17; decimal count
  uint8_t reserved2[13]; // Byte: 18-30; reserved
  uint8_t mdx; // Byte: 31; MDX flag
};

#pragma pack(pop)

class Dbf {
public:
  using Record = std::vector<std::string>;
  using Records = std::vector<Record>;

  // Constructors & Destructor
  Dbf();

  ~Dbf();

  // File Operations
  bool open(const std::string& filename, bool readOnly = true);

  const std::vector<DB_FIELD>& getFields() const { return fields_; }

  bool create(const std::string& filename, const std::vector<DB_FIELD>& fields);

  bool save();

  void close();

  // Record Operations
  std::pair<bool, Record> getRecord(size_t index) const;

  bool addRecord(const Record& record);

  bool updateRecord(size_t index, const Record& record);

  bool deleteRecord(size_t index);

  // Field Operations
  std::string getFieldValue(size_t recordIndex, size_t fieldIndex) const;

  std::string getFieldName(size_t fieldIndex) const;

  char getFieldType(size_t fieldIndex) const;

  uint8_t getFieldLength(size_t fieldIndex) const;

  uint8_t getFieldDecimals(size_t fieldIndex) const;

  // Status
  bool isOpen() const { return isOpen_; }
  bool isReadOnly() const { return readOnly_; }
  bool isModified() const { return isModified_; }
  size_t getRecordCount() const { return header_.records; }
  size_t getFieldCount() const { return fields_.size(); }

  // Error handling
  std::string getLastError() const { return lastError_; }

private:
  // Internal methods
  bool readHeader();

  bool writeHeader();

  bool readFields();

  bool writeFields();

  bool readRecords();

  bool writeRecords();

  bool validateHeader() const;

  bool validateRecord(const Record& record) const;

  std::string formatField(const std::string& value, size_t fieldIndex) const;

  // Utility methods
  void updateLastUpdateDate();

  void setLastError(const std::string& error);

  static bool isValidFieldType(char type);

  static uint16_t calculateRecordLength(const std::vector<DB_FIELD>& fields);

private:
  // Endian conversion helpers
  static uint16_t swapBytes16(uint16_t value) {
      return (value << 8) | (value >> 8);
  }

  static uint32_t swapBytes32(uint32_t value) {
      return  ((value & 0xFF000000) >> 24) |
              ((value & 0x00FF0000) >> 8)  |
              ((value & 0x0000FF00) << 8)  |
              ((value & 0x000000FF) << 24);
  }

  static bool isBigEndian() {
      static const uint16_t test = 0x0001;
      return *reinterpret_cast<const uint8_t*>(&test) == 0;
  }

  // Convert from little-endian (file) to host byte order
  static uint16_t leToHost16(uint16_t value) {
      return isBigEndian() ? swapBytes16(value) : value;
  }

  static uint32_t leToHost32(uint32_t value) {
      return isBigEndian() ? swapBytes32(value) : value;
  }

  // Convert from host byte order to little-endian (file)
  static uint16_t hostToLe16(uint16_t value) {
      return isBigEndian() ? swapBytes16(value) : value;
  }

  static uint32_t hostToLe32(uint32_t value) {
      return isBigEndian() ? swapBytes32(value) : value;
  }

private:
  // Member variables
  std::string filename_;
  DB_HEADER header_;
  std::vector<DB_FIELD> fields_;
  Records records_;
  bool isOpen_;
  bool readOnly_;
  bool isModified_;
  std::string lastError_;
  std::fstream file_;

  // Constants
  static const char DELETED_RECORD = '*';
  static const char VALID_RECORD = ' ';
  static const char FIELD_TERMINATOR = 0x0D;
  static const char EOF_MARKER = 0x1A;
};

#endif // DBF_HPP