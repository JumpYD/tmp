#include <stdio.h>
#include <string.h>
#include "api.h"


//����,ʱ����Ӧ�ķ��ʱ�
UL gBasicFeeTbl[AUTO_TYPE_BUTT][BASI_FEE_BUTT] =
{
    {10, 1, 20},
    {20, 1, 30},
};

AutoInfo gAutoInfo[AUTO_COUNT];
CardStatus gCardStatus[CARD_COUNT];

AutoHistoryInfoNode *g_pAutoHistoryInfo = NULL;
ParkingInfo gParkingInfo;


void Exam_InitAutoInfo(void)
{
    UL i;

    for (i = 0; i < AUTO_COUNT; i++)
    {
        gAutoInfo[i].AutoNo     = MIN_AUTONO + i;
        gAutoInfo[i].EnterCount = 0;
        gAutoInfo[i].Status     = AUTO_OUT_PARKING;
    }
    return;
}

void Exam_InitCardStatus(void)
{
    UL i;

    for (i = 0; i < CARD_COUNT; i++)
    {
        gCardStatus[i] = CARD_IDLE;
    }
    return;
}

UL Exam_GetMinimumCardNo(void)
{
    UL i;

    for (i = 0; i < CARD_COUNT; i++)
    {
        if (CARD_IDLE == gCardStatus[i])
        {
            return i;
        }
    }

    return i;
}

UL Exam_GetParkingHours(AutoInfo *pAutoInfo)
{
    UL tmpHours = 0, tmpMinutes = 0;
    UL TotalMinutes = 0;

    //ǰ���Ѿ���֤��Out��ʱ��϶�����In��ʱ��
    tmpHours = pAutoInfo->GetOutTime.hh - pAutoInfo->GetInTime.hh;

    if (pAutoInfo->GetOutTime.mm < pAutoInfo->GetInTime.mm)
    {
        tmpHours--;
        tmpMinutes = MINUTES_OF_ONEHOUR + pAutoInfo->GetOutTime.mm -pAutoInfo->GetInTime.mm;
    }
    else
    {
        tmpMinutes = pAutoInfo->GetOutTime.mm -pAutoInfo->GetInTime.mm;
    }

    TotalMinutes = tmpHours * MINUTES_OF_ONEHOUR + tmpMinutes;

    //ǰ30�������
    if (TotalMinutes <= MINUTES_OF_FREE)
    {
        TotalMinutes = 0;
    }

    return ((TotalMinutes + MINUTES_OF_ONEHOUR - 1)/ MINUTES_OF_ONEHOUR);
}

UL Exam_GetFee(AutoInfo *pAutoInfo)
{
    UL totalHours;
    UL fee = 0;

    totalHours = Exam_GetParkingHours(pAutoInfo);

    if (0 == totalHours)
    {
        fee = 0;
    }
    else if (totalHours <= LOW_FIX_SEPARATE_STEP)
    {
        fee = gBasicFeeTbl[pAutoInfo->AutoType][LOW_FIX_FEE];
    }
    else if (totalHours <= STEP_SEPARATE_UPPER_FIX)
    {
        fee = gBasicFeeTbl[pAutoInfo->AutoType][LOW_FIX_FEE]
             +gBasicFeeTbl[pAutoInfo->AutoType][STEP_FEE] * (totalHours - LOW_FIX_SEPARATE_STEP);
    }
    else
    {
        fee = gBasicFeeTbl[pAutoInfo->AutoType][HIGH_FIX_FEE];
    }

    return fee;
}

//ʱ���߼����Ta�Ƿ��Ƿ�����Tb
UL Exam_TaLessThanTb(ParkSysTime TimeA, ParkSysTime TimeB)
{
    if ((TimeB.hh * MINUTES_OF_ONEHOUR + TimeB.mm)
      > (TimeA.hh * MINUTES_OF_ONEHOUR + TimeA.mm))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void main(int argc, char* argv[])
{
    /*
    ����Socket��������5555�˿�(sapi_server_start������lib����ʵ��)��
    lib����ʵ�ִ�Socket���յ��ַ����������ַ�����
    */
    apiServerStart(argc,argv);
    // �˴�����ִ�е���ע�ⲻҪ�ڴ˴���Ӵ���
}

/*****************************************************************************
 �� �� ��  : OpReset
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,ϵͳ��ʼ��
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
void OpReset(void)
{
    Exam_InitAutoInfo();

    Exam_InitCardStatus();

    gParkingInfo.IdleParkSpaceNumber = PARKING_SPACE;
    gParkingInfo.bInitial            = TRUE;

    //ɾ������
    if (NULL != g_pAutoHistoryInfo)
    {
        if (TRUE != RemoveList(g_pAutoHistoryInfo))
        {
            api_parksys_ret(OP_INTERNAL_ERR);
            return;
        }
        g_pAutoHistoryInfo = NULL;
    }

    //��������
    g_pAutoHistoryInfo = CreateList();
    if (NULL == g_pAutoHistoryInfo)
    {
        api_parksys_ret(OP_INTERNAL_ERR);
        return;
    }

    api_parksys_ret(OP_RESET_SUCC);

    return;
}

/*****************************************************************************
 �� �� ��  : OpGetin
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,��������ͣ����
 �������  : UL AutoNo ���ƺ�
             ParkSysAutoType AutoType ��������
             ParkSysTime InTime ��������ͣ����ʱ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void OpGetin(UL AutoNo, ParkSysAutoType AutoType, ParkSysTime InTime)
{
    UL MinimumIdleCardNo;
    UL TimeLogicFlag;
    //UL CardNo;
    UL ActAutoNo = AutoNo - MIN_AUTONO; //���ƺ���ƫ��

//�����Ϸ��Լ��
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoNo, MIN_AUTONO, MAX_AUTONO);
    //EXAM_CHK_PRAR_DIGITAL_RETURN(CardNo, 0, CARD_COUNT - 1);
    EXAM_CHK_PRAR_TIME_RETURN(InTime.hh, InTime.mm);
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoType, CAR, BUS);

//�߼��������
    //�п��г�λ������������
    EXAM_TRUE_RETURN(gParkingInfo.IdleParkSpaceNumber < 1, OP_E_PARKING_FULL);

    //ͬһ���ƺŵĳ����ɶ�ν���ͣ����,�����ͣ������,�����ٴν���ͣ����
    EXAM_TRUE_RETURN(AUTO_IN_PARKING == gAutoInfo[ActAutoNo].Status, OP_E_EXISTED_ATUO);

    if (gAutoInfo[ActAutoNo].EnterCount > 0)
    {
        //ͬһ�����ظ�����ͣ����ʱ,�������ͱ�������һ��һ��
        if (gAutoInfo[ActAutoNo].AutoType != AutoType)
        {
            api_parksys_ret(OP_E_MISMATCH_AUTO_TYPE);

            //�������ƣ���մ˳�����������Ϣ����ǰ���㷨�Ѿ�ʵ����ͬһ���Ƶĳ��������ȳ�ͣ��������˴˴��������ͷſ��ͳ�λ
            //����˳�����������ʷͣ����Ϣ
            if (TRUE != RemoveNodeByAutoNo(g_pAutoHistoryInfo, AutoNo))
            {
                api_parksys_ret(OP_INTERNAL_ERR);
                return;
            }

            return;
        }
    }

    //ʱ����,�����ʱ��>=�ϴ��뿪��ʱ��
    if (gAutoInfo[ActAutoNo].EnterCount > 0)
    {
        TimeLogicFlag = Exam_TaLessThanTb(InTime, gAutoInfo[ActAutoNo].LastLeaveTime);
        EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);
    }

    //ʱ����,��ǰ�����ͣ������ʱ��>=��һ�ν����ͣ������ʱ��
    if (!gParkingInfo.bInitial)
    {
        TimeLogicFlag = Exam_TaLessThanTb(InTime, gParkingInfo.LastOpTime);
        EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);
    }

    //Ŀǰ�Ĺ�񲻻���ֳ������޵����
    MinimumIdleCardNo = Exam_GetMinimumCardNo();

    //����ͣ����Ϣ
    gAutoInfo[ActAutoNo].AutoType   = AutoType;
    gAutoInfo[ActAutoNo].EnterCount++;
    gAutoInfo[ActAutoNo].CardNo     = MinimumIdleCardNo;
    gAutoInfo[ActAutoNo].Status     = AUTO_IN_PARKING;
    gAutoInfo[ActAutoNo].GetInTime  = InTime;

    EXAM_SET_CARD_STATUS(MinimumIdleCardNo, CARD_INUSE);

    gParkingInfo.IdleParkSpaceNumber--;
    gParkingInfo.bInitial   = FALSE;
    gParkingInfo.LastOpTime = InTime;

    //��¼����ʷ��Ϣ��
    ParkingAutoHistoryInfo stAutoInfo;

    stAutoInfo.AutoNo       = gAutoInfo[ActAutoNo].AutoNo;
    stAutoInfo.AutoType     = gAutoInfo[ActAutoNo].AutoType;
    stAutoInfo.CardNo       = gAutoInfo[ActAutoNo].CardNo;
    stAutoInfo.GetInTime    = gAutoInfo[ActAutoNo].GetInTime;
    stAutoInfo.GetOutTime.hh= 99;
    stAutoInfo.GetOutTime.mm= 99;
    stAutoInfo.Fee          = 0;

    if (TRUE != PushBackNode(g_pAutoHistoryInfo, &stAutoInfo))
    {
        api_parksys_ret(OP_INTERNAL_ERR);
        return;
    }

    api_parksys_ret(OP_PARKING_SUCC);

    return;
}

/*****************************************************************************
 �� �� ��  : OpGetout
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,�����뿪ͣ����
 �������  : UL AutoNo ���ƺ�
             UL CardNo ����
             ParkSysTime InTime ��������ͣ����ʱ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void OpGetout(UL AutoNo, UL CardNo, ParkSysTime OutTime)
{
    UL fee;
    UL TimeLogicFlag;
    UL ActAutoNo = AutoNo - MIN_AUTONO; //���ƺ���ƫ��

//�����Ϸ��Լ��
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoNo, MIN_AUTONO, MAX_AUTONO);
    EXAM_CHK_PRAR_DIGITAL_RETURN(CardNo, 0, CARD_COUNT - 1);
    EXAM_CHK_PRAR_TIME_RETURN(OutTime.hh, OutTime.mm);

//�߼��������
    //���������Ƚ���ͣ������ſ����뿪
    EXAM_TRUE_RETURN(AUTO_IN_PARKING != gAutoInfo[ActAutoNo].Status, OP_E_NOT_IN_PARKING);

    //���ƺźͿ�����Ҫ�����ʱ����Ϣһ��
    EXAM_TRUE_RETURN(CardNo != gAutoInfo[ActAutoNo].CardNo, OP_E_AUTONO_MISMATCH_CARDNO);

    //ʱ���߼�,�����ʱ��>=�ϴ��뿪��ʱ��
    TimeLogicFlag = Exam_TaLessThanTb(OutTime, gAutoInfo[ActAutoNo].GetInTime);
    EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);

    //ʱ���߼�,��ǰ�����ͣ������ʱ��>=��һ�ν����ͣ������ʱ��
    if (!gParkingInfo.bInitial)
    {
        TimeLogicFlag = Exam_TaLessThanTb(OutTime, gParkingInfo.LastOpTime);
        EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);
    }

    //�뿪ͣ��������
    gAutoInfo[ActAutoNo].Status        = AUTO_OUT_PARKING;
    gAutoInfo[ActAutoNo].GetOutTime    = OutTime;
    gAutoInfo[ActAutoNo].LastLeaveTime = OutTime;

    EXAM_SET_CARD_STATUS(CardNo, CARD_IDLE);

    gParkingInfo.IdleParkSpaceNumber++;
    gParkingInfo.bInitial   = FALSE;
    gParkingInfo.LastOpTime = OutTime;

    //���ü���
    fee  = Exam_GetFee(&gAutoInfo[ActAutoNo]);

    //ˢ��ȡ��ʱ�估����
    AutoHistoryInfoNode *pNode = NULL;
    pNode = g_pAutoHistoryInfo;
    while (NULL != pNode)
    {
        pNode = FindNodeByCardNo(pNode, AutoNo);
        if (AutoNo == pNode->data.AutoNo)
        {
            if (99 == pNode->data.GetOutTime.mm)
            {
                pNode->data.GetOutTime = OutTime;
                pNode->data.Fee        = fee;
                break;
            }
        }
    }

    api_parksys_fee(fee);

    return;
}

/*****************************************************************************
 �� �� ��  : OpList
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,ͣ�������г�λ��ѯ
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
void OpListIdleParkingSpaceNumber(void)
{
    api_parksys_idle_parkingspace_number(gParkingInfo.IdleParkSpaceNumber);

    return;
}

/*****************************************************************************
 �� �� ��  : OpList
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,ָ����״̬��ѯ
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
void OpListCardStatus(UL CardNo)
{
    //�����Ϸ��Լ��
    EXAM_CHK_PRAR_DIGITAL_RETURN(CardNo, 0, CARD_COUNT - 1);

    api_parksys_card_status(gCardStatus[CardNo]);

    return;
}

/*****************************************************************************
 �� �� ��  : OpList
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,ͣ������ָ��������Ϣ��ѯ
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
void OpListParkingAuto(UL AutoNo)
{
    UL ActAutoNo = AutoNo - MIN_AUTONO; //���ƺ���ƫ��
    ParkingAutoInfo AutoInfo;

    //�����Ϸ��Լ��
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoNo, MIN_AUTONO, MAX_AUTONO);

    if (AUTO_IN_PARKING == gAutoInfo[ActAutoNo].Status)
    {
        AutoInfo.AutoNo       = gAutoInfo[ActAutoNo].AutoNo;
        AutoInfo.CurGetInTime = gAutoInfo[ActAutoNo].GetInTime;
        AutoInfo.CardNo       = gAutoInfo[ActAutoNo].CardNo;
        AutoInfo.AutoType     = gAutoInfo[ActAutoNo].AutoType;
        AutoInfo.ParkingCount = gAutoInfo[ActAutoNo].EnterCount;

        api_parksys_parking_auto(&AutoInfo);
    }
    else
    {
        api_parksys_ret(OP_E_NOT_IN_PARKING);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : OpListParkingAutoHistoryInfo
 ��������  : ��Ҫ����ʵ�ֵĽӿ�,ͣ��������ʷ���г���ͣ����Ϣ��ѯ
 �������  : UL AutoNo ���ƺ�
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��9��
    ��    ��   :
    �޸�����   : �����ɺ���

*****************************************************************************/
void OpListParkingAutoHistoryInfo(UL AutoNo)
{

    AutoHistoryInfoNode *pNode = NULL;

    //�����Ϸ��Լ��
    if ((0 != AutoNo) && (AutoNo < MIN_AUTONO) || (AutoNo > MAX_AUTONO))
    {
        api_parksys_ret(OP_E_PARAMETER);
        return;
    }

    if (0 != AutoNo)
    {
        //�Ȳ����Ƿ��ж�Ӧ���ŵļ�¼
        if (NULL == FindNodeByCardNo (g_pAutoHistoryInfo, AutoNo))
        {
            api_parksys_ret(OP_SPEC_CAR_PARKING);
            return;
        }

        pNode = g_pAutoHistoryInfo->pNext;
        while (NULL != pNode)
        {
            if (AutoNo == pNode->data.AutoNo)
            {
                api_parksys_parking_auto_history_info (&pNode->data);
            }
            pNode = pNode->pNext;
        }
    }
    else
    {
        UL i = 0;
        for (i = MIN_AUTONO; i <= MAX_AUTONO; i++)
        {
             //�Ȳ����Ƿ��ж�Ӧ���ŵļ�¼
            if (NULL == g_pAutoHistoryInfo->pNext)
            {
                api_parksys_ret(OP_ANY_CAR_PAKING);
                return;
            }

            pNode = g_pAutoHistoryInfo->pNext;
            while (NULL != pNode)
            {
                if (i == pNode->data.AutoNo)
                {
                    api_parksys_parking_auto_history_info (&pNode->data);
                }
                pNode = pNode->pNext;
            }
        }
    }

    return;
}

