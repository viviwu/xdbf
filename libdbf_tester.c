//
// Created by wuxw on 2025/4/24/024.
//


#include <libdbf/libdbf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>  // 用于ftime函数
#include <time.h>
#include <atgo/atgo_struct.h>
// 获取今日日期字符串 YYYYMMDD
void get_today_date(char* buffer, size_t size)
{
  if (!buffer || size < 9)
    return;
  time_t     rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, size, "%Y%m%d", timeinfo);
}

// 生成OrderAlgo_YYYYMMDD.dbf格式的文件名
void generate_order_algo_filename(char* buffer, size_t size)
{
  if (!buffer || size < 24)
    return;
  char date[9];
  get_today_date(date, sizeof(date));
  sprintf(buffer, "OrderAlgo_%s.dbf", date);
}

// 生成YYYYMMDDsshhmmSSS格式的固定时间字符串
void generate_fixed_time_string(char* buffer, size_t size, const char* fixed_part)
{
  if (!buffer || size < 18)
    return;
  char date[9];
  get_today_date(date, sizeof(date));
  sprintf(buffer, "%s%s", date, fixed_part);
}

// 生成当前时间戳（精确到秒）YYYYMMDDHHMMSS
void generate_current_timestamp_second(char* buffer, size_t size)
{
  if (!buffer || size < 15)
    return;
  time_t     now;
  struct tm* tm_info;

  time(&now);
  tm_info = localtime(&now);

  sprintf(buffer, "%04d%02d%02d%02d%02d%02d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
          tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

// 生成当前时间戳（精确到毫秒）YYYYMMDDHHMMSSsss
void generate_current_timestamp_millisecond(char* buffer, size_t size)
{
  if (!buffer || size < 18)
    return;
  struct timeb tb;
  struct tm*   tm_info;
  time_t       now;

  time(&now);
  tm_info = localtime(&now);
  ftime(&tb);

  sprintf(buffer, "%04d%02d%02d%02d%02d%02d%03d", tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
          tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tb.millitm);

  // 计算毫秒（注意：这种方法依赖于CLOCKS_PER_SEC，可能不是很精确）
  //  unsigned int milliseconds;
  //  clock_now = clock();
  //  milliseconds = (clock_now % CLOCKS_PER_SEC) * 1000 / CLOCKS_PER_SEC;
}

int main()
{
  char filename[24] = {0};
  char eff_time[18] = {0};  //YYYYMMDDHHMMSSsss
  char exp_time[18] = {0};  //YYYYMMDDHHMMSSsss
  char timestamp_sec[15] = {0};
  char cur_timestamp[18] = {0};  //YYYYMMDDHHMMSSsss

  generate_order_algo_filename(filename, sizeof(filename));
  generate_fixed_time_string(eff_time, sizeof(eff_time), "093001000");
  generate_fixed_time_string(exp_time, sizeof(exp_time), "145659000");
  generate_current_timestamp_second(timestamp_sec, sizeof(timestamp_sec));
  generate_current_timestamp_millisecond(cur_timestamp, sizeof(cur_timestamp));

  printf("filename: %s\n", filename);
  printf("eff_time: %s\n", eff_time);
  printf("exp_time: %s\n", exp_time);
  printf("timestamp_sec: %s\n", timestamp_sec);
  printf("cur_timestamp: %s\n", cur_timestamp);

  P_DBF* p_dbf = dbf_Open(filename, false);
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

  char *record; // char record[636];
#if 0
  //source data:
  char* order_data[] = {cur_timestamp, "10001253", "605009.SH", "1", "100", "201", eff_time, exp_time, "1", "1", ""};
//  printf("order_data:%s\n", order_data[0]);

  // receive buffer:
  record = (char*) malloc(rec_len);
  if (record == NULL)
  {
    dbf_Close(p_dbf);
    return -1;
  }

  memset(record, 0x2A, rec_len); // *
  memset(record, 0x20, 1); // space
  // fill data to buffer
  for (int column = 0; column < num_cols; ++column)
  {
    char* str = order_data[column];
    int   len = dbf_ColumnSize(p_dbf, column);
    if (len > 0)
    {
      int str_len = strlen(str);  // 此处需要去掉c字符串的\0结尾符
      if (str_len > len)
        str_len = len;
      memcpy(record + dbf_ColumnAddress(p_dbf, column), str, str_len);
      printf("ColumnName=%s, Field(%d,%d), Value=%s, str_len=%d\n", dbf_ColumnName(p_dbf, column),
             dbf_ColumnAddress(p_dbf, column), dbf_ColumnSize(p_dbf, column), str, str_len);
    }
  }
#endif

  OrderAlgo *order= malloc(sizeof(OrderAlgo));
  memset(order, 0x20, sizeof(OrderAlgo));
  strncpy(order->ExternalId, cur_timestamp, sizeof(cur_timestamp));
  strncpy(order->ClientName, "10001253", 8);
  strncpy(order->Symbol, "605009.SH", 9);
  strncpy(order->Side, "1", 1);
  strncpy(order->OrderQty, "100", 3);
  strncpy(order->OrdType, "201", 3);
  strncpy(order->EffTime, eff_time,  sizeof(order->EffTime));
  strncpy(order->ExpTime, exp_time, sizeof(order->ExpTime));
  order->LimAction='1';
  order->AftAction='1';
  strncpy(order->AlgoParam, "", 0);

  record = order;
  printf("order record:%s \n", record);

//  int ret = dbf_WriteRecordWithFlag(p_dbf, record, rec_len);
    int ret =dbf_WriteRecord(p_dbf, record, sizeof(OrderAlgo));
  if (0 == ret)
  {
    printf("dbf_WriteRecord succeed with no memo!");
  }
  else if (1 == ret)
  {
    printf("dbf_WriteRecord succeed with memo!");
  }
  else
    printf("dbf_WriteRecord on error!!");

  dbf_Close(p_dbf);

  if (record != NULL)
    free(record);

  return 0;
}
