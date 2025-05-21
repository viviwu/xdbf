//
// Created by dejavu on 202018/4/2.
//

#ifndef DBF_DOC_ATGO_DICT_H
#define DBF_DOC_ATGO_DICT_H
/***********************************************
 * 数据字典枚举定义 (基于atgo_dts_dic.xlsx)
 ***********************************************/

/* 订单状态枚举 (适用于母单和子单) */
typedef enum {
    ORDSTATUS_INVALID       = -1,
    ORDSTATUS_NEW           = 0,   /* 已报 */
    ORDSTATUS_PARTIALFILL   = 1,   /* 部成 */
    ORDSTATUS_FILLED        = 2,   /* 已成 */
    ORDSTATUS_CANCELED      = 4,   /* 已撤 */
    ORDSTATUS_PENDINGCANCEL = 6,   /* 待撤 */
    ORDSTATUS_REJECTED      = 8,   /* 拒绝/废单 */
    ORDSTATUS_SUSPEND       = 9,   /* 暂停 */
    ORDSTATUS_PENDINGNEW    = 10   /* 未报 */
} OrdStatus;

/* 买卖方向枚举 */
typedef enum {
    SIDE_INVALID        = -1,
    SIDE_BUY            = 1,    /* 普通买入 */
    SIDE_SELL           = 2,    /* 普通卖出 */
    SIDE_CLOSE_BUY      = 3,    /* 买券还券 */
    SIDE_SHORT_SELL     = 4,    /* 融券卖出 */
    SIDE_SHORT_BUY      = 5,    /* 融资买入 */
    SIDE_CLOSE_SELL     = 6,    /* 卖券还款 */
    SIDE_FUTURE_OPEN_BUY  = 101, /* 买入开仓 */
    SIDE_FUTURE_OPEN_SELL = 102, /* 卖出开仓 */
    SIDE_FUTURE_CLOSE_BUY = 103, /* 买入平仓 */
    SIDE_FUTURE_CLOSE_SELL= 106  /* 卖出平仓 */
} Side;

/* 算法类型枚举 */
typedef enum {
    ORDTYPE_INVALID     = -1,
    ORDTYPE_TWAP_PLUS   = 101,  /* TWAP_Plus算法 */
    ORDTYPE_VWAP_PLUS   = 102,  /* VWAP_Plus算法 */
    ORDTYPE_TWAP_CORE   = 103,  /* TWAP_Core算法 */
    ORDTYPE_VWAP_CORE   = 104,  /* VWAP_Core算法 */
    ORDTYPE_POV_CORE    = 105,  /* POV_Core算法 */
    ORDTYPE_PASSTHRU    = 201   /* Passthru算法 */
} OrdType;

/* 撤单类型枚举 */
typedef enum {
    CXL_INVALID         = -1,
    CXL_NOTCANCELED     = 0,    /* 未发起撤单 */
    CXL_STOPTRADE       = 1,    /* 已发起撤单 */
    CXL_STOPINIT        = 2,    /* 禁开 */
    CXL_EXPIRED         = 3     /* 已过期 */
} CxlType;

/* 交易所类型枚举 */
typedef enum {
    EXCH_SZ             = 1,    /* 深交所 */
    EXCH_SH             = 2,    /* 上交所 */
    EXCH_CFE            = 3,    /* 中金所 */
    EXCH_SHF            = 4,    /* 上期所 */
    EXCH_DCE            = 5,    /* 大商所 */
    EXCH_ZCE            = 6,    /* 郑商所 */
    EXCH_INE            = 7     /* 能源中心 */
} ExchangeType;

//过期后是否继续交易：AfterAction
//字典名称	字典枚举值
//否	        0
//是	        1

//涨跌停是否继续交易：LimitAction
//字典名称	字典枚举值
//        否	0
//是	1

#endif //DBF_DOC_ATGO_DICT_H
