/******************************************************************************

                  ��Ȩ���� (C), 2009-2009, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : api.h
  �� �� ��   : ����
  ��    ��   :
  ��������   : 2009��9��9��
  ����޸�   :
  ��������   : api.cpp ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __API_H__
#define __API_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define CARD_COUNT   5       //��������

#define MIN_AUTONO   10      //��С���ƺ�
#define MAX_AUTONO   20      //����ƺ�

typedef unsigned char UC;
typedef unsigned long UL;

//��������
typedef enum
{
    CAR,     //С�γ�
    BUS,     //���
    AUTO_TYPE_BUTT,
}ParkSysAutoType;

//��״̬
typedef enum
{
    CARD_IDLE  = 0,  //������
    CARD_INUSE = 1,  //���Ѿ�ʹ��
}CardStatus;

//�����붨��
typedef enum
{
    OP_E_CMD                    = 0,  //(������ʹ��)��Ч������
    OP_RESET_SUCC               = 1,  //ϵͳ��ʼ���ɹ�
    OP_E_PARAMETER              = 2,  //�������Ϸ�
    OP_E_PARKING_FULL           = 3,  //��λ����
    OP_E_EXISTED_ATUO           = 4,  //�ó��Ѿ���ͣ����
    OP_E_MISMATCH_AUTO_TYPE     = 5,  //���������Ƴ����������
    OP_E_NOT_IN_PARKING         = 6,  //�ó�����ͣ����
    OP_E_AUTONO_MISMATCH_CARDNO = 7,  //���ƺźͿ��Ų���Ӧ�������뿪
    OP_E_TIME_LOGIC             = 8,  //ʱ���߼�����
    OP_PARKING_SUCC             = 9,  //ͣ�������ɹ�
    OP_SPEC_CAR_PARKING         = 10, //ָ���ĳ�����ͣ��������δͣ��
    OP_ANY_CAR_PAKING           = 11, //ͣ��������δ���κγ���ͣ��
    OP_INTERNAL_ERR             =999, //�ڲ�ϵͳ����
}ParkSysOpRet;

//ʱ��
typedef struct
{
    UC hh;  //Сʱ
    UC mm;  //����
}ParkSysTime;

//��ͣ�����еĳ�����Ϣ
typedef struct
{
    UL              AutoNo;       //���ƺ�
    ParkSysTime     CurGetInTime; //��ǰ����ʱ��
    UL              CardNo;       //����
    ParkSysAutoType AutoType;     //����
    UC              ParkingCount; //ͣ������
}ParkingAutoInfo;

//ͣ�����г�����ʷͣ����Ϣ
typedef struct
{
    UL              AutoNo;       //���ƺ�
    ParkSysAutoType AutoType;     //��������
    UL              CardNo;       //����
    ParkSysTime     GetInTime;    //ͣ��ʱ��
    ParkSysTime     GetOutTime;   //ȡ��ʱ��
    UL              Fee;          //ͣ������
}ParkingAutoHistoryInfo;


#define PARKING_SPACE 5

#define AUTO_COUNT   ((MAX_AUTONO) - (MIN_AUTONO) + 1)

#define TRUE  1
#define FALSE 0

#define MAX_HOURS          24
#define MINUTES_OF_ONEHOUR 60
#define MINUTES_OF_FREE    30     //ǰ30�������

#define FIRST_DISCOUNT_RATE   100 //��һ�β�����
#define SECOND_DISCOUNT_RATE  80  //�ڶ��μ��Ժ�8��

#define LOW_FIX_SEPARATE_STEP   2
#define STEP_SEPARATE_UPPER_FIX 12
#define MAX_LIMIT_FEE        30


#define EXAM_SET_CARD_STATUS(CardNo, Status) \
gCardStatus[CardNo] = Status

#define EXAM_CHK_PRAR_DIGITAL_RETURN(DigitalVal, LowerLimit, UpperLimit) \
if ((DigitalVal) < (LowerLimit) || (DigitalVal) > (UpperLimit)) \
{ \
    api_parksys_ret(OP_E_PARAMETER); \
    return; \
}

#define EXAM_CHK_PRAR_TIME_RETURN(hh,mm) \
if ((hh) > MAX_HOURS \
 || (mm) > (MINUTES_OF_ONEHOUR - 1) \
 || ((hh) == MAX_HOURS && (mm) != 0)) \
{ \
    api_parksys_ret(OP_E_PARAMETER); \
    return; \
}

#define EXAM_TRUE_RETURN(condition, OpResult) \
if ((condition)) \
{ \
    api_parksys_ret(OpResult); \
    return; \
}

//��������
typedef enum _BasicFeeRate
{
    LOW_FIX_FEE,   //ǰ2Сʱ�̶���������
    STEP_FEE,      //�м�ÿСʱ����ֵ����
    HIGH_FIX_FEE,  //����12Сʱ�̶���������
    BASI_FEE_BUTT,
}BasicFeeRate;


//����״̬
typedef enum _AutoStatus
{
    AUTO_IN_PARKING,  //��ͣ����
    AUTO_OUT_PARKING, //����ͣ����
    AUTO_NO_LICENSE,  //����ִ�յ���
}AutoStatus;

//������Ϣ
typedef struct _AutoInfo
{
    UL              AutoNo;        //���ƺ�
    ParkSysAutoType AutoType;      //��������
    UL              CardNo;        //����
    UC              EnterCount;    //�������
    AutoStatus      Status;        //����״̬
    ParkSysTime     GetInTime;     //����ʱ��
    ParkSysTime     GetOutTime;    //�뿪ʱ��
    ParkSysTime     LastLeaveTime; //���һ���뿪ʱ��
}AutoInfo;


//ͣ������Ϣ
typedef struct _ParkingInfo
{
    UL           IdleParkSpaceNumber; //���г�λ��
    UC           bInitial;            //ͣ�����Ƿ��ʱ״̬��ΪTRUE,�г����ɹ���������FALSE
                                      //�ڸ���LastOpTime�жϺ����Ĳ���ʱ���Ƿ���˳���
    ParkSysTime  LastOpTime;          //���һ�β���ʱ��
}ParkingInfo;

//��ʷͣ����Ϣ����ṹ
typedef struct tagAutoHistoryInfoNode
{
    ParkingAutoHistoryInfo          data;  //��ʷͣ����Ϣ
    struct tagAutoHistoryInfoNode   *pNext;
}AutoHistoryInfoNode;

AutoHistoryInfoNode * CreateList(void);
AutoHistoryInfoNode * FindNodeByCardNo(AutoHistoryInfoNode *pHead, UL uAutoNo);
int PushBackNode(AutoHistoryInfoNode *pHead, ParkingAutoHistoryInfo *pAutoInfo);
int RemoveNodeByAutoNo(AutoHistoryInfoNode *pHead, UL AutoNo);
int RemoveList(AutoHistoryInfoNode *pHead);


/*****************************************************************************
 �� �� ��  : api_parksys_ret
 ��������  : ϵͳ�ṩ�ĺ���,����������,��ӡ���������Ϣ
 �������  : ParkSysOpRet OpRet�����붨��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void api_parksys_ret(ParkSysOpRet OpRet);

/*****************************************************************************
 �� �� ��  : api_parksys_fee
 ��������  : ϵͳ�ṩ�ĺ���,����������,�����뿪ͣ��������ȡ�ķ���
 �������  : UL fee����ͣ����ȡ�ķ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void api_parksys_fee(UL fee);

/*****************************************************************************
 �� �� ��  : api_parksys_idle_parkingspace_number
 ��������  : ϵͳ�ṩ�ĺ���,����������,ͣ�������г�λ��ѯ���
 �������  : UL number���г�λ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void api_parksys_idle_parkingspace_number(UL number);

/*****************************************************************************
 �� �� ��  : api_parksys_card_status
 ��������  : ϵͳ�ṩ�ĺ���,����������,ָ����״̬��ѯ���
 �������  : CardStatus cardStatusָ����״̬
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void api_parksys_card_status(CardStatus cardStatus);

/*****************************************************************************
 �� �� ��  : api_parksys_parking_auto
 ��������  : ϵͳ�ṩ�ĺ���,����������,ͣ������ָ��������Ϣ��ѯ���
 �������  : ParkingAutoInfo *pAutoInfoͣ������ָ��������Ϣ
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void api_parksys_parking_auto(ParkingAutoInfo *pAutoInfo);

/*****************************************************************************
 �� �� ��  : api_parksys_parking_auto_history_info
 ��������  : ϵͳ�ṩ�ĺ���,����������,ͣ������ָ��������Ϣ��ѯ���
 �������  : ParkingAutoHistoryInfo *pAutoHistoryInfoͣ��������ʷ����ͣ����Ϣ
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void api_parksys_parking_auto_history_info(ParkingAutoHistoryInfo *pAutoHistoryInfo);


/*****************************************************************************
 �� �� ��  : api_server_start
 ��������  : ����socket����ˣ��������ķ�ʽ�����û�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void apiServerStart(int argc, char* argv[]);


// ������ʵ�ֵĺ���

void OpReset(void);

void OpGetin(UL AutoNo, ParkSysAutoType AutoType, ParkSysTime InTime);

void OpGetout(UL AutoNo, UL CardNo, ParkSysTime OutTime);

void OpListIdleParkingSpaceNumber(void);

void OpListCardStatus(UL CardNo);

void OpListParkingAuto(UL AutoNo);

void OpListParkingAutoHistoryInfo(UL AutoNo);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __API_H__ */
