#include "dbf_doc.h"

#include <cstring>
#include <ctime>
#include <system_error>


#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#define O_BINARY 0x8000
#else
#include <unistd.h>
#endif

namespace dbf
{

DbfDoc::DbfDoc() : header_(nullptr), file_handle_(-1), current_record_(0), is_open_(false)
{
}

DbfDoc::~DbfDoc()
{
  Close();
}

bool DbfDoc::Open(const std::string& filename, bool read_only)
{
  if (is_open_)
  {
    Close();
  }

  int flags = read_only ? O_RDONLY : O_RDWR;
  flags |= O_BINARY;

  file_handle_ = ::open(filename.c_str(), flags);
  if (file_handle_ == -1)
  {
    return false;
  }

  if (!ReadHeaderInfo() || !ReadFieldInfo())
  {
    Close();
    return false;
  }

  is_open_ = true;
  current_record_ = 0;
  return true;
}

bool DbfDoc::ReadHeaderInfo()
{
  header_ = std::make_unique<DbHeader>();

  if (::read(file_handle_, header_.get(), sizeof(DbHeader)) != sizeof(DbHeader))
  {
    return false;
  }

  // Convert from little endian if necessary
  header_->num_records = _byteswap_ulong(header_->num_records);
  header_->header_length = _byteswap_ushort(header_->header_length);
  header_->record_length = _byteswap_ushort(header_->record_length);

  return true;
}

bool DbfDoc::ReadFieldInfo()
{
  if (!header_ || !is_open_)
  {
    return false;
  }

  int columns = static_cast<int>((header_->header_length - sizeof(DbHeader) - 1) / sizeof(DbField));
  if (columns <= 0)
  {
    return false;
  }

  // Seek to field info position
  if (::lseek(file_handle_, sizeof(DbHeader), SEEK_SET) == -1)
  {
    return false;
  }

  // Read field info
  fields_.resize(columns);
  if (::read(file_handle_, fields_.data(), columns * sizeof(DbField)) == -1)
  {
    return false;
  }

  // Calculate field offsets
  size_t offset = 1;  // First byte is deletion flag
  for (auto& field : fields_)
  {
    field.offset = offset;
    offset += field.length;
  }

  return true;
}

bool DbfDoc::WriteFieldInfo()
{
  if (!is_open_ || fields_.empty())
  {
    return false;
  }

  if (::lseek(file_handle_, sizeof(DbHeader), SEEK_SET) == -1)
  {
    return false;
  }

  if (::write(file_handle_, fields_.data(), fields_.size() * sizeof(DbField)) == -1)
  {
    return false;
  }

  // Write terminator
  return (::write(file_handle_, "\r\0", 2) != -1);
}

bool DbfDoc::Create(const std::string& filename, const std::vector<DbField>& fields)
{
  Close();

  file_handle_ = ::open(filename.c_str(), O_WRONLY | O_BINARY | O_CREAT, 0644);
  if (file_handle_ == -1)
  {
    return false;
  }

  // Initialize header
  header_ = std::make_unique<DbHeader>();
  memset(header_.get(), 0, sizeof(DbHeader));

  // Calculate record length
  uint16_t record_length = 1;  // 1 byte for deletion flag
  for (const auto& field : fields)
  {
    record_length += field.length;
  }

  header_->version = 0x03;  // FoxBase+/dBASE III+
  header_->record_length = record_length;
  header_->header_length = sizeof(DbHeader) + fields.size() * sizeof(DbField) + 2;
  header_->num_records = 0;

  // Write header
  if (!WriteHeaderInfo())
  {
    Close();
    return false;
  }

  // Copy and write fields
  fields_ = fields;
  if (!WriteFieldInfo())
  {
    Close();
    return false;
  }

  is_open_ = true;
  current_record_ = 0;
  return true;
}

bool DbfDoc::WriteRecord(const char* record, size_t len)
{
  if (!is_open_ || len != header_->record_length - 1)
  {
    return false;
  }

  // Calculate exact position
  off_t offset = header_->header_length + header_->num_records * header_->record_length;

  if (::lseek(file_handle_, offset, SEEK_SET) == -1)
  {
    return false;
  }

  // Write deletion flag + record
  if (::write(file_handle_, " ", 1) == -1 || ::write(file_handle_, record, len) == -1)
  {
    return false;
  }

  // Update record count
  header_->num_records++;
  return WriteHeaderInfo();
}

bool DbfDoc::WriteRecordWithFlag(const char* record, size_t len)
{
  if (!is_open_ || !header_ || len != header_->record_length)
  {
    return false;
  }

  // Calculate exact position
  off_t offset = header_->header_length + header_->num_records * header_->record_length;

  if (::lseek(file_handle_, offset, SEEK_SET) == -1)
  {
    return false;
  }

  // Write complete record including deletion flag
  if (::write(file_handle_, record, header_->record_length) == -1)
  {
    return false;
  }

  // Update record count
  header_->num_records++;
  return WriteHeaderInfo();
}

std::string DbfDoc::GetFieldValue(const char* record, int column) const
{
  if (!record || column < 0 || column >= fields_.size())
  {
    return {};
  }

  const auto& field = fields_[column];
  std::string value(record + field.offset, field.length);

  // Trim right spaces
  while (!value.empty() && value.back() == ' ')
  {
    value.pop_back();
  }

  return value;
}

size_t DbfDoc::GetFieldOffset(int column) const
{
  if (column < 0 || column >= fields_.size())
  {
    return 0;
  }
  return fields_[column].offset;
}

size_t DbfDoc::GetFieldSize(int column) const
{
  if (column < 0 || column >= fields_.size())
  {
    return 0;
  }
  return fields_[column].length;
}

char DbfDoc::GetFieldType(int column) const
{
  if (column < 0 || column >= fields_.size())
  {
    return 0;
  }
  return fields_[column].type;
}

std::string DbfDoc::GetFieldName(int column) const
{
  if (column < 0 || column >= fields_.size())
  {
    return {};
  }
  return std::string(fields_[column].name);
}

int DbfDoc::GetFieldDecimals(int column) const
{
  if (column < 0 || column >= fields_.size())
  {
    return 0;
  }
  return fields_[column].decimal_count;
}

}  // namespace dbf