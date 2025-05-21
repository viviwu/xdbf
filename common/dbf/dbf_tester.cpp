#include "dbf.h"
#include <iostream>
#include <cassert>
#include <iomanip>

class DbfTester {
public:
    static bool RunAllTests() {
        std::cout << "��ʼDBF�ļ���������...\n\n";
        
        bool success = true;
        success &= TestCreateAndSave();
        success &= TestOpenAndRead();
        success &= TestAddRecord();
        success &= TestModifyRecord();
        success &= TestDeleteRecord();
        
        std::cout << "\n���Խ��: " << (success ? "ȫ��ͨ��" : "����ʧ��") << "\n";
        return success;
    }

private:
    static bool TestCreateAndSave() {
        std::cout << "���Դ����ͱ��湦��...\n";
        bool success = true;
        
        try {
            Dbf doc;
            std::vector<DB_FIELD> fields;
            
            // �����ֶ�������
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
            
            // �����ļ�
            success &= doc.create("test.dbf", fields);
            if (!success) {
                std::cout << "����DBF�ļ�ʧ��: " << doc.getLastError() << std::endl;
                return false;
            }
            
            // ��Ӳ��Լ�¼
            std::vector<std::string> record1 = {"Zhang San", "30"};
            std::vector<std::string> record2 = {"Li Si", "25"};
            
            success &= doc.addRecord(record1);
            success &= doc.addRecord(record2);
            success &= doc.save();
            
            if (!success) {
                std::cout << "��Ӽ�¼ʧ��: " << doc.getLastError() << std::endl;
                return false;
            }
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "ͨ��" : "ʧ��") << "\n";
        return success;
    }
    
    static bool TestOpenAndRead() {
        std::cout << "���Դ򿪺Ͷ�ȡ����...\n";
        bool success = true;
        
        try {
            Dbf doc;
            success &= doc.open("test.dbf", true);
            if (!success) {
                std::cout << "��DBF�ļ�ʧ��: " << doc.getLastError() << std::endl;
                return false;
            }
            
            // ��֤��¼��
            assert(doc.getRecordCount() == 2);
            
            // ��֤�ֶ���
            assert(doc.getFieldCount() == 2);
            
            // ��ȡ����֤��һ����¼
            auto record = doc.getRecord(0);
            if (record.first) {
                assert(record.second[0] == "Zhang San");
                assert(record.second[1] == "30");
            } else {
                success = false;
            }
            
            // ��֤�ֶ�����
            assert(doc.getFieldName(0) == "NAME");
            assert(doc.getFieldName(1) == "AGE");
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "ͨ��" : "ʧ��") << "\n";
        return success;
    }
    
    static bool TestAddRecord() {
        std::cout << "������Ӽ�¼����...\n";
        bool success = true;
        
        try {
            Dbf doc;
            success &= doc.open("test.dbf", false);
            if (!success) {
                std::cout << "��DBF�ļ�ʧ��: " << doc.getLastError() << std::endl;
                return false;
            }
            
            size_t originalCount = doc.getRecordCount();
            
            // ����¼�¼
            std::vector<std::string> newRecord = {"Wang Wu", "28"};
            success &= doc.addRecord(newRecord);
            success &= doc.save();
            
            // ��֤��¼������
            assert(doc.getRecordCount() == originalCount + 1);
            
            // ��֤�¼�¼����
            auto record = doc.getRecord(doc.getRecordCount() - 1);
            if (record.first) {
                assert(record.second[0] == "Wang Wu");
                assert(record.second[1] == "28");
            } else {
                success = false;
            }
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "ͨ��" : "ʧ��") << "\n";
        return success;
    }
    
    static bool TestModifyRecord() {
        std::cout << "�����޸ļ�¼����...\n";
        bool success = true;
        
        try {
            Dbf doc;
            success &= doc.open("test.dbf", false);
            if (!success) {
                std::cout << "��DBF�ļ�ʧ��: " << doc.getLastError() << std::endl;
                return false;
            }
            
            // �޸ĵ�һ����¼
            std::vector<std::string> modifiedRecord = {"Zhang San Jr", "31"};
            success &= doc.updateRecord(0, modifiedRecord);
            success &= doc.save();
            
            // ��֤�޸Ľ��
            auto record = doc.getRecord(0);
            if (record.first) {
                assert(record.second[0] == "Zhang San Jr");
                assert(record.second[1] == "31");
            } else {
                success = false;
            }
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "ͨ��" : "ʧ��") << "\n";
        return success;
    }
    
    static bool TestDeleteRecord() {
        std::cout << "����ɾ����¼����...\n";
        bool success = true;
        
        try {
            Dbf doc;
            success &= doc.open("test.dbf", false);
            if (!success) {
                std::cout << "��DBF�ļ�ʧ��: " << doc.getLastError() << std::endl;
                return false;
            }
            
            size_t originalCount = doc.getRecordCount();
            
            // ɾ����һ����¼
            success &= doc.deleteRecord(0);
            success &= doc.save();
            
            // ��֤��¼������
            assert(doc.getRecordCount() == originalCount - 1);
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << (success ? "ͨ��" : "ʧ��") << "\n";
        return success;
    }
};

int main() {
    try {
        return DbfTester::RunAllTests() ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << "���Թ����з����쳣: " << e.what() << std::endl;
        return 1;
    }
}