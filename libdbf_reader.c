//
// Created by wuxw on 2025/5/15/015.
//

#include <stdio.h>
#include <assert.h>
#include <atx/atx_struct.h>
#include <libdbf/libdbf.h>
#include "common/utils/c_utils.h"
#include "common/3rd/libdbf/src/dbf.h"

int main()
{
  printf("sizeof(DB_HEADER)=%zd \n", sizeof(DB_HEADER));
  // printf("sizeof(DB_FIELD)=%d \n", sizeof(DB_FIELD));

  char filename[24] = {0};
  today_order_algo_filename(filename, sizeof(filename));
  printf("filename: %s\n", filename);

  char dbf_file_path[256];
  sprintf(dbf_file_path, "D:\\Program Files\\SJZQ-FZHJ\\OrderScan\\%s", filename);
  printf("dbf_file_path:%s \n", dbf_file_path);

  P_DBF* p_dbf = dbf_Open(filename, true);
  int    rec_len = dbf_RecordLength(p_dbf);
  int    num_cols = dbf_NumCols(p_dbf);
  int    num_rows = dbf_NumRows(p_dbf);

  printf("version of dbf:%s\n", dbf_GetStringVersion(p_dbf));
  printf("formatted date:%s\n", dbf_GetDate(p_dbf));
  printf("Number of columns:%d\n", num_cols);
  printf("Number of rows:%d\n", num_rows);
  printf("header length:%d\n", dbf_HeaderSize(p_dbf));
  printf("record length:%d\n", rec_len);

  for (int column = 0; column < num_cols; ++column)
  {
    printf("column:%d name: %s; type:%c; address:%d; length:%d; decimals:%d \n", column, dbf_ColumnName(p_dbf, column),
           dbf_ColumnType(p_dbf, column), dbf_ColumnAddress(p_dbf, column), dbf_ColumnSize(p_dbf, column),
           dbf_ColumnDecimals(p_dbf, column));
  }

  char buffer[636];
  for (int row = 0; row < num_rows; ++row)
  {
    memset(buffer, 0x20, 636);
    dbf_ReadRecord(p_dbf, buffer, 1024);
    for (int column = 0; column < num_cols; ++column)
    {
      const char* name = dbf_ColumnName(p_dbf, column);
      char        col_type = dbf_ColumnType(p_dbf, column);
      int         field_size = dbf_ColumnSize(p_dbf, column);
      char*       val = malloc(field_size);
      memset(val, 0x20, field_size);
      dbf_GetRecordValue(p_dbf, buffer, column, val);
      if ('N' == col_type)
      {
        unsigned long long num = strtoull(val, NULL, 10);
        printf("(%d,%d) %s: <%llu> [%c]\n", row, column, name, num, col_type);
      }
      else
      {
        printf("(%d,%d) %s: <%s> [%c]\n", row, column, name, val, col_type);
      }
    }
  }

  dbf_Close(p_dbf);

  return 0;
}
