//
// Created by viviwu on 2022/5/14.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <locale.h>

#include <libdbf/libdbf.h>

int main(int argc, char *argv[]) {
  //    setlocale(LC_ALL, "utf-8");
  printf("read dbf! \n");

  char date_str[9] = {0};       // 20230418
  time_t now = time(NULL);
  strftime(date_str, sizeof(date_str), "%Y%m%d", localtime(&now));
  char file_name[30] = {0};     // "ReportBalance_20230419.dbf";
  snprintf(file_name, sizeof(file_name), "OrderAlgo_%s.dbf", date_str);

  if (argc > 1)
    strcpy(file_name, argv[1]);

  P_DBF *p_dbf = dbf_Open(file_name, true);
  if (NULL == p_dbf)
    return -1;

  const char *ver = dbf_GetStringVersion(p_dbf);
  const char *dt = dbf_GetDate(p_dbf);
  printf("dbf version:%s, date:%s\n", ver, dt);
  int rec_len = dbf_RecordLength(p_dbf);
  int head_size = dbf_HeaderSize(p_dbf);
  int row_cnt = dbf_NumRows(p_dbf);
  int col_cnt = dbf_NumCols(p_dbf);

  printf("head_size:%d record_length:%d\n", head_size, rec_len);
  printf("header size:%d \n", dbf_HeaderSize(p_dbf));
  printf("dbf %d rows, %d cols \n", row_cnt, col_cnt);

  if(col_cnt>1000){
      printf("read dbf data error!!\n");
      return -1;
  }
  for (int i = 0; i < col_cnt; ++i) {
    const char *name = dbf_ColumnName(p_dbf, i);
    const char type = dbf_ColumnType(p_dbf, i);
    u_int32_t addr = dbf_ColumnAddress(p_dbf, i);
    int size = dbf_ColumnSize(p_dbf, i);
    int deci = dbf_ColumnDecimals(p_dbf, i);
    printf("Column Field Name:%s Type:%c Address:%d Size:%d Decimals:%d \n",
           name, type, addr, size, deci);
  }

  char record[1024];
  for (int j = 0; j <  row_cnt ; j++) {
    record[rec_len] = 0;
    dbf_ReadRecord(p_dbf, record, 1024);

    for (int i = 0; i < col_cnt; ++i) {
      const char *name = dbf_ColumnName(p_dbf, i);
      int filed_size = dbf_ColumnSize(p_dbf, i);
      char *field_buf = malloc(filed_size + 1);
      // memset(val, 0, sizeof(filed_size+1));
      field_buf[filed_size] = 0;
      dbf_GetRecordValue(p_dbf, record, i, field_buf);
      char type = dbf_ColumnType(p_dbf, i);

      unsigned long long num = 0;
      if ('N' == type) {
        num = strtoull(field_buf, NULL, 10);
        // sscanf(field_buf, "%llu", &num);
        printf("%2d-%2d %s: %llu [%c]\n", j, i, name, num, type);
      } else {
        printf("%2d-%2d %s: %s [%c]\n", j, i, name, field_buf, type);
      }
      free(field_buf);
    }
  }

  dbf_Close(p_dbf);

  getchar();
  return 0;
}