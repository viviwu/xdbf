#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "atgo/atgo_dict.h"
#include "atgo/atgo_struct.h"
#include "dbf_doc.h"
#include "tool.h"
#include "utils.h"

void printOrderAlgo(const std::vector<std::string>& record)
{
  if (record.empty())
  {
    std::cout << "Empty or deleted record" << std::endl;
    return;
  }
  std::cout << "ExternalId: " << record[0] << " | "
            << "ClientName: " << record[1] << " | "
            << "Symbol: " << record[2] << " | "
            << "Side: " << record[3] << " | "
            << "OrderQty: " << record[4] << " | "
            << "OrdType: " << record[5] << " | "
            << "EffTime: " << record[6] << " | "
            << "ExpTime: " << record[7] << " | "
            << "LimAction: " << record[8] << " | "
            << "AftAction: " << record[9] << " | "
            << "AlgoParam: " << record[10] << std::endl;
}

int main()
{

  char account_no[] = "10001253";
  char symbol[] = "605009.SH";
  char side[] = "1";
  char order_qty[] = "100";
  char ord_type[] = "201";
  char algo_param[] = "PriceTypeI=0:priceF=58.34";
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

  char dbf_file_path[256];
  sprintf(dbf_file_path, "D:\\Program Files\\SJZQ-FZHJ\\OrderScan\\%s", filename);
  printf("dbf_file_path:%s \n", dbf_file_path);

  DbfDoc doc;
  // Open or create the DBF file
  if (!doc.open(dbf_file_path))
  {
    std::cout << "File not found" << filename << std::endl;
  }

  // Print existing records
  std::cout << "Existing records:" << std::endl;
  for (uint32_t i = 0; i < doc.getRecordCount(); ++i)
  {
    std::cout << "Record " << i << ": ";
    printOrderAlgo(doc.readRecord(i));
  }

  // Add a new order
  std::vector<std::string> order_data = {cur_timestamp, account_no, symbol, side, order_qty, ord_type,
                                         eff_time,      exp_time,   "1",    "1",  algo_param};

  if (!doc.addRecord(order_data))
  {
    std::cerr << "Failed to add new order" << std::endl;
    return 1;
  }

  // Save the updated file
  if (!doc.save())
  {
    std::cerr << "Failed to save DBF file" << std::endl;
    return 1;
  }

  // Reopen and verify
  if (!doc.open(filename))
  {
    std::cerr << "Failed to reopen DBF file" << std::endl;
    return 1;
  }

  std::cout << "\nUpdated records:" << std::endl;
  for (uint32_t i = 0; i < doc.getRecordCount(); ++i)
  {
    std::cout << "Record " << i << ": ";
    printOrderAlgo(doc.readRecord(i));
  }

  std::cout << "File " << filename << " updated successfully" << std::endl;
  return 0;
}