#include <string.h>
#include "stdio.h"
#include "api.h"
#include "car.h"

/*Υ�����Ͷ�Ӧ�ķ���Ϳ۷�*/
PeccancyRule Rule[]=
{
    {100,1},    //Υ������0
    {200,2},    //Υ������1
    {300,3},    //Υ������2
    {400,4},    //Υ������3
    {500,5},    //Υ������4
    {0,0}       //BUTTֵ
};


CarInfo         stCarInfo;          //������Ϣȫ�ּ�¼��
int             CurrentDays;        //��ǰ����ʱ��
bool            IsSystemInit=false; //ϵͳ��ʼ����־
int             g_iGlobalIndex = 0; //��¼¼���Υ���¼��ȫ�����

AutoHistoryNodeTypeInfo *g_pAutoHistoryInfoHead = NULL;   //������ʷ��Ϣָ��ͷ

void main(int argc, char* argv[])
{
    /* ����Socket��������5555�˿�(sapi_server_start������lib����ʵ��)��
     * lib����ʵ�ִ�Socket���յ��ַ����������ַ�����
     * ����ֻ��Ҫʵ�ַַ���ĸ�����ɡ�
    */
    api_server_start(argc, argv);
	// �˴�����ִ�е���ע�ⲻҪ�ڴ˴���Ӵ���
}

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
void opInit(void)
{
    //�ָ�����δ������־
    stCarInfo.WithdrawFlag = false;

    //�ָ��������ֳ�ʼֵ
    stCarInfo.Score = MAX_SCORE_VALUE;

    //����Υ���¼�����
    stCarInfo.RecordNum = 0;

    //�����ʷΥ���¼
    memset(stCarInfo.Record, 0, (sizeof(PeccancyRecord) * MAX_PECCANCY_RECORD_NUM));

    //��ǰʱ����0
    CurrentDays = 0;

    //��ϵͳ��ʼ����־λtrue
    IsSystemInit = true;

    //¼���ȫ��Υ�������0
    g_iGlobalIndex = 0;

    //��ճ�����ʷΥ����Ϣ��¼  --xhj
    if (NULL != g_pAutoHistoryInfoHead)
    {
        if (RET_SUCC != RemoveList(g_pAutoHistoryInfoHead))
        {
            api_print_result(E999);
            return;
        }
        g_pAutoHistoryInfoHead = NULL;
    }

    //�������������ڴ洢��ʷΥ����Ϣ
    g_pAutoHistoryInfoHead = CreateList();
    if (NULL == g_pAutoHistoryInfoHead)
    {
        api_print_result(E999);
        return;
    }

    //��ʼ���ɹ�
    api_print_result(S001);

    return;
}

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
void opRecord(int PeccancyType, int Days)
{
    QueryResult stOneCarInfo = { 0 };

    //ϵͳ��δ��ʼ��
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    //�����Υ�����Ͳ��Ϸ�
    if ((PeccancyType < PECCANCY_TYPE_0) || (PECCANCY_TYPE_4 < PeccancyType))
    {
        api_print_result(E003);
        return;
    }

    //�����ʱ�䲻�Ϸ�
    if ((Days <= 0) || (MAX_DAYS_VALUE < Days))
    {
        api_print_result(E002);
        return;
    }

    //�����ʱ��С�ڵ�ǰʱ��
    if (Days < CurrentDays)
    {
        api_print_result(E008);
        return;
    }

    //��δ�ɷѵ�Υ���¼����
    if (MAX_PECCANCY_RECORD_NUM <=  stCarInfo.RecordNum)
    {
        api_print_result(E009);
        return;
    }

    //�����Ѿ�����
    if (stCarInfo.WithdrawFlag == true)
    {
        api_print_result(E004);
        return;
    }

    //��¼Υ����Ϣ��������Ϣͳ�Ʊ���
    stCarInfo.Record[stCarInfo.RecordNum].PeccancyType = PeccancyType;
    stCarInfo.Record[stCarInfo.RecordNum].Days = Days;
    stCarInfo.Record[stCarInfo.RecordNum].fee = Rule[PeccancyType].Fee;

    //����˴�Υ�����ۻ��ִ���ʣ����֣�����ֱ����0���ó��Ƶ���
    if (Rule[PeccancyType].Score >= stCarInfo.Score)
    {
        stCarInfo.WithdrawFlag = true;
        stCarInfo.Score = 0;
    }
    else
    {
        stCarInfo.Score = stCarInfo.Score - Rule[PeccancyType].Score;
    }

    //Υ�����Ĭ�ϴ�1��ʼ��0Ϊ��Ч
    g_iGlobalIndex++;
    stCarInfo.Record[stCarInfo.RecordNum].PeccancyGlobalIndex = g_iGlobalIndex;

    if (true == stCarInfo.WithdrawFlag)
    {
        //���Ƶ����������ʷ����Υ���¼��Ϣ --xhj
        if (RET_SUCC != RemoveList(g_pAutoHistoryInfoHead))
        {
            api_print_result(E999);
            return;
        }

        //Υ���¼�����
        g_iGlobalIndex = 0;
        g_pAutoHistoryInfoHead = NULL;
    }
    else
    {
        //���Υ���¼����ʷΥ����Ϣ�б��� --xhj
        stOneCarInfo.Index   = g_iGlobalIndex;
        stOneCarInfo.stCarSysInfo.Reason = stCarInfo.Record[stCarInfo.RecordNum].PeccancyType;
        stOneCarInfo.stCarSysInfo.Time   = stCarInfo.Record[stCarInfo.RecordNum].Days;
        stOneCarInfo.Fee                 = stCarInfo.Record[stCarInfo.RecordNum].fee;
        stOneCarInfo.PayFlag = STAT_NO_PAY;
        stOneCarInfo.Score   = stCarInfo.Score;

        if (NULL == Insert2ListTail(g_pAutoHistoryInfoHead, &stOneCarInfo))
        {
            api_print_result(E999);
            return;
        }
    }

    //��¼Υ���������ǰʱ��
    stCarInfo.RecordNum++;
    CurrentDays = Days;

    api_print_result(S002);

    return;
}

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
void opQuery(void)
{
    //ϵͳ��δ��ʼ��
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    if (true == stCarInfo.WithdrawFlag)
    {
        api_print_result(E004);
        return;
    }

    //���κ�Υ���¼
    if ((0 == g_iGlobalIndex) || (NULL == g_pAutoHistoryInfoHead))
    {
        api_print_result (E010);
        return;
    }

    //��ӡ��ʷΥ����Ϣ��socket  --xhj
    AutoHistoryNodeTypeInfo *pNode = NULL;
    pNode = g_pAutoHistoryInfoHead->pNext;
    while (NULL != pNode)
    {
        api_print_query_info(&(pNode->data));
        pNode = pNode->pNext;
    }

    return;
}

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
void opPay(int Money)
{
    PayResult stResult;
    int i;
    int TmpMoney;

    //ϵͳ��δ��ʼ��
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    //����Ľ��Ϸ�
    if ((Money <= 0) || (MAX_MONEY_VALUE < Money))
    {
        api_print_result(E005);
        return;
    }

    //��Υ���¼��������Ч
    if (stCarInfo.RecordNum == 0)
    {
        api_print_result(E007);
        return;
    }

    //�����Ѿ�����
    if (stCarInfo.WithdrawFlag == true)
    {
        api_print_result(E004);
        return;
    }

    //�������
    TmpMoney = Money;
    opPayFee(&TmpMoney);

    //�ɷ�ʧ��
    if (TmpMoney == Money)
    {
        api_print_result(E006);
        return;
    }

    //����ɷѽ��
    stResult.Fee = 0;

    for (i = 0; i < stCarInfo.RecordNum; i++)
    {
        stResult.Fee = stResult.Fee + stCarInfo.Record[i].fee;
    }

    stResult.ReturnMoney = TmpMoney;
    stResult.PeccancyNo = stCarInfo.RecordNum;
    stResult.Score = stCarInfo.Score;

    api_print_pay_info(&stResult);

    return;
}

void opPayFee(int *pMoney)
{
    int i;

    //�������ѽ���Υ�淣����

    for (i = 0; i < stCarInfo.RecordNum;)
    {
        if (stCarInfo.Record[i].fee <= *pMoney)
        {
            //����һ���ɷѼ�¼
            opPayLargeFeeOnSameDay(pMoney, i);
        }
        else
        {
            i++;
        }
    }

    return;
}

void opPayLargeFeeOnSameDay(int *pMoney, int i)
{
    int j;
    int MaxFeefee, CurFeefee;
    int RecordNum;
    int iTempMoney;
    AutoHistoryNodeTypeInfo *pNode = NULL;

    iTempMoney = *pMoney;

    MaxFeefee = stCarInfo.Record[i].fee;
    RecordNum = i;

    //�����Ƿ�����ͬһ��ļ�¼
    for ( j = i + 1; j < stCarInfo.RecordNum; j++)
    {
        if(stCarInfo.Record[j].Days == stCarInfo.Record[i].Days)
        {
            CurFeefee = stCarInfo.Record[j].fee;
            if((MaxFeefee < CurFeefee) && (CurFeefee <= iTempMoney))
            {
                RecordNum = j;
                MaxFeefee = CurFeefee;
            }
        }
    }

    *pMoney = iTempMoney - stCarInfo.Record[RecordNum].fee;

    //�ɷ�ʧ�ܣ����ٴ���
    if (*pMoney == iTempMoney)
    {
        return;
    }

    //ˢ����ʷ��¼��  --xhj
    pNode = FindNodeByGlobalIndex (g_pAutoHistoryInfoHead, stCarInfo.Record[RecordNum].PeccancyGlobalIndex);
    if (NULL == pNode)
    {
        api_print_result(E999);
        return;
    }
    pNode->data.PayFlag = STAT_HAVE_PAY;

    //����ѽɷѼ�¼
    opDeleteRecord(RecordNum);

    stCarInfo.RecordNum = stCarInfo.RecordNum -1;

    return;
}

void opDeleteRecord(int i)
{
    int j;

    //��δ�ɷѼ�¼ǰ�ƣ�ͬʱ����ѽɷѼ�¼
    for( j = i; j < (stCarInfo.RecordNum - 1); j++)
    {
        memcpy(&stCarInfo.Record[j], &stCarInfo.Record[j+1], sizeof(PeccancyRecord));
    }

    memset(&stCarInfo.Record[stCarInfo.RecordNum -1], 0, sizeof(PeccancyRecord));
}

