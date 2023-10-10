//
// Created by viviwu on 2023/5/9.
//
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../common/base/atgo_def.h"
#include "dbf_doc.h"

#include <iostream>


using namespace std;

int read_dbf() {
    cout << __func__ << endl;
    char date_str[9] = {0};              // 20230418
    time_t now = time(NULL);
    strftime(date_str, sizeof(date_str), "%Y%m%d", localtime(&now));

    char file_name[30] = {0};       // "OrderAlgo_20230510.dbf";
    snprintf(file_name, sizeof(file_name), "OrderAlgo_%s.dbf", date_str);
    DbfDoc dbf = DbfDoc(file_name);

    const char *ver = dbf.GetDbfStringVersion();
    const char *dt = dbf.GetDbfDate();
    printf("dbf version:%s, date:%s\n", ver, dt);
    int rec_len = dbf.DbfRecordLength();
    int head_size = dbf.DbfHeaderSize();
    int row_cnt = dbf.DbfRowsNum();
    int col_cnt = dbf.DbfColsNum();

    printf("head_size:%d record_length:%d\n", head_size, rec_len);
    printf("header size:%d \n", dbf.DbfHeaderSize());
    printf("dbf %d rows, %d cols \n", row_cnt, col_cnt);

    for (int i = 0; i < col_cnt; ++i) {
        const char *name = dbf.DbfColumnName(i);
        const char type = dbf.DbfColumnType(i);
        u_int32_t addr = dbf.DbfColumnAddress(i);
        int size = dbf.DbfColumnSize(i);
        int deci = dbf.DbfColumnDecimals(i);
        printf("Field-%d Name:%s Type:%c Address:%d Size:%d Decimals:%d \n",
               i, name, type, addr, size, deci);
    }

    char record[1024];
    for (int j = 1; j < 2/*row_cnt*/ ; j++) {
    record[rec_len] = 0;
    dbf.ReadDbfRecordAtRow(record, j);

    for (int i = 0; i < col_cnt; ++i) {
        const char *name = dbf.DbfColumnName(i);
        int filed_size = dbf.DbfColumnSize(i);
        char *field_buf = (char *) malloc(filed_size + 1);
        // memset(val, 0, sizeof(filed_size+1));
        field_buf[filed_size] = 0;
        dbf.GetDbfRecordValue(record, i, field_buf);
        char type = dbf.DbfColumnType(i);

        unsigned long long num =
            0; // num = strtod(field_buf, NULL);  // atol? strtol?
        if ('N' == type) {
            sscanf(field_buf, "%llu", &num);
            printf("%2d-%2d %s: %llu [%c]\n", j, i, name, num, type);
        } else {
            printf("%2d-%2d %s: %s [%c]\n", j, i, name, field_buf, type);
        }
        free(field_buf);
    }
    }
    return 0;
}

void write_dbf() {
    char stk_acct[] = "stk10001253";
    char symbol[] = "600030";
    char side[] = "1"; // 1:BUY; 2:sell
    char ord_qty[] = "100";
    char ord_type[] = "201";             //
    char algo_paragm[] = "priceF=20.50"; // :buyTypeI=1:sellTypeI=1
    char timestamp[15] = {0};            // 20230418 093000
    char date_str[9] = {0};              // 20230418
    char eff_time[18] = {0};             // 20230418 093000 000
    char exp_time[18] = {0};             // 20230418 145700 000

    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", localtime(&now));
    strftime(date_str, sizeof(date_str), "%Y%m%d", localtime(&now));
    snprintf(eff_time, sizeof(eff_time), "%s000", timestamp);
    snprintf(exp_time, sizeof(exp_time), "%s145700000", date_str);

    AORecord order;
    printf("sizeof(order)==%zu \n", sizeof(order)); // 632
    memset(&order, ' ', sizeof(order));
    strncpy(order.ExternalId, timestamp, strlen(timestamp));
    strncpy(order.ClientName, stk_acct, strlen(stk_acct));
    strncpy(order.Symbol, symbol, strlen(symbol));
    strncpy(order.EffTime, eff_time, strlen(eff_time));
    strncpy(order.ExpTime, exp_time, strlen(exp_time));
    strncpy(order.Side, side, strlen(side));
    strncpy(order.OrderQty, ord_qty, strlen(ord_qty));
    strncpy(order.OrdType, ord_type, strlen(ord_type));
    order.LimAction = '0';
    order.AftAction = '0';
    strncpy(order.AlgoParam, algo_paragm, strlen(algo_paragm));

//  printf("ClientName:%s\n", order.ClientName);
//  printf("Symbol:%s\n", order.Symbol);
//  printf("ExpTime:%s\n", order.ExpTime);
//  printf("OrderQty=%s\n", order.OrderQty);
//  printf("AlgoParam=%s\n", order.OrderQty);

    char file_name[30] = {0};
    snprintf(file_name, sizeof(file_name), "OrderAlgo_%s.dbf", date_str);
    DbfDoc dbf = DbfDoc(file_name, false);
    int rec_len = dbf.DbfRecordLength();
    int head_size = dbf.DbfHeaderSize();
    int row_cnt = dbf.DbfRowsNum();
    int col_cnt = dbf.DbfColsNum();

    printf("head_size:%d record_length:%d\n", head_size, rec_len);
    printf("header size:%d \n", dbf.DbfHeaderSize());
    printf("dbf %d rows, %d cols \n", row_cnt, col_cnt);

    char *record_buffer = (char *) malloc(sizeof(AORecord));
    memcpy(record_buffer, (char *) (&order), sizeof(record_buffer));
    // printf("record_buffer:%s \n", record_buffer);

    int records = dbf.WriteDbfRecord(record_buffer, sizeof(order));
    if (records < 0)
        printf("WriteDbfRecord failed! %s\n", record_buffer);
    printf("file_name:%s records=%d \n", file_name, records);

    free(record_buffer);

}

int main(int argc, char *argv[]) {

//    write_dbf();

    read_dbf();

    getchar();

    return 0;
}