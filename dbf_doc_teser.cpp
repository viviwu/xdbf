#include <cstring>
#include <iostream>
#include <vector>

#include "atgo/atgo_dbf_doc.h"
#include "atgo/atgo_dict.h"
#include "atgo/atgo_struct.h"
#include "tool.h"

void printOrderAlgo(const std::vector<std::string>& record) {
    if (record.empty()) {
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

int main() {
    // 获取今日日期 字符串
    std::string today = getTodayString();

    OrderAlgoDbfDoc doc;

    // File name with today's date (2025-04-01)
    std::string filename = "CancelOrderAlgo_20250424.dbf";

    // Open or create the DBF file
    if (!doc.open(filename)) {
        std::cout << "File not found, creating " << filename << std::endl;
        if (!doc.create(filename)) {
            std::cerr << "Failed to create DBF file" << std::endl;
            return 1;
        }
    }

    // Print existing records
    std::cout << "Existing records:" << std::endl;
    for (uint32_t i = 0; i < doc.getRecordCount(); ++i) {
        std::cout << "Record " << i << ": ";
        printOrderAlgo(doc.readRecord(i));
    }

    // Add a new order
    OrderAlgo newOrder = {};
    std::strncpy(newOrder.ExternalId, "ORD20250424001", 31);
    std::strncpy(newOrder.ClientName, "10026851", 256);
    std::strncpy(newOrder.Symbol, "000001", 41);
    newOrder.Side = SIDE_BUY;  // From atgo_dict.h
    newOrder.OrderQty = 1000;
    newOrder.OrdType = ORDTYPE_TWAP_PLUS;                     // From atgo_dict.h
    std::strncpy(newOrder.EffTime, "20250424093000000", 18);  // 9:30:00.000
    std::strncpy(newOrder.ExpTime, "20250424150000000", 18);  // 15:00:00.000
    newOrder.LimAction = 1;                                   // Yes, continue trading at limit
    newOrder.AftAction = 0;                                   // No, stop after expiration
    std::strncpy(newOrder.AlgoParam, "", 256);

    if (!doc.addOrderAlgo(newOrder)) {
        std::cerr << "Failed to add new order" << std::endl;
        return 1;
    }

    // Save the updated file
    if (!doc.save()) {
        std::cerr << "Failed to save DBF file" << std::endl;
        return 1;
    }

    // Reopen and verify
    if (!doc.open(filename)) {
        std::cerr << "Failed to reopen DBF file" << std::endl;
        return 1;
    }

    std::cout << "\nUpdated records:" << std::endl;
    for (uint32_t i = 0; i < doc.getRecordCount(); ++i) {
        std::cout << "Record " << i << ": ";
        printOrderAlgo(doc.readRecord(i));
    }

    std::cout << "File " << filename << " updated successfully" << std::endl;
    return 0;
}