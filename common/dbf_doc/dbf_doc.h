//
// Created by dejavu on 2025/4/2.
//

#ifndef DBF_DOC_H
#define DBF_DOC_H

#include <fstream>
#include <string>
#include <vector>

#include "dbf_def.h"


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

  // Open and parse an existing DBF file
  bool open(const std::string& filename, bool read_only = true);

  // Save changes to the current file
  bool save();

  // Save to a new file
  bool saveAs(const std::string& filename);

  // Get the number of records
  uint32_t getRecordCount() const { return header_.num_records; }

  // Get field definitions
  const std::vector<DB_FIELD>& getFields() const { return fields_; }

  // Read a specific record (returns empty vector if index is invalid)
  std::vector<std::string> readRecord(uint32_t index) const;

  // Modify a record (returns false if index is invalid or data size mismatches fields)
  bool modifyRecord(uint32_t index, const std::vector<std::string>& data);

  // Add a new record (appends to the end)
  bool addRecord(const std::vector<std::string>& data);

  // Mark a record as deleted (returns false if index is invalid)
  bool deleteRecord(uint32_t index);

  // Create a new DBF file with given fields (overwrites if file exists)
  bool create(const std::string& filename, const std::vector<DB_FIELD>& fields);

private:
  std::string filename_; // Current file path
  DB_HEADER header_; // File header
  std::vector<DB_FIELD> fields_; // Field definitions
  std::vector<char> data_; // Raw data records
  bool isModified_; // Track if changes need saving
  bool readOnly_;

  // Helper to update header's last update field (YYMMDD)
  void updateLastUpdate();

  // Helper to calculate record length from fields
  uint16_t calculateRecordLength() const;

  // Helper to write header and fields to file
  bool writeHeaderAndFields(std::ofstream& ofs) const;

  // Helper to validate data against fields
  bool validateData(const std::vector<std::string>& data) const;

  bool validateHeader() const; // New: Validate header consistency

  void debugPrint() const; // New: For debugging

  // a func to debug DB_HEADER member value info
  void DbfDoc::debugPrintHeader();
};

#endif  // DBF_DOC_H