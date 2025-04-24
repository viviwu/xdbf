//
// Created by wuxw on 2025/4/2/002.
//

#ifndef ORDER_ALGO_DBF_DOC_H
#define ORDER_ALGO_DBF_DOC_H

#include "../common/dbf/dbf_doc.h"
#include "atgo_struct.h"
#include "atgo_dict.h"

class OrderAlgoDbfDoc : public DbfDoc {
public:
    OrderAlgoDbfDoc();
//    ~OrderAlgoDbfDoc() override = default;

    bool addOrderAlgo(const OrderAlgo& order);
    bool create(const std::string& filename);

private:
    std::vector<DB_FIELD> getOrderAlgoFields() const;
};

#endif // ORDER_ALGO_DBF_DOC_H
