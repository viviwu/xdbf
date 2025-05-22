#include "dbf.h"
#include <iostream>
#include <cassert>
#include <iomanip>

class DbfTester {
public:
  static bool RunAllTests() {
    std::cout << "开始DBF文件操作测试...\n\n";

    bool success = true;
    try {
      success &= TestCreateAndSave();
      success &= TestOpenAndRead();
      success &= TestAddRecord();
      success &= TestModifyRecord();
      // success &= TestDeleteRecord();
    } catch (const std::exception& e) {
      std::cout << "测试过程中发生异常: " << e.what() << std::endl;
      return false;
    }

    std::cout << "\n测试结果: " << (success ? "全部通过" : "存在失败") << "\n";
    return success;
  }

private:
  static void PrintError(const Dbf& doc, const std::string& operation) {
    std::cout << "操作失败 [" << operation << "]: " << doc.getLastError() <<
        std::endl;
  }

  static bool TestCreateAndSave() {
    std::cout << "测试创建和保存功能...\n";

    Dbf doc;
    std::vector<DB_FIELD> fields;

    // 创建字段描述符
    DB_FIELD nameField = {0};
    std::strncpy(reinterpret_cast<char*>(nameField.field_name), "NAME", 11);
    nameField.field_type = 'C';
    nameField.field_length = 20;
    nameField.field_decimals = 0;
    fields.push_back(nameField);

    DB_FIELD ageField = {0};
    std::strncpy(reinterpret_cast<char*>(ageField.field_name), "AGE", 11);
    ageField.field_type = 'N';
    ageField.field_length = 3;
    ageField.field_decimals = 0;
    fields.push_back(ageField);

    // 创建文件
    if (!doc.create("test.dbf", fields)) {
      PrintError(doc, "create");
      return false;
    }

    // 添加测试记录
    std::vector<std::string> record1 = {"Zhang San", "30"};
    std::vector<std::string> record2 = {"Li Si", "25"};

    if (!doc.addRecord(record1)) {
      PrintError(doc, "addRecord1");
      return false;
    }

    if (!doc.addRecord(record2)) {
      PrintError(doc, "addRecord2");
      return false;
    }

    if (!doc.save()) {
      PrintError(doc, "save");
      return false;
    }

    doc.close();
    std::cout << "通过\n";
    return true;
  }

  static bool TestOpenAndRead() {
    std::cout << "测试打开和读取功能...\n";

    Dbf doc;
    if (!doc.open("test.dbf", true)) {
      PrintError(doc, "open");
      return false;
    }

    // 验证记录数
    if (doc.getRecordCount() != 2) {
      std::cout << "记录数不匹配: 期望 2, 实际 " << doc.getRecordCount() << std::endl;
      return false;
    }

    // 验证字段数
    if (doc.getFieldCount() != 2) {
      std::cout << "字段数不匹配: 期望 2, 实际 " << doc.getFieldCount() << std::endl;
      return false;
    }

    // 读取并验证第一条记录
    auto record = doc.getRecord(0);
    if (!record.first) {
      PrintError(doc, "getRecord");
      return false;
    }

#ifdef _DEBUG
    auto fields = doc.getFields();
    for (int i = 0; i < record.second.size(); ++i) {
      auto field = fields[i];
      auto value = record.second[i];
      printf("Field[%zu] type=%c value=<%s>\n",
             i, field.field_type, value.c_str());
    }
#endif

    if (record.second[0] != "Zhang San" || record.second[1] != "30") {
      std::cout << "记录内容不匹配: " << record.second[0] << ", " << record.second[1]
          << std::endl;
      return false;
    }

    // 验证字段名称
    if (doc.getFieldName(0) != "NAME" || doc.getFieldName(1) != "AGE") {
      std::cout << "字段名称不匹配: " << doc.getFieldName(0) << ", " << doc.
          getFieldName(1) << std::endl;
      return false;
    }

    doc.close();
    std::cout << "通过\n";
    return true;
  }

  static bool TestAddRecord() {
    std::cout << "测试添加记录功能...\n";
    bool success = true;

    try {
      Dbf doc;
      success &= doc.open("test.dbf", false);
      if (!success) {
        std::cout << "打开DBF文件失败: " << doc.getLastError() << std::endl;
        return false;
      }

      size_t originalCount = doc.getRecordCount();

      // 添加新记录
      std::vector<std::string> newRecord = {"Wang Wu", "28"};
      success &= doc.addRecord(newRecord);
      success &= doc.save();

      // 验证记录数增加
      assert(doc.getRecordCount() == originalCount + 1);

      // 验证新记录内容
      auto record = doc.getRecord(doc.getRecordCount() - 1);
      if (record.first) {
        assert(record.second[0] == "Wang Wu");
        assert(record.second[1] == "28");
      } else {
        success = false;
      }
    } catch (const std::exception& e) {
      std::cout << "异常: " << e.what() << std::endl;
      return false;
    }

    std::cout << (success ? "通过" : "失败") << "\n";
    return success;
  }

  static bool TestModifyRecord() {
    std::cout << "测试修改记录功能...\n";
    bool success = true;

    try {
      Dbf doc;
      success &= doc.open("test.dbf", false);
      if (!success) {
        std::cout << "打开DBF文件失败: " << doc.getLastError() << std::endl;
        return false;
      }

      // 修改第一条记录
      std::vector<std::string> modifiedRecord = {"Zhang San Jr", "31"};
      success &= doc.updateRecord(0, modifiedRecord);
      success &= doc.save();

      // 验证修改结果
      auto record = doc.getRecord(0);
      if (record.first) {
        assert(record.second[0] == "Zhang San Jr");
        assert(record.second[1] == "31");
      } else {
        success = false;
      }
    } catch (const std::exception& e) {
      std::cout << "异常: " << e.what() << std::endl;
      return false;
    }

    std::cout << (success ? "通过" : "失败") << "\n";
    return success;
  }

  static bool TestDeleteRecord() {
    std::cout << "测试删除记录功能...\n";
    bool success = true;

    try {
      Dbf doc;
      success &= doc.open("test.dbf", false);
      if (!success) {
        std::cout << "打开DBF文件失败: " << doc.getLastError() << std::endl;
        return false;
      }

      size_t originalCount = doc.getRecordCount();

      // 删除第一条记录
      success &= doc.deleteRecord(0);
      success &= doc.save();

      // 验证记录数减少
      assert(doc.getRecordCount() == originalCount - 1);
    } catch (const std::exception& e) {
      std::cout << "异常: " << e.what() << std::endl;
      return false;
    }

    std::cout << (success ? "通过" : "失败") << "\n";
    return success;
  }
};

int main() {
  try {
    bool success = DbfTester::RunAllTests();
    std::cout << "\n按任意键继续...\n";
    std::getchar();
    return success ? 0 : 1;
  } catch (const std::exception& e) {
    std::cerr << "未捕获的异常: " << e.what() << std::endl;
    std::getchar();
    return 1;
  }
}