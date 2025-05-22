#include "dbf.h"
#include <iostream>
#include <cassert>
#include <iomanip>

class DbfTester {
public:
  static bool RunAllTests() {
    std::cout << "��ʼDBF�ļ���������...\n\n";

    bool success = true;
    try {
      success &= TestCreateAndSave();
      success &= TestOpenAndRead();
      success &= TestAddRecord();
      success &= TestModifyRecord();
      // success &= TestDeleteRecord();
    } catch (const std::exception& e) {
      std::cout << "���Թ����з����쳣: " << e.what() << std::endl;
      return false;
    }

    std::cout << "\n���Խ��: " << (success ? "ȫ��ͨ��" : "����ʧ��") << "\n";
    return success;
  }

private:
  static void PrintError(const Dbf& doc, const std::string& operation) {
    std::cout << "����ʧ�� [" << operation << "]: " << doc.getLastError() <<
        std::endl;
  }

  static bool TestCreateAndSave() {
    std::cout << "���Դ����ͱ��湦��...\n";

    Dbf doc;
    std::vector<DB_FIELD> fields;

    // �����ֶ�������
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

    // �����ļ�
    if (!doc.create("test.dbf", fields)) {
      PrintError(doc, "create");
      return false;
    }

    // ��Ӳ��Լ�¼
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
    std::cout << "ͨ��\n";
    return true;
  }

  static bool TestOpenAndRead() {
    std::cout << "���Դ򿪺Ͷ�ȡ����...\n";

    Dbf doc;
    if (!doc.open("test.dbf", true)) {
      PrintError(doc, "open");
      return false;
    }

    // ��֤��¼��
    if (doc.getRecordCount() != 2) {
      std::cout << "��¼����ƥ��: ���� 2, ʵ�� " << doc.getRecordCount() << std::endl;
      return false;
    }

    // ��֤�ֶ���
    if (doc.getFieldCount() != 2) {
      std::cout << "�ֶ�����ƥ��: ���� 2, ʵ�� " << doc.getFieldCount() << std::endl;
      return false;
    }

    // ��ȡ����֤��һ����¼
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
      std::cout << "��¼���ݲ�ƥ��: " << record.second[0] << ", " << record.second[1]
          << std::endl;
      return false;
    }

    // ��֤�ֶ�����
    if (doc.getFieldName(0) != "NAME" || doc.getFieldName(1) != "AGE") {
      std::cout << "�ֶ����Ʋ�ƥ��: " << doc.getFieldName(0) << ", " << doc.
          getFieldName(1) << std::endl;
      return false;
    }

    doc.close();
    std::cout << "ͨ��\n";
    return true;
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
    } catch (const std::exception& e) {
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
    } catch (const std::exception& e) {
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
    } catch (const std::exception& e) {
      std::cout << "�쳣: " << e.what() << std::endl;
      return false;
    }

    std::cout << (success ? "ͨ��" : "ʧ��") << "\n";
    return success;
  }
};

int main() {
  try {
    bool success = DbfTester::RunAllTests();
    std::cout << "\n�����������...\n";
    std::getchar();
    return success ? 0 : 1;
  } catch (const std::exception& e) {
    std::cerr << "δ������쳣: " << e.what() << std::endl;
    std::getchar();
    return 1;
  }
}