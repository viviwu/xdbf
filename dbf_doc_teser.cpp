#include <stdio.h>
#include <stdlib.h>

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "atx/at_util.h"
#include "atx/atx_struct.h"
#include "common/utils/c_utils.h"
#include "dbf_doc.h"

void debugLogImpl(std::ostream&) {
}

template <typename T, typename... Args>
void debugLogImpl(std::ostream& os, T&& t, Args&&... args) {
  os << std::forward<T>(t);
  debugLogImpl(os, std::forward<Args>(args)...);
}

template <typename... Args>
void debugLog(Args&&... args) {
  std::ostringstream oss;
  debugLogImpl(oss, std::forward<Args>(args)...);
  std::cout << "[DEBUG] " << oss.str() << std::endl;
}

#define DEBUG_LOG(...)                                        \
  do                                                          \
  {                                                           \
    std::ostringstream oss;                                   \
    oss << "[DEBUG][" << __FILE__ << ":" << __LINE__ << "] "; \
    oss << __VA_ARGS__;                                       \
    std::cout << oss.str() << std::endl;                      \
  } while (0)

class DbfDocTester {
public:
  static bool RunAllTests() {
    std::cout << "Starting DBF document tests...\n\n";

    bool success = true;
    success &= TestCreateAndSave();
    // success &= TestOpenAndRead();
    // success &= TestAddRecord();
    // success &= TestModifyAndSave();
    // success &= TestDeleteRecord();

    std::cout << "\nTest results: " << (success ? "All PASSED" : "Some FAILED")
        << "\n";
    return success;
  }

private:
  static bool TestCreateAndSave() {
    std::cout << "Testing create and save functions...\n";

    DbfDoc doc;
    std::vector<DB_FIELD> fields;

    // Setup test fields
    DB_FIELD nameField = {0};
    std::strcpy(reinterpret_cast<char*>(nameField.field_name), "NAME");
    nameField.field_type = 'C';
    nameField.field_length = 20;

    DB_FIELD ageField = {0};
    std::strcpy(reinterpret_cast<char*>(ageField.field_name), "AGE");
    ageField.field_type = 'N';
    ageField.field_length = 3;

    fields.push_back(nameField);
    fields.push_back(ageField);

    bool success = doc.create("test.dbf", fields);
    assert(success && "Failed to create DBF file");

    // Add test records
    std::vector<std::string> record1 = {"John Doe", "30"};
    std::vector<std::string> record2 = {"Jane Smith", "25"};

    success &= doc.addRecord(record1);
    success &= doc.addRecord(record2);
    success &= doc.save();

    std::cout << (success ? "PASSED" : "FAILED") << "\n";
    return success;
  }

  static std::string testFileName() {
    char filename[24] = {0};
    char dbf_file_path[256];
    today_order_algo_filename(filename, sizeof(filename));
    printf("filename: %s\n", filename);
    // sprintf(dbf_file_path, "D:\\Program Files\\SJZQ-FZHJ\\OrderScan\\%s", filename);
    // printf("dbf_file_path:%s \n", dbf_file_path);
    return std::string(filename);
  }

  static bool TestOpenAndRead() {
    std::cout << "Testing open and read functions...\n";

    auto filename = testFileName();
    DbfDoc doc;
    bool success = doc.open(filename);
    assert(success && "Failed to open DBF file");

    // Test field count
    auto fields = doc.getFields();
    printf("doc.getFieldCount()=%zd \n", fields.size());
    assert(fields.size() == 11);

    // Test record count
    auto record_cnt = doc.getRecordCount();
    printf("doc.getRecordCount()=%d \n", record_cnt);
    // assert(record_cnt == 2);

    // Read and verify first record
    for (int r = 0; r < record_cnt; r++) {
      auto record = doc.readRecord(r);
      if (record.size()) {
        for (int i = 0; i < record.size(); i++) {
          auto field = fields[i];
          debugLog(field.field_name, ": ", record[i], ", field_type:",
                   field.field_type);
        }
        success = true;
      } else {
        if (record.size() == 0)
          debugLog("record was marked as deleted at row:", r);
        else
          success = false;
      }
    }

    std::cout << (success ? "PASSED" : "FAILED") << "\n";
    return success;
  }

  static std::vector<std::string> newRecordData() {
    char account_no[] = "10001253";
    char symbol[] = "300676.SZ";
    char side[] = "1";
    char order_qty[] = "200";
    char ord_type[] = "201";
    char algo_param[] = "PriceTypeI=0:priceF=55.95";
    char eff_time[18] = {0}; //YYYYMMDDHHMMSSsss
    char exp_time[18] = {0}; //YYYYMMDDHHMMSSsss
    char timestamp_sec[15] = {0};
    char cur_timestamp[18] = {0}; //YYYYMMDDHHMMSSsss

    today_fixed_time_string(eff_time, sizeof(eff_time), "093001000");
    today_fixed_time_string(exp_time, sizeof(exp_time), "145659000");
    formatted_current_timestamp_second(timestamp_sec, sizeof(timestamp_sec));
    formatted_current_timestamp_millisecond(cur_timestamp,
                                            sizeof(cur_timestamp));

    printf("eff_time: %s\n", eff_time);
    printf("exp_time: %s\n", exp_time);
    printf("timestamp_sec: %s\n", timestamp_sec);
    printf("cur_timestamp: %s\n", cur_timestamp);

    std::vector<std::string> newRecord = {cur_timestamp, account_no, symbol,
                                          side, order_qty, ord_type,
                                          eff_time, exp_time, "1", "1",
                                          algo_param};
    return newRecord;
  }

  static bool TestAddRecord() {
    std::cout << "Testing add record function...\n";

    auto filename = testFileName();
    DbfDoc doc;
    bool success = doc.open(filename);
    assert(success && "Failed to open DBF file");

    // 保存原始记录数
    size_t originalCount = doc.getRecordCount();

    // 添加新记录
    std::vector<std::string> newRecord = newRecordData();
    success &= doc.addRecord(newRecord);

    // 验证记录数增加
    assert(doc.getRecordCount() == originalCount + 1);

    // 保存更改
    success &= doc.save();

    // 重新打开文件验证
    success &= TestOpenAndRead();

    std::cout << (success ? "PASSED" : "FAILED") << "\n";
    return success;
  }

  static bool TestModifyAndSave() {
    std::cout << "Testing modify and save functions...\n";

    auto filename = testFileName();
    DbfDoc doc;
    bool success = doc.open(filename);
    assert(success && "Failed to open DBF file");

    // Modify first record
    std::vector<std::string> modifiedRecord = newRecordData();;
    success &= doc.modifyRecord(0, modifiedRecord);
    success &= doc.save();

    // Verify modification
    success &= TestOpenAndRead();

    std::cout << (success ? "PASSED" : "FAILED") << "\n";
    return success;
  }

  static bool TestDeleteRecord() {
    std::cout << "Testing delete record function...\n";

    auto filename = testFileName();
    DbfDoc doc;
    bool success = doc.open(filename);
    assert(success && "Failed to open DBF file");

    // Delete first record
    success &= doc.deleteRecord(0);
    success &= doc.save();

    // Verify deletion
    auto record = doc.readRecord(0);
    assert(!record.size() && "Record should be marked as deleted");

    std::cout << (success ? "PASSED" : "FAILED") << "\n";
    return success;
  }
};

int main() {
  try {
    return DbfDocTester::RunAllTests() ? 0 : 1;
  } catch (const std::exception& e) {
    std::cerr << "Test failed with exception: " << e.what() << "\n";
    return 1;
  }
}