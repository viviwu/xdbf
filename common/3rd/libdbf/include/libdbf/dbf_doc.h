#ifndef DBF_DOC_H
#define DBF_DOC_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>


namespace dbf
{

// DBF field types
enum class FieldType : char
{
  Character = 'C',
  Numeric = 'N',
  Date = 'D',
  Logical = 'L',
  Memo = 'M'
};

#pragma pack(push, 1)
struct DbHeader
{
  uint8_t  version;          // dBase version
  uint8_t  last_update[3];   // YYMMDD format
  uint32_t num_records;      // Number of records
  uint16_t header_length;    // Length of header
  uint16_t record_length;    // Length of each record
  uint8_t  reserved1[2];     // Reserved
  uint8_t  transaction;      // Transaction flag
  uint8_t  encryption;       // Encryption flag
  uint8_t  reserved2[12];    // Reserved for multi-user
  uint8_t  mdx_flag;         // Production MDX flag
  uint8_t  language_driver;  // Language driver
  uint8_t  reserved3[2];     // Reserved
};

struct DbField
{
  char     name[11];       // Field name
  char     type;           // Field type
  uint32_t address;        // Field data address
  uint8_t  length;         // Field length
  uint8_t  decimal_count;  // Decimal count
  uint8_t  reserved1[2];   // Reserved
  uint32_t offset;         // Offset in record
  uint8_t  reserved2[7];   // Reserved
  uint8_t  mdx_flag;       // Production MDX flag
};
#pragma pack(pop)

class DbfDoc
{
 public:
  // Constructors & Destructor
  DbfDoc();
  ~DbfDoc();

  // File operations
  bool Open(const std::string& filename, bool read_only = true);
  bool Create(const std::string& filename, const std::vector<DbField>& fields);
  void Close();

  // Record operations
  bool ReadRecord(char* record, size_t len);
  bool WriteRecord(const char* record, size_t len);
  bool WriteRecordWithFlag(const char* record, size_t len);

  // Field operations
  std::string GetFieldValue(const char* record, int column) const;
  size_t      GetFieldOffset(int column) const;
  size_t      GetFieldSize(int column) const;
  char        GetFieldType(int column) const;
  std::string GetFieldName(int column) const;
  int         GetFieldDecimals(int column) const;

  // Information getters
  size_t      GetNumRecords() const;
  size_t      GetNumFields() const;
  size_t      GetRecordLength() const;
  size_t      GetHeaderLength() const;
  std::string GetVersion() const;
  std::string GetLastUpdateDate() const;
  bool        IsMemo() const;

  // Position operations
  bool SetRecordPos(int pos);
  int  GetCurrentRecordPos() const;

 private:
  // Internal helper functions
  bool        ReadHeaderInfo();
  bool        ReadFieldInfo();
  bool        WriteHeaderInfo();
  bool        WriteFieldInfo();
  std::string GetDbVersion(uint8_t version) const;

  // Member variables
  std::unique_ptr<DbHeader> header_;
  std::vector<DbField>      fields_;
  int                       file_handle_;
  int                       current_record_;
  bool                      is_open_;
};

}  // namespace dbf

#endif  // DBF_DOC_H