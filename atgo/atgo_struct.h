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

#pragma pack(push, 1) /* ȷ��1�ֽڶ��� */

/* ĸ��ί���ļ��ṹ */
typedef struct
{
  char   ExternalId[30];  /* �Զ���ί�б�� (N) */
  char   ClientName[255]; /* �˻����� (Y) */
  char   Symbol[40];      /* ֤ȯ���룬��000001 (Y) */
  char   Side[3];         /* �������� (��Sideö��) (Y) */
  char   OrderQty[10];    /* ί������ (Y) */
  char   OrdType[6];      /* �㷨���� (��OrdTypeö��) (Y) */
  char   EffTime[17];     /* ��ʼʱ�� (YYYYMMDDHHMMSSmmm) (Y) */
  char   ExpTime[17];     /* ����ʱ�� (��ʽͬ��) (Y) */
  int8_t LimAction;       /* �ǵ�ͣ���Ƿ��� 0-�� 1-�� (Y) */
  int8_t AftAction;       /* ���ں��Ƿ��� 0-�� 1-�� (Y) */
  char   AlgoParam[255];  /* ���Բ��� (N) */
} OrderAlgo;

/* ����ί���ļ��ṹ */
typedef struct
{
  char QuoteId[20]; /* ĸ��ί�б�� (Y) */
  char CxlType[3];  /* �������� (��CxlTypeö��) (Y) */
} CancelOrderAlgo;

/* ĸ��ί�лر��ṹ */
typedef struct
{
  char   ExternalId[30];  /* �Զ���ί�б�� */
  char   QuoteId[20];     /* ϵͳί�б�� */
  char   ClientName[255]; /* �˻����� */
  char   Symbol[40];      /* ֤ȯ���� */
  char   SecType[3];      /* ֤ȯ���� (��SecurityTypeö��) */
  char   SecExch[3];      /* ���������� (��ExchangeTypeö��) */
  char   Side[3];         /* �������� */
  char   TransTime[17];   /* ί��ʱ�� */
  char   OrderQty[10];    /* ί������ */
  char   OrdType[6];      /* �㷨���� */
  char   Price[10];       /* �۸� (����3) */
  char   EffTime[17];     /* ��ʼʱ�� */
  char   ExpTime[17];     /* ����ʱ�� */
  int8_t LimAction;       /* �ǵ�ͣ���� */
  int8_t AftAction;       /* ���ڴ��� */
  char   AlgoParam[255];  /* ���Բ��� */
  char   CumQty[10];      /* �ۼƳɽ����� */
  char   LeavesQty[10];   /* ʣ������ */
  char   OutstaQty[10];   /* δ�ɽ����� */
  char   AvgPx[10];       /* ƽ���ɽ��� (����3) */
  char   OrdStatus[3];    /* ����״̬ */
  char   CxlType[3];      /* �������� */
  char   BasketId[255];   /* ���ӱ�� */
  char   Text[255];       /* ��ע��Ϣ */
  char   UpdateTime[17];  /* ����ʱ�� */
} ReportOrderAlgo;

/* �ӵ�ί�лر��ṹ */
typedef struct
{
  char ExternalId[30];  /* �Զ���ί�б�� */
  char QuoteId[20];     /* ĸ����� */
  char ClOrdId[20];     /* �ӵ���� */
  char OrderId[20];     /* �ⲿί�б�� */
  char ClientName[255]; /* �˻����� */
  char TransTime[17];   /* ί��ʱ�� */
  char OrdStatus[3];    /* ί��״̬ */
  char Symbol[40];      /* ֤ȯ���� */
  char Side[3];         /* �������� */
  char SecType[3];      /* ֤ȯ���� */
  char SecExch[3];      /* ���������� */
  char Price[10];       /* ί�м۸� */
  char OrdType[6];      /* �㷨���� */
  char OrderQty[10];    /* ί������ */
  char CumQty[10];      /* �ۼƳɽ� */
  char LeavesQty[10];   /* ʣ������ */
  char AvgPx[10];       /* �ɽ����� */
  char UpdateTime[17];  /* ����ʱ�� */
  char Text[255];       /* ��ע */
} SubOrderAlgo;

//�ʽ�ر� dbf�ֶ�
typedef struct ReportBalance
{
  char    ClientName[255];  // �˻�����
  uint8_t EnBalance[23];    // .3f �����ʽ�
  uint8_t CrBalance[23];    // .3f ���ʿ����ʽ�
  uint8_t AssetAmt[23];     // .3f ���ʲ�
  uint8_t MartketAmt[23];   // .3f ����ֵ
  char    UpdateTime[17];   // ����ʱ��(20201217091705000)
} RBRecord;

//֤ȯ�ֲֻر� dbf�ֶ�
typedef struct ReportPosition
{
  char    ClientName[255];  // �˻�����
  uint8_t Exchange[3];      // ֤ȯ�г�
  char    Symbol[40];       // ֤ȯ����
  uint8_t CurrentQ[10];     // ��ǰ����
  uint8_t EnableQty[10];    // ��������
  uint8_t ShortQty[10];     // ��ȯ����
  char    UpdateTime[17];   // ����ʱ��(20201217091705000)
} RPRecord;

#pragma pack(pop)

#endif  // LIBDBF_ATGO_DEF_H
