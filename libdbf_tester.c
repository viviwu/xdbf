//
// Created by wuxw on 2025/4/24/024.
//

#include <stdio.h>
#include <assert.h>
#include <atgo/atgo_struct.h>
#include <libdbf/libdbf.h>

#include "common/utils/c_utils.h"

int main()
{
  char account_no[] = "10001253";
  char symbol[] = "300676.SZ";
  char side[] = "1";
  char order_qty[] = "200";
  char ord_type[] = "201";
  char algo_param[] = "PriceTypeI=0:priceF=55.95";
  char filename[24] = {0};
  char eff_time[18] = {0};  //YYYYMMDDHHMMSSsss
  char exp_time[18] = {0};  //YYYYMMDDHHMMSSsss
  char timestamp_sec[15] = {0};
  char cur_timestamp[18] = {0};  //YYYYMMDDHHMMSSsss

  today_order_algo_filename(filename, sizeof(filename));
  today_fixed_time_string(eff_time, sizeof(eff_time), "093001000");
  today_fixed_time_string(exp_time, sizeof(exp_time), "145659000");
  formatted_current_timestamp_second(timestamp_sec, sizeof(timestamp_sec));
  formatted_current_timestamp_millisecond(cur_timestamp, sizeof(cur_timestamp));

  printf("filename: %s\n", filename);
  printf("eff_time: %s\n", eff_time);
  printf("exp_time: %s\n", exp_time);
  printf("timestamp_sec: %s\n", timestamp_sec);
  printf("cur_timestamp: %s\n", cur_timestamp);

  char dbf_file_path[256];
  sprintf(dbf_file_path, "D:\\Program Files\\SJZQ-FZHJ\\OrderScan\\%s", filename);
  printf("dbf_file_path:%s \n", dbf_file_path);

  P_DBF* p_dbf = dbf_Open(dbf_file_path, false);
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
      char*       val = (char*) malloc(field_size);
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

  // write record:
  if (sizeof(OrderAlgo) != rec_len - 1)
  {
    fprintf(stderr, "sizeof(OrderAlgo) %zd != rec_len(%d)\n", sizeof(OrderAlgo), rec_len);
    return -1;
  }
  char* record;  // char[636];
  if (NULL == (record = (char*) malloc(rec_len)))
  {
    fprintf(stderr, "OrderAlgo* order malloc failed!\n");
    dbf_Close(p_dbf);
    return -1;
  }
  memset(record, 0x20, rec_len);  //0x20 0x2A

#if 0
  //source data:
  char* order_data[] = {cur_timestamp, account_no, symbol, side, order_qty, ord_type,
                        eff_time,      exp_time,   "1",    "1",  algo_param};

  // fill data to buffer
  for (int col = 0; col < num_cols; ++col)
  {
    char* str = order_data[col];
    int   len = dbf_ColumnSize(p_dbf, col);
    if (len > 0)
    {
      int str_len = strlen(str);  // cstring need trim '\0'
      if (str_len > len)
        str_len = len;
      memcpy(record + dbf_ColumnAddress(p_dbf, col), str, str_len);
      printf("ColumnName=%s, Field(%d,%d), Value=%s, str_len=%d\n", dbf_ColumnName(p_dbf, col),
             dbf_ColumnAddress(p_dbf, col), dbf_ColumnSize(p_dbf, col), str, str_len);
    }
  }
  printf("%s \n", record);
  int ret = dbf_WriteRecordWithFlag(p_dbf, record, rec_len);
  if (ret > 0)
  {
    printf("dbf_WriteRecord succeed!");
  }
  else
  {
    printf("dbf_WriteRecord on error!!");
  }
#else
  OrderAlgo *order = malloc(sizeof(OrderAlgo));
  memset(order, 0x20, sizeof(OrderAlgo));
  strncpy(order->ExternalId, cur_timestamp, strlen(cur_timestamp));
  strncpy(order->ClientName, account_no, strlen(account_no));
  strncpy(order->Symbol, symbol, strlen(symbol));
  strncpy(order->Side, side, strlen(side));
  strncpy(order->OrderQty, order_qty, strlen(order_qty));
  strncpy(order->OrdType, ord_type, strlen(ord_type));
  strncpy(order->EffTime, eff_time, strlen(eff_time));
  strncpy(order->ExpTime, exp_time, strlen(exp_time));
  order->LimAction = '1';
  order->AftAction = '1';
  strncpy(order->AlgoParam, algo_param, strlen(algo_param));

//  int ret = dbf_WriteRecord(p_dbf, order, sizeof(OrderAlgo));
  memcpy(record+1, order, sizeof(OrderAlgo));
  int ret = dbf_WriteRecordWithFlag(p_dbf, record, rec_len);
  if (ret > 0)
  {
    printf("dbf_WriteRecord succeed!");
  }
  else
  {
    printf("dbf_WriteRecord on error!!");
  }

#endif

  dbf_Close(p_dbf);

  if (record != NULL)
    free(record);

  return 0;
}
