#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>

#include "../common_util/tool.h"

#include "atgo_def.h"
#include "dbf_reader.h"
#include "dbf_writer.h"

void TestDbfRead(){
    char path[200] = "ReportOrderAlgo_20220614.dbf";
//    char path[200] = "asset.dbf NQHQ.DBF";
    DbfReader reader(path, "|");
    int record_length = reader.RecordLength();
    printf("record_length=%d %d \n", record_length, sizeof(AORecord));
#if 1
    RORecord record = {0} ;
    reader.NextRecord(&record);
    printf("ClientName: %s\n", record.ClientName);
    printf("Symbol: %s\n", record.Symbol);
    printf("OrderQty: %s\n", record.OrderQty);
//    while (reader.NextRecord(&record) > 0) {
//        printf("record: %s\n", record.Symbol);
//        memset(record, 0, record_length);
//    }
#else
    char *record;
    record_length += reader.ColumnsCount();
    record = (char *) malloc(record_length);
    memset(record, 0, record_length);
//    reader.NextLine(record);
    while (reader.NextLine(record)>0){
        printf("record: %s\n", record);
        memset(record, 0, record_length);
    }
    free(record);
#endif

}

void TestDbfWrite(){
    char fp[]="new1.dbf";
    DbfWriter writer(fp, 3);
    writer.SetHeaderField('C', "name", 16, 0);
    writer.SetHeaderField('N', "code", 4, 0);
    writer.SetHeaderField('F', "price", 8, 2);
    writer.WriteHeaderInfo();
    writer.WriteFieldInfo();

    int record_len = writer.RecordLength();
    printf("record_len: %d \n", record_len);
    char *record = (char *)malloc(record_len);
    memset(record, '0', record_len);
    for (int j = 0; j < 5; ++j) {
        stroffset(record, "apple", 0);
        stroffset(record, "8888", 16);
        stroffset(record, "12.345", 20);
        writer.WriteRecord(record, record_len);
    }
    free(record);
}

int main(int argc, char *argv[]) {
    std::cout << "Test of DBFEngine Started" << std::endl;

    TestDbfRead();
//    TestDbfWrite();
//    _sleep(1000);


    return 0;
}


//  std::stringstream ss;
//  ss << "12.345";
//  float num;
//  ss >> num;
//  std::cout << num << std::endl;

//    getchar();