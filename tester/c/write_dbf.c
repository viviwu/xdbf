/**
 * @program: c_dbf
 * @description: [...]
 * @author: viviwu [viviwu]
 * @create: 2023-04-14 13:07
 **/
// // created by viviwu on 2023/4/14.
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "atgo_def.h"
#include <libdbf/libdbf.h>

int main(int argc, char *argv[]) {
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
  printf("sizeof(order)==%zd \n", sizeof(order)); // 632
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

  char file_name[30] = {0};
  snprintf(file_name, sizeof(file_name), "OrderAlgo_%s.dbf", date_str);
  P_DBF *p_dbf = dbf_Open(file_name, false);
  if (p_dbf == NULL) {
    return -1;
  }

  char *record = (char *)malloc(sizeof(AORecord));
  memcpy(record, &order, sizeof(AORecord));
  int records = dbf_WriteRecord(p_dbf, record, sizeof(order));
  if (records == -1) {
    printf("dbf_WriteRecord failed!!\n");
    return -1;
  }
  free(record);

  int rec_len = dbf_RecordLength(p_dbf);
  int head_size = dbf_HeaderSize(p_dbf);
  int row_cnt = dbf_NumRows(p_dbf);
  int col_cnt = dbf_NumCols(p_dbf);

  printf("head_size:%d record_length:%d\n", head_size, rec_len);
  printf("header size:%d \n", dbf_HeaderSize(p_dbf));
  printf("dbf %d rows, %d cols \n", row_cnt, col_cnt);

  dbf_Close(p_dbf);

    system("read_dbf.exe");
//    execlp("ls", "ls", "-l", NULL); // 在 Linux/Unix 系统中调用 ls 命令
    getchar();
  return 0;
}
