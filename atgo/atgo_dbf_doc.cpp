//
// Created by wuxw on 2025/4/2/002.
//

#include "atgo_dbf_doc.h"
#include <cstring>
#include <iostream>

OrderAlgoDbfDoc::OrderAlgoDbfDoc() {}

bool OrderAlgoDbfDoc::addOrderAlgo(const OrderAlgo& order) {
    std::vector<std::string> data;
    // 使用固定长度构造字符串，避免 \0
    data.emplace_back(std::string(order.ExternalId, 30));
    data.emplace_back(std::string(order.ClientName, 255));
    data.emplace_back(std::string(order.Symbol, 40));
    data.emplace_back(std::to_string(order.Side));
    data.emplace_back(std::to_string(order.OrderQty));
    data.emplace_back(std::to_string(order.OrdType));
    data.emplace_back(std::string(order.EffTime, 17));
    data.emplace_back(std::string(order.ExpTime, 17));
    data.emplace_back(std::to_string(order.LimAction));
    data.emplace_back(std::to_string(order.AftAction));
    data.emplace_back(std::string(order.AlgoParam, 255));

    // 截断超长部分并用空格填充
    for (auto& field : data) {
        size_t max_len = (field == data[0]) ? 30 :
                         (field == data[1]) ? 255 :
                         (field == data[2]) ? 40 :
                         (field == data[6] || field == data[7]) ? 17 :
                         (field == data[10]) ? 255 : field.length();
        if (field.length() > max_len) field = field.substr(0, max_len);
        field.resize(max_len, ' ');
    }

    return addRecord(data);
}

bool OrderAlgoDbfDoc::create(const std::string& filename) {
    std::vector<DB_FIELD> fields = getOrderAlgoFields();
    return DbfDoc::create(filename, fields);
}

std::vector<DB_FIELD> OrderAlgoDbfDoc::getOrderAlgoFields() const {
    std::vector<DB_FIELD> fields(11);

    std::memset(&fields[0], 0, sizeof(DB_FIELD));
    std::strncpy(fields[0].field_name, "EXTERNALID", 10);
    fields[0].field_type = DBF_FIELD_CHAR;
    fields[0].field_length = 30;

    std::memset(&fields[1], 0, sizeof(DB_FIELD));
    std::strncpy(fields[1].field_name, "CLIENTNAME", 10);
    fields[1].field_type = DBF_FIELD_CHAR;
    fields[1].field_length = 255;

    std::memset(&fields[2], 0, sizeof(DB_FIELD));
    std::strncpy(fields[2].field_name, "SYMBOL", 10);
    fields[2].field_type = DBF_FIELD_CHAR;
    fields[2].field_length = 40;

    std::memset(&fields[3], 0, sizeof(DB_FIELD));
    std::strncpy(fields[3].field_name, "SIDE", 10);
    fields[3].field_type = DBF_FIELD_NUMERIC;
    fields[3].field_length = 4;

    std::memset(&fields[4], 0, sizeof(DB_FIELD));
    std::strncpy(fields[4].field_name, "ORDERQTY", 10);
    fields[4].field_type = DBF_FIELD_NUMERIC;
    fields[4].field_length = 4;

    std::memset(&fields[5], 0, sizeof(DB_FIELD));
    std::strncpy(fields[5].field_name, "ORDTYPE", 10);
    fields[5].field_type = DBF_FIELD_NUMERIC;
    fields[5].field_length = 4;

    std::memset(&fields[6], 0, sizeof(DB_FIELD));
    std::strncpy(fields[6].field_name, "EFFTIME", 10);
    fields[6].field_type = DBF_FIELD_CHAR;
    fields[6].field_length = 17;

    std::memset(&fields[7], 0, sizeof(DB_FIELD));
    std::strncpy(fields[7].field_name, "EXPTIME", 10);
    fields[7].field_type = DBF_FIELD_CHAR;
    fields[7].field_length = 17;

    std::memset(&fields[8], 0, sizeof(DB_FIELD));
    std::strncpy(fields[8].field_name, "LIMACTION", 10);
    fields[8].field_type = DBF_FIELD_NUMERIC;
    fields[8].field_length = 1;

    std::memset(&fields[9], 0, sizeof(DB_FIELD));
    std::strncpy(fields[9].field_name, "AFTACTION", 10);
    fields[9].field_type = DBF_FIELD_NUMERIC;
    fields[9].field_length = 1;

    std::memset(&fields[10], 0, sizeof(DB_FIELD));
    std::strncpy(fields[10].field_name, "ALGOPARAM", 10);
    fields[10].field_type = DBF_FIELD_CHAR;
    fields[10].field_length = 255;

    return fields;
}