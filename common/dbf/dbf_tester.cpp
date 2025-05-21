#include "dbf.h"
#include <iostream>
#include <cassert>
#include <iomanip>

class DbfTester {
public:
    static bool RunAllTests() {
        std::cout << "开始DBF文件操作测试...\n\n";
        
        bool success = true;
        success &= TestCreateAndSave();
        success &= TestOpenAndRead();
        success &= TestAddRecord();
        success &= TestModifyRecord();
        success &= TestDeleteRecord();
        
        std::cout << "\n测试结果: " << (success ? "全部通过" : "存在失败") << "\n";
        return success;
    }

private:
    static bool TestCreateAndSave() {
        std::cout << "测试创建和保存功能...\n";
        bool success = true;
        
        try {
            Dbf doc;
            std::vector<DB_FIELD> fields;
            
            // 创建字段描述符
            DB_FIELD nameField = {0};
            std::strcpy(reinterpret_cast<char*>(nameField.field_name), "NAME");
            nameField.field_type = 'C';
            nameField.field_length = 20;
            nameField.field_decimals = 0;
            
            DB_FIELD ageField = {0};
            std::strcpy(reinterpret_cast<char*>(ageField.field_name), "AGE");
            ageField.field_type = 'N';
            ageField.field_length = 3;
            ageField.field_decimals = 0;
            
            fields.push_back(nameField);
            fields.push_back(ageField);
            
            // 创建文件
            success &= doc.create("test.dbf", fields);
            if (!success) {
                std::cout << "创建DBF文件失败: " << doc.getLastError() << std::endl;
                return false;
            }
            
            // 添加测试记录
            std::vector<std::string> record1 = {"Zhang San", "30"};
            std::vector<std::string> record2 = {"Li Si", "25"};
            
            success &= doc.addRecord(record1);
            success &= doc.addRecord(record2);
            success &= doc.save();
            
            if (!success) {
                std::cout << "添加记录失败: " << doc.getLastError() << std::endl;
                return false;
            }
        }
        catch (const std::exception& e) {
            std::cout << "异常: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "通过" : "失败") << "\n";
        return success;
    }
    
    static bool TestOpenAndRead() {
        std::cout << "测试打开和读取功能...\n";
        bool success = true;
        
        try {
            Dbf doc;
            success &= doc.open("test.dbf", true);
            if (!success) {
                std::cout << "打开DBF文件失败: " << doc.getLastError() << std::endl;
                return false;
            }
            
            // 验证记录数
            assert(doc.getRecordCount() == 2);
            
            // 验证字段数
            assert(doc.getFieldCount() == 2);
            
            // 读取并验证第一条记录
            auto record = doc.getRecord(0);
            if (record.first) {
                assert(record.second[0] == "Zhang San");
                assert(record.second[1] == "30");
            } else {
                success = false;
            }
            
            // 验证字段名称
            assert(doc.getFieldName(0) == "NAME");
            assert(doc.getFieldName(1) == "AGE");
        }
        catch (const std::exception& e) {
            std::cout << "异常: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "通过" : "失败") << "\n";
        return success;
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
        }
        catch (const std::exception& e) {
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
        }
        catch (const std::exception& e) {
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
        }
        catch (const std::exception& e) {
            std::cout << "异常: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "通过" : "失败") << "\n";
        return success;
    }
};

int main() {
    try {
        return DbfTester::RunAllTests() ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
}