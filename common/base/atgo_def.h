/**
* @program: ATGO
* @description: [...]
* @author: viviwu [Administrator]
* @create: 2022-06-15 09:44
**/
#include <string.h>
#include <stdlib.h>

#ifndef LIBDBF_ATGO_DEF_H
#define LIBDBF_ATGO_DEF_H


typedef unsigned char   uint8_t;

#pragma pack(push, 1)

//母单委托文件
typedef struct AlgoOrder{
    char   ExternalId[30];     // N, 自定义委托编号;
    char   ClientName[255];    // Y, 账户名称;
    char   Symbol[40];         // Y, 证券代码，例如：000001;
    char Side[3];           // Y, 买卖方向；见附录，字段说明;
    char OrderQty[10];      // Y, 委托数量;
    char OrdType[3];        // Y, 算法类型；见附录，字段说明;
    char   EffTime[17];        // Y, 开始时间(20201106 102101 000); Number
    char   ExpTime[17];        // Y, 结束时间(20201106102101000); Number
    uint8_t     LimAction;       // Y, 0-涨跌停后不交易, 1-涨跌停后仍交易;
    uint8_t     AftAction;       // Y, 0-时间过期后不交易, 1-时间过期后仍交易;
    char   AlgoParam[255];     // N, 策略参数；见附录，字段说明
}AORecord;

// 撤单委托文件
typedef struct CancelOrderAlgo {
    char QuoteId[20];      // Y 母单委托编号
    uint8_t CxlType[3];    // Y 撤单类型(1-撤单，2-禁开)
} CORecord;


// 母单委托回报文件
typedef struct ReportOrderAlgo{
    char ExternalId[30];        // 自定义委托编号;
    char QuoteId[20];        // 母单委托编号;
    char ClientName[255];        // 账户名称;
    char Symbol[40];        // 证券代码;
    uint8_t SecType[3];        // 证券类型；见附录，字段说明;
    uint8_t SecExch[3];        // 交易所类型；见附录，字段说明;
    uint8_t Side[3];        // 买卖方向；见附录，字段说明;
    char TransTime[17];        // 委托时间(20201217091705000);
    uint8_t OrderQty[10];        // 委托数量;
    uint8_t OrdType[3];        // 算法类型；见附录，字段说明;
    uint8_t Price[10];        //  .3f 价格;
    char EffTime[17];        // 开始时间(20201217091705000);
    char ExpTime[17];        // 结束时间(20201217091705000);
    uint8_t LimAction[1];        // 0-涨跌停后不交易];        // 1-涨跌停后仍交易;
    uint8_t AftAction[1];        // 0-时间过期后不交易];        // 1-时间过期后仍交易;
    char AlgoParam[255];        // 策略参数;
    uint8_t CumQty[10];              // 累计成交数量;
    uint8_t LeavesQty[10];           // 剩余数量;
    uint8_t OutstaQty[10];           // 未成交数量
    uint8_t AvgPx[10];               // 3f 平均成交价格;
    uint8_t OrdStatus[3];            // 母单状态；见附录，字段说明;
    uint8_t CxlType[3];              // 撤单类型;
    char BasketId[255];        // 篮子编号;
    char Text[255];            // 备注信息;
    char UpdateTime[17];       // 更新时间(20201217091705000)
}RORecord;

// 子单委托回报文件
typedef struct SubOrderAlgo{
    char ExternalId[30];    // 自定义委托编号
    char QuoteId[20];    // 母单委托编号
    char ClOrdId[20];    // 子单委托编号
    char OrderId[20];    // 外部委托编号
    char ClientName[255];    // 账户名称
    char TransTime[17];    // 委托时间(20201217091705000)
    uint8_t OrdStatus[3];    // 委托状态
    char Symbol[40];    // 证券代码
    uint8_t Side[3];    // 买卖方向；见附录，字段说明
    uint8_t SecType[3];    // 证券类型；见附录，字段说明
    uint8_t SecExch[3];    // 交易所类型；见附录，字段说明
    uint8_t Price[10];    // .3f 委托价格
    uint8_t OrdType[3];    // 算法类型；见附录，字段说明
    uint8_t OrderQty[10];    // 委托股数
    uint8_t CumQty[10];    // 累计成交数量
    uint8_t LeavesQty[10];    // 仍可撮合量
    uint8_t AvgPx[10];    // .3f 成交均价
    uint8_t UpdateTime[17];    // 更新时间(20201217091705000)
    char Text[255];    // 备注
}SORecord;

//资金回报文件
typedef struct ReportBalance{
    char ClientName[255];     // 账户名称
    uint8_t EnBalance[23];     // .3f 可用资金
    uint8_t CrBalance[23];     // .3f 融资可用资金
    uint8_t AssetAmt[23];        // .3f 总资产
    uint8_t MartketAmt[23];      // .3f 总市值
    char UpdateTime[17];     // 更新时间(20201217091705000)
}RBRecord;

//证券持仓回报文件
typedef struct ReportPosition{
    char ClientName[255];        // 账户名称
    uint8_t Exchange[3];        // 证券市场
    char Symbol[40];        // 证券代码
    uint8_t CurrentQ[10];        // 当前数量
    uint8_t EnableQty[10];        // 可用数量
    uint8_t ShortQty[10];        // 融券可用
    char UpdateTime[17];        // 更新时间(20201217091705000)
}RPRecord;

#pragma pack(pop)

#endif //LIBDBF_ATGO_DEF_H
