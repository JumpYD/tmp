/******************************************************************************

                  ��Ȩ���� (C), 2009-2009, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : car.h
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2010��1��21��
  ����޸�   :
  ��������   : car.cpp ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2010��1��21��
    ��    ��   :
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __CAR_H__
#define __CAR_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define MAX_PECCANCY_RECORD_NUM           10           //���Υ���¼��
#define MAX_SCORE_VALUE                   20           //��ʼ����
#define MAX_DAYS_VALUE                    1000         //���ʱ�䣨�죩
#define MAX_MONEY_VALUE                   6000         //�����

#define RET_SUCC                          0
#define RET_FAILURE                       -1

/*Υ������*/
enum enPeccancyType
{
    PECCANCY_TYPE_0,            //Υ��ͣ��
    PECCANCY_TYPE_1,            //����
    PECCANCY_TYPE_2,            //�����
    PECCANCY_TYPE_3,            //����
    PECCANCY_TYPE_4,            //��Ƽݳ�
    PECCANCY_TYPE_BUTT
};

/*Υ�淣��ɷ�״̬*/
enum enPayStat
{
    STAT_NO_PAY,                //δ���ɷ���
    STAT_HAVE_PAY               //�ѽ��ɷ���
};

typedef struct PECCANCY_RECORD
{
    int PeccancyGlobalIndex;                            //ȫ��Υ�����
    int PeccancyType;                                   //Υ������
    int Days;                                           //ʱ��
    int fee;                                            //����
}PeccancyRecord;

typedef struct CAR_INFO
{
    bool WithdrawFlag;                                  //���Ƶ�����ʶ
    int   Score;                                        //ʣ�����
    int   RecordNum;                                    //Υ���¼��
    PeccancyRecord Record[MAX_PECCANCY_RECORD_NUM];     //Υ���¼�����֧��10��
}CarInfo;

typedef struct PECCANCY_RULE
{
    int Fee;                                            //����
    int Score;                                          //�۷�
}PeccancyRule;

typedef struct AutoHistoryNodeType
{
    QueryResult                 data;   //��ʷ��Ϣ�ṹ
    struct AutoHistoryNodeType  *pNext;
}AutoHistoryNodeTypeInfo;

/*****************************************************************************
 �� �� ��  : opInit
 ��������  : ������Ҫʵ�ֵĽӿ�
             ��ɡ�ϵͳ��ʼ��������
             ����ʵ����i
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��1��21��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void opInit(void);

/*****************************************************************************
 �� �� ��  : opRecord
 ��������  : ������Ҫʵ�ֵĽӿ�
             ��ɡ�¼��Υ���¼������
             ����ʵ����r 0-1
 �������  : int Peccancy  :    Υ������
             int Days      :    ʱ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��1��21��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void opRecord(int PeccancyType, int Days);


/*****************************************************************************
 �� �� ��  : opQuery
 ��������  : ������Ҫʵ�ֵĽӿ�
             ��ɲ�ѯΥ���¼����
             ����ʵ����q
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��1��21��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void opQuery(void);


/*****************************************************************************
 �� �� ��  : opPay
 ��������  : ������Ҫʵ�ֵĽӿ�
             ��ɽ���Υ�淣�����
             ����ʵ����p 100
 �������  : int  Money       :    ���ɽ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��1��21��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void opPay(int Money);

void opPayFee(int *pMoney);
void opPayLargeFeeOnSameDay(int *pMoney, int i);
void opDeleteRecord(int i);
AutoHistoryNodeTypeInfo* CreateList(void);
AutoHistoryNodeTypeInfo* Insert2ListTail(AutoHistoryNodeTypeInfo *pHead, QueryResult *pAutoInfo);
AutoHistoryNodeTypeInfo* FindNodeByGlobalIndex(AutoHistoryNodeTypeInfo *pHead, int iGlobalIndex);
int RemoveList(AutoHistoryNodeTypeInfo *pHead);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CAR_H__ */
