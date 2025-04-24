//
// Created by dejavu on 2025/4/2.
//

#ifndef DBF_DOC_H
#define DBF_DOC_H

#include "dbf_struct.h"
#include <string>
#include <vector>
#include <fstream>

class DbfDoc {
public:
    DbfDoc();
    ~DbfDoc();

    // Open and parse an existing DBF file
    bool open(const std::string& filename);

    // Save changes to the current file
    bool save();

    // Save to a new file
    bool saveAs(const std::string& filename);

    // Get the number of records
    uint32_t getRecordCount() const { return header_.num_records; }

    // Get field definitions
    const std::vector<DBFField>& getFields() const { return fields_; }

    // Read a specific record (returns empty vector if index is invalid)
    std::vector<std::string> readRecord(uint32_t index) const;

    // Modify a record (returns false if index is invalid or data size mismatches fields)
    bool modifyRecord(uint32_t index, const std::vector<std::string>& data);

    // Add a new record (appends to the end)
    bool addRecord(const std::vector<std::string>& data);

    // Mark a record as deleted (returns false if index is invalid)
    bool deleteRecord(uint32_t index);

    // Create a new DBF file with given fields (overwrites if file exists)
    bool create(const std::string& filename, const std::vector<DBFField>& fields);

    void debugPrint() const; // New: For debugging

private:
    std::string filename_;              // Current file path
    DBFHeader header_;                  // File header
    std::vector<DBFField> fields_;      // Field definitions
    std::vector<char> data_;            // Raw data records
    bool isModified_;                   // Track if changes need saving

    // Helper to update header's last update field (YYMMDD)
    void updateLastUpdate();

    // Helper to calculate record length from fields
    uint16_t calculateRecordLength() const;

    // Helper to write header and fields to file
    bool writeHeaderAndFields(std::ofstream& ofs) const;

    // Helper to validate data against fields
    bool validateData(const std::vector<std::string>& data) const;

    bool validateHeader() const; // New: Validate header consistency

};

#endif // DBF_DOC_H