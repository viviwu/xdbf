/**
 * @program: ATGO
 * @description: [...]
 * @author: wuxw
 * @create: 2022-06-15 09:44
 **/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifndef LIBDBF_ATGO_DEF_H
#define LIBDBF_ATGO_DEF_H

typedef unsigned char uint8_t;

#pragma pack(push, 1) /* 确保1字节对齐 */

/* 母单委托文件结构 */
typedef struct
{
  char   ExternalId[30];  /* 自定义委托编号 (N) */
  char   ClientName[255]; /* 账户名称 (Y) */
  char   Symbol[40];      /* 证券代码，如000001 (Y) */
  char   Side[3];         /* 买卖方向 (见Side枚举) (Y) */
  char   OrderQty[10];    /* 委托数量 (Y) */
  char   OrdType[6];      /* 算法类型 (见OrdType枚举) (Y) */
  char   EffTime[17];     /* 开始时间 (YYYYMMDDHHMMSSmmm) (Y) */
  char   ExpTime[17];     /* 结束时间 (格式同上) (Y) */
  int8_t LimAction;       /* 涨跌停后是否交易 0-否 1-是 (Y) */
  int8_t AftAction;       /* 过期后是否交易 0-否 1-是 (Y) */
  char   AlgoParam[255];  /* 策略参数 (N) */
} OrderAlgo;

/* 撤单委托文件结构 */
typedef struct
{
  char    QuoteId[20]; /* 母单委托编号 (Y) */
  int32_t CxlType;     /* 撤单类型 (见CxlType枚举) (Y) */
} CancelOrderAlgo;

/* 母单委托回报结构 */
typedef struct
{
  char    ExternalId[30];  /* 自定义委托编号 */
  char    QuoteId[20];     /* 系统委托编号 */
  char    ClientName[255]; /* 账户名称 */
  char    Symbol[40];      /* 证券代码 */
  int32_t SecType;         /* 证券类型 (见SecurityType枚举) */
  int32_t SecExch;         /* 交易所类型 (见ExchangeType枚举) */
  int32_t Side;            /* 买卖方向 */
  char    TransTime[17];   /* 委托时间 */
  int32_t OrderQty;        /* 委托数量 */
  int32_t OrdType;         /* 算法类型 */
  double  Price;           /* 价格 (精度3) */
  char    EffTime[17];     /* 开始时间 */
  char    ExpTime[17];     /* 结束时间 */
  int8_t  LimAction;       /* 涨跌停处理 */
  int8_t  AftAction;       /* 过期处理 */
  char    AlgoParam[255];  /* 策略参数 */
  int32_t CumQty;          /* 累计成交数量 */
  int32_t LeavesQty;       /* 剩余数量 */
  int32_t OutstaQty;       /* 未成交数量 */
  double  AvgPx;           /* 平均成交价 (精度3) */
  int32_t OrdStatus;       /* 订单状态 */
  int32_t CxlType;         /* 撤单类型 */
  char    BasketId[255];   /* 篮子编号 */
  char    Text[255];       /* 备注信息 */
  char    UpdateTime[17];  /* 更新时间 */
} ReportOrderAlgo;

/* 子单委托回报结构 */
typedef struct
{
  char    ExternalId[30];  /* 自定义委托编号 */
  char    QuoteId[20];     /* 母单编号 */
  char    ClOrdId[20];     /* 子单编号 */
  char    OrderId[20];     /* 外部委托编号 */
  char    ClientName[255]; /* 账户名称 */
  char    TransTime[17];   /* 委托时间 */
  int32_t OrdStatus;       /* 委托状态 */
  char    Symbol[40];      /* 证券代码 */
  int32_t Side;            /* 买卖方向 */
  int32_t SecType;         /* 证券类型 */
  int32_t SecExch;         /* 交易所类型 */
  double  Price;           /* 委托价格 */
  int32_t OrdType;         /* 算法类型 */
  int32_t OrderQty;        /* 委托数量 */
  int32_t CumQty;          /* 累计成交 */
  int32_t LeavesQty;       /* 剩余数量 */
  double  AvgPx;           /* 成交均价 */
  char    UpdateTime[17];  /* 更新时间 */
  char    Text[255];       /* 备注 */
} SubOrderAlgo;

//资金回报 dbf字段
typedef struct ReportBalance
{
  char    ClientName[255];  // 账户名称
  uint8_t EnBalance[23];    // .3f 可用资金
  uint8_t CrBalance[23];    // .3f 融资可用资金
  uint8_t AssetAmt[23];     // .3f 总资产
  uint8_t MartketAmt[23];   // .3f 总市值
  char    UpdateTime[17];   // 更新时间(20201217091705000)
} RBRecord;

//证券持仓回报 dbf字段
typedef struct ReportPosition
{
  char    ClientName[255];  // 账户名称
  uint8_t Exchange[3];      // 证券市场
  char    Symbol[40];       // 证券代码
  uint8_t CurrentQ[10];     // 当前数量
  uint8_t EnableQty[10];    // 可用数量
  uint8_t ShortQty[10];     // 融券可用
  char    UpdateTime[17];   // 更新时间(20201217091705000)
} RPRecord;

#pragma pack(pop)

#endif  // LIBDBF_ATGO_DEF_H
