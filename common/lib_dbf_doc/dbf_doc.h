#ifndef DBF_DOC_H
#define DBF_DOC_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

namespace dbf {

// Maintain consistency with original DBF structures
#pragma pack(push, 1)
struct DB_HEADER {
    uint8_t  version;           // Byte: 0; dBase version
    uint8_t  last_update[3];    // Byte: 1-3; date of last update
    uint32_t records;           // Byte: 4-7; number of records in table
    uint16_t header_length;     // Byte: 8-9; number of bytes in the header
    uint16_t record_length;     // Byte: 10-11; number of bytes in the record
    uint8_t  reserved01[2];     // Byte: 12-13; reserved
    uint8_t  transaction;       // Byte: 14; incomplete transaction flag
    uint8_t  encryption;        // Byte: 15; encryption flag
    uint8_t  reserved02[12];    // Byte: 16-27; reserved for multi-user
    uint8_t  mdx;              // Byte: 28; production MDX flag
    uint8_t  language;         // Byte: 29; language driver
    uint8_t  reserved03[2];    // Byte: 30-31; reserved
};

struct DB_FIELD {
    uint8_t  field_name[11];    // Byte: 0-10; field name in ASCII
    uint8_t  field_type;        // Byte: 11; field type in ASCII
    uint32_t field_address;     // Byte: 12-15; field data address
    uint8_t  field_length;      // Byte: 16; field length
    uint8_t  field_decimals;    // Byte: 17; field decimal count
    uint8_t  reserved2[13];     // Byte: 18-30; reserved
    uint8_t  mdx;              // Byte: 31; production MDX flag
};
#pragma pack(pop)

class DbfDoc {
public:
    // Constructors & destructor
    DbfDoc();
    ~DbfDoc();

    // Disable copying
    DbfDoc(const DbfDoc&) = delete;
    DbfDoc& operator=(const DbfDoc&) = delete;

    // Enable moving
    DbfDoc(DbfDoc&&) = default;
    DbfDoc& operator=(DbfDoc&&) = default;

    // File operations
    bool open(const std::string& filename, bool read_only = true);
    bool create(const std::string& filename, const std::vector<DB_FIELD>& fields);
    void close();

    // Record operations
    bool readRecord(size_t index, std::vector<char>& record) const;
    bool writeRecord(const std::vector<char>& record);
    bool updateRecord(size_t index, const std::vector<char>& record);
    bool deleteRecord(size_t index);

    // Field operations
    std::string getFieldValue(const std::vector<char>& record, size_t fieldIndex) const;
    size_t getFieldAddress(size_t fieldIndex) const;
    size_t getFieldLength(size_t fieldIndex) const;
    char getFieldType(size_t fieldIndex) const;
    std::string getFieldName(size_t fieldIndex) const;
    int getFieldDecimals(size_t fieldIndex) const;

    // Information getters
    size_t getRecordCount() const;
    size_t getFieldCount() const;
    size_t getRecordLength() const;
    size_t getHeaderLength() const;
    std::string getVersion() const;
    std::string getLastUpdateDate() const;
    bool isMemo() const;

private:
    // Internal helper methods
    bool readHeader();
    bool readFields();
    bool writeHeader();
    bool writeFields();
    std::string getVersionString(uint8_t version) const;

    // Record management
    bool loadAllRecords();
    bool saveAllRecords();

private:
    std::fstream file_;
    std::unique_ptr<DB_HEADER> header_;
    std::vector<DB_FIELD> fields_;
    std::vector<std::vector<char>> records_;
    bool isOpen_;
    bool isModified_;
    bool readOnly_;
};

} // namespace dbf

#endif // DBF_DOC_H