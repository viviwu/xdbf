//
// Created by dejavu on 202018/4/2.
//

#ifndef DBF_DOC_ATGO_DICT_H
#define DBF_DOC_ATGO_DICT_H
/***********************************************
 * �����ֵ�ö�ٶ��� (����atgo_dts_dic.xlsx)
 ***********************************************/

/* ����״̬ö�� (������ĸ�����ӵ�) */
typedef enum {
    ORDSTATUS_INVALID       = -1,
    ORDSTATUS_NEW           = 0,   /* �ѱ� */
    ORDSTATUS_PARTIALFILL   = 1,   /* ���� */
    ORDSTATUS_FILLED        = 2,   /* �ѳ� */
    ORDSTATUS_CANCELED      = 4,   /* �ѳ� */
    ORDSTATUS_PENDINGCANCEL = 6,   /* ���� */
    ORDSTATUS_REJECTED      = 8,   /* �ܾ�/�ϵ� */
    ORDSTATUS_SUSPEND       = 9,   /* ��ͣ */
    ORDSTATUS_PENDINGNEW    = 10   /* δ�� */
} OrdStatus;

/* ��������ö�� */
typedef enum {
    SIDE_INVALID        = -1,
    SIDE_BUY            = 1,    /* ��ͨ���� */
    SIDE_SELL           = 2,    /* ��ͨ���� */
    SIDE_CLOSE_BUY      = 3,    /* ��ȯ��ȯ */
    SIDE_SHORT_SELL     = 4,    /* ��ȯ���� */
    SIDE_SHORT_BUY      = 5,    /* �������� */
    SIDE_CLOSE_SELL     = 6,    /* ��ȯ���� */
    SIDE_FUTURE_OPEN_BUY  = 101, /* ���뿪�� */
    SIDE_FUTURE_OPEN_SELL = 102, /* �������� */
    SIDE_FUTURE_CLOSE_BUY = 103, /* ����ƽ�� */
    SIDE_FUTURE_CLOSE_SELL= 106  /* ����ƽ�� */
} Side;

/* �㷨����ö�� */
typedef enum {
    ORDTYPE_INVALID     = -1,
    ORDTYPE_TWAP_PLUS   = 101,  /* TWAP_Plus�㷨 */
    ORDTYPE_VWAP_PLUS   = 102,  /* VWAP_Plus�㷨 */
    ORDTYPE_TWAP_CORE   = 103,  /* TWAP_Core�㷨 */
    ORDTYPE_VWAP_CORE   = 104,  /* VWAP_Core�㷨 */
    ORDTYPE_POV_CORE    = 105,  /* POV_Core�㷨 */
    ORDTYPE_PASSTHRU    = 201   /* Passthru�㷨 */
} OrdType;

/* ��������ö�� */
typedef enum {
    CXL_INVALID         = -1,
    CXL_NOTCANCELED     = 0,    /* δ���𳷵� */
    CXL_STOPTRADE       = 1,    /* �ѷ��𳷵� */
    CXL_STOPINIT        = 2,    /* ���� */
    CXL_EXPIRED         = 3     /* �ѹ��� */
} CxlType;

/* ����������ö�� */
typedef enum {
    EXCH_SZ             = 1,    /* ��� */
    EXCH_SH             = 2,    /* �Ͻ��� */
    EXCH_CFE            = 3,    /* �н��� */
    EXCH_SHF            = 4,    /* ������ */
    EXCH_DCE            = 5,    /* ������ */
    EXCH_ZCE            = 6,    /* ֣���� */
    EXCH_INE            = 7     /* ��Դ���� */
} ExchangeType;

//���ں��Ƿ�������ף�AfterAction
//�ֵ�����	�ֵ�ö��ֵ
//��	        0
//��	        1

//�ǵ�ͣ�Ƿ�������ף�LimitAction
//�ֵ�����	�ֵ�ö��ֵ
//        ��	0
//��	1

#endif //DBF_DOC_ATGO_DICT_H
