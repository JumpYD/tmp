#include <string.h>
#include "stdio.h"
#include "api.h"
#include "car.h"

/*Î¥¹æÀàĞÍ¶ÔÓ¦µÄ·£¿îºÍ¿Û·Ö*/
PeccancyRule Rule[]=
{
    {100,1},    //Î¥¹æÀàĞÍ0
    {200,2},    //Î¥¹æÀàĞÍ1
    {300,3},    //Î¥¹æÀàĞÍ2
    {400,4},    //Î¥¹æÀàĞÍ3
    {500,5},    //Î¥¹æÀàĞÍ4
    {0,0}       //BUTTÖµ
};


CarInfo         stCarInfo;          //³µÁ¾ĞÅÏ¢È«¾Ö¼ÇÂ¼±í
int             CurrentDays;        //µ±Ç°²Ù×÷Ê±¼ä
bool            IsSystemInit=false; //ÏµÍ³³õÊ¼»¯±êÖ¾
int             g_iGlobalIndex = 0; //¼ÇÂ¼Â¼ÈëµÄÎ¥¹æ¼ÇÂ¼ÊıÈ«¾ÖĞòºÅ

AutoHistoryNodeTypeInfo *g_pAutoHistoryInfoHead = NULL;   //³µÁ¾ÀúÊ·ĞÅÏ¢Ö¸ÕëÍ·

void main(int argc, char* argv[])
{
    /* Æô¶¯Socket·şÎñÕìÌı5555¶Ë¿Ú(sapi_server_startº¯ÊıÔÚlib¿âÒÑÊµÏÖ)¡£
     * lib¿âÒÑÊµÏÖ´ÓSocket½ÓÊÕµ½×Ö·û´®ºóµÄÃüÁî·Ö·¢´¦Àí£»
     * ¿¼ÉúÖ»ĞèÒªÊµÏÖ·Ö·¢ºóµÄ¸÷ÃüÁî¼´¿É¡£
    */
    api_server_start(argc, argv);
	// ´Ë´¦²»»áÖ´ĞĞµ½£¬×¢Òâ²»ÒªÔÚ´Ë´¦Ìí¼Ó´úÂë
}

/*****************************************************************************
 º¯ Êı Ãû  : opInit
 ¹¦ÄÜÃèÊö  : ¿¼ÉúĞèÒªÊµÏÖµÄ½Ó¿Ú
             Íê³É¡°ÏµÍ³³õÊ¼»¯¡±²Ù×÷
             ÃüÁîÊµÀı£ºi
 ÊäÈë²ÎÊı  : ÎŞ
 Êä³ö²ÎÊı  : ÎŞ
 ·µ »Ø Öµ  : ÎŞ
 µ÷ÓÃº¯Êı  :
 ±»µ÷º¯Êı  :

 ĞŞ¸ÄÀúÊ·      :
  1.ÈÕ    ÆÚ   : 2010Äê1ÔÂ21ÈÕ
    ×÷    Õß   :
    ĞŞ¸ÄÄÚÈİ   : ĞÂÉú³Éº¯Êı

*****************************************************************************/
void opInit(void)
{
    //»Ö¸´³µÅÆÎ´µõÏú±êÖ¾
    stCarInfo.WithdrawFlag = false;

    //»Ö¸´³µÁ¾»ı·Ö³õÊ¼Öµ
    stCarInfo.Score = MAX_SCORE_VALUE;

    //³µÁ¾Î¥¹æ¼ÇÂ¼ÊıÇå¿Õ
    stCarInfo.RecordNum = 0;

    //Çå¿ÕÀúÊ·Î¥¹æ¼ÇÂ¼
    memset(stCarInfo.Record, 0, (sizeof(PeccancyRecord) * MAX_PECCANCY_RECORD_NUM));

    //µ±Ç°Ê±¼äÇå0
    CurrentDays = 0;

    //ÖÃÏµÍ³³õÊ¼»¯±êÖ¾Î»true
    IsSystemInit = true;

    //Â¼ÈëµÄÈ«¾ÖÎ¥¹æĞòºÅÇå0
    g_iGlobalIndex = 0;

    //Çå¿Õ³µÁ¾ÀúÊ·Î¥¹æĞÅÏ¢¼ÇÂ¼  --xhj
    if (NULL != g_pAutoHistoryInfoHead)
    {
        if (RET_SUCC != RemoveList(g_pAutoHistoryInfoHead))
        {
            api_print_result(E999);
            return;
        }
        g_pAutoHistoryInfoHead = NULL;
    }

    //´´½¨¿ÕÁ´±íÓÃÓÚ´æ´¢ÀúÊ·Î¥¹æĞÅÏ¢
    g_pAutoHistoryInfoHead = CreateList();
    if (NULL == g_pAutoHistoryInfoHead)
    {
        api_print_result(E999);
        return;
    }

    //³õÊ¼»¯³É¹¦
    api_print_result(S001);

    return;
}

/*****************************************************************************
 º¯ Êı Ãû  : opRecord
 ¹¦ÄÜÃèÊö  : ¿¼ÉúĞèÒªÊµÏÖµÄ½Ó¿Ú
             Íê³É¡°Â¼ÈëÎ¥¹æ¼ÇÂ¼¡±²Ù×÷
             ÃüÁîÊµÀı£ºr 0-1
 ÊäÈë²ÎÊı  : int Peccancy  :    Î¥¹æÀàĞÍ
             int Days      :    Ê±¼ä
 Êä³ö²ÎÊı  : ÎŞ
 ·µ »Ø Öµ  : ÎŞ
 µ÷ÓÃº¯Êı  :
 ±»µ÷º¯Êı  :

 ĞŞ¸ÄÀúÊ·      :
  1.ÈÕ    ÆÚ   : 2010Äê1ÔÂ21ÈÕ
    ×÷    Õß   :
    ĞŞ¸ÄÄÚÈİ   : ĞÂÉú³Éº¯Êı

*****************************************************************************/
void opRecord(int PeccancyType, int Days)
{
    QueryResult stOneCarInfo = { 0 };

    //ÏµÍ³ÉĞÎ´³õÊ¼»¯
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    //ÊäÈëµÄÎ¥¹æÀàĞÍ²»ºÏ·¨
    if ((PeccancyType < PECCANCY_TYPE_0) || (PECCANCY_TYPE_4 < PeccancyType))
    {
        api_print_result(E003);
        return;
    }

    //ÊäÈëµÄÊ±¼ä²»ºÏ·¨
    if ((Days <= 0) || (MAX_DAYS_VALUE < Days))
    {
        api_print_result(E002);
        return;
    }

    //ÊäÈëµÄÊ±¼äĞ¡ÓÚµ±Ç°Ê±¼ä
    if (Days < CurrentDays)
    {
        api_print_result(E008);
        return;
    }

    //ÉĞÎ´½É·ÑµÄÎ¥¹æ¼ÇÂ¼ÒÑÂú
    if (MAX_PECCANCY_RECORD_NUM <=  stCarInfo.RecordNum)
    {
        api_print_result(E009);
        return;
    }

    //³µÅÆÒÑ¾­µõÏú
    if (stCarInfo.WithdrawFlag == true)
    {
        api_print_result(E004);
        return;
    }

    //¼ÇÂ¼Î¥¹æĞÅÏ¢µ½³µÁ¾ĞÅÏ¢Í³¼Æ±íÖĞ
    stCarInfo.Record[stCarInfo.RecordNum].PeccancyType = PeccancyType;
    stCarInfo.Record[stCarInfo.RecordNum].Days = Days;
    stCarInfo.Record[stCarInfo.RecordNum].fee = Rule[PeccancyType].Fee;

    //Èç¹û´Ë´ÎÎ¥¹æËù¿Û»ı·Ö´óÓÚÊ£Óà»ı·Ö£¬»ı·ÖÖ±½ÓÇå0£¬ÖÃ³µÅÆµõÏú
    if (Rule[PeccancyType].Score >= stCarInfo.Score)
    {
        stCarInfo.WithdrawFlag = true;
        stCarInfo.Score = 0;
    }
    else
    {
        stCarInfo.Score = stCarInfo.Score - Rule[PeccancyType].Score;
    }

    //Î¥¹æĞòºÅÄ¬ÈÏ´Ó1¿ªÊ¼£¬0ÎªÎŞĞ§
    g_iGlobalIndex++;
    stCarInfo.Record[stCarInfo.RecordNum].PeccancyGlobalIndex = g_iGlobalIndex;

    if (true == stCarInfo.WithdrawFlag)
    {
        //³µÅÆµõÏú£¬Çå¿ÕÀúÊ·ËùÓĞÎ¥¹æ¼ÇÂ¼ĞÅÏ¢ --xhj
        if (RET_SUCC != RemoveList(g_pAutoHistoryInfoHead))
        {
            api_print_result(E999);
            return;
        }

        //Î¥¹æ¼ÇÂ¼ÊıÇå¿Õ
        g_iGlobalIndex = 0;
        g_pAutoHistoryInfoHead = NULL;
    }
    else
    {
        //Ìí¼ÓÎ¥¹æ¼ÇÂ¼µ½ÀúÊ·Î¥¹æĞÅÏ¢ÁĞ±íÖĞ --xhj
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

    //¼ÇÂ¼Î¥¹æ´ÎÊı¼°µ±Ç°Ê±¼ä£
    stCarInfo.RecordNum++;
    CurrentDays = Days;

    api_print_result(S002);

    return;
}

/*****************************************************************************
 º¯ Êı Ãû  : opQuery
 ¹¦ÄÜÃèÊö  : ¿¼ÉúĞèÒªÊµÏÖµÄ½Ó¿Ú
             Íê³É²éÑ¯Î¥¹æ¼ÇÂ¼²Ù×÷
             ÃüÁîÊµÀı£ºq
 ÊäÈë²ÎÊı  : ÎŞ
 Êä³ö²ÎÊı  : ÎŞ
 ·µ »Ø Öµ  : ÎŞ
 µ÷ÓÃº¯Êı  :
 ±»µ÷º¯Êı  :

 ĞŞ¸ÄÀúÊ·      :
  1.ÈÕ    ÆÚ   : 2010Äê1ÔÂ21ÈÕ
    ×÷    Õß   :
    ĞŞ¸ÄÄÚÈİ   : ĞÂÉú³Éº¯Êı

*****************************************************************************/
void opQuery(void)
{
    //ÏµÍ³ÉĞÎ´³õÊ¼»¯
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

    //ÎŞÈÎºÎÎ¥¹æ¼ÇÂ¼
    if ((0 == g_iGlobalIndex) || (NULL == g_pAutoHistoryInfoHead))
    {
        api_print_result (E010);
        return;
    }

    //´òÓ¡ÀúÊ·Î¥¹æĞÅÏ¢µ½socket  --xhj
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
 º¯ Êı Ãû  : opPay
 ¹¦ÄÜÃèÊö  : ¿¼ÉúĞèÒªÊµÏÖµÄ½Ó¿Ú
             Íê³É½ÉÄÉÎ¥¹æ·£¿î²Ù×÷
             ÃüÁîÊµÀı£ºp 100
 ÊäÈë²ÎÊı  : int  Money       :    ½ÉÄÉ½ğ¶î
 Êä³ö²ÎÊı  : ÎŞ
 ·µ »Ø Öµ  : ÎŞ
 µ÷ÓÃº¯Êı  :
 ±»µ÷º¯Êı  :

 ĞŞ¸ÄÀúÊ·      :
  1.ÈÕ    ÆÚ   : 2010Äê1ÔÂ21ÈÕ
    ×÷    Õß   :
    ĞŞ¸ÄÄÚÈİ   : ĞÂÉú³Éº¯Êı

*****************************************************************************/
void opPay(int Money)
{
    PayResult stResult;
    int i;
    int TmpMoney;

    //ÏµÍ³ÉĞÎ´³õÊ¼»¯
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    //ÊäÈëµÄ½ğ¶î²»ºÏ·¨
    if ((Money <= 0) || (MAX_MONEY_VALUE < Money))
    {
        api_print_result(E005);
        return;
    }

    //ÎŞÎ¥¹æ¼ÇÂ¼£¬½ÉÄÉÎŞĞ§
    if (stCarInfo.RecordNum == 0)
    {
        api_print_result(E007);
        return;
    }

    //³µÅÆÒÑ¾­µõÏú
    if (stCarInfo.WithdrawFlag == true)
    {
        api_print_result(E004);
        return;
    }

    //¼ÆËã·ÑÓÃ
    TmpMoney = Money;
    opPayFee(&TmpMoney);

    //½É·ÑÊ§°Ü
    if (TmpMoney == Money)
    {
        api_print_result(E006);
        return;
    }

    //Êä³ö½É·Ñ½á¹û
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

    //±éÀú¸¶·Ñ½ğ¶îºÍÎ¥¹æ·£¿î½ğ¶î

    for (i = 0; i < stCarInfo.RecordNum;)
    {
        if (stCarInfo.Record[i].fee <= *pMoney)
        {
            //´¦ÀíÒ»Ìõ½É·Ñ¼ÇÂ¼
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

    //±éÀúÊÇ·ñÓĞÏàÍ¬Ò»ÌìµÄ¼ÇÂ¼
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

    //½É·ÑÊ§°Ü£¬²»ÔÙ´¦Àí
    if (*pMoney == iTempMoney)
    {
        return;
    }

    //Ë¢ĞÂÀúÊ·¼ÇÂ¼±í  --xhj
    pNode = FindNodeByGlobalIndex (g_pAutoHistoryInfoHead, stCarInfo.Record[RecordNum].PeccancyGlobalIndex);
    if (NULL == pNode)
    {
        api_print_result(E999);
        return;
    }
    pNode->data.PayFlag = STAT_HAVE_PAY;

    //Çå¿ÕÒÑ½É·Ñ¼ÇÂ¼
    opDeleteRecord(RecordNum);

    stCarInfo.RecordNum = stCarInfo.RecordNum -1;

    return;
}

void opDeleteRecord(int i)
{
    int j;

    //°ÑÎ´½É·Ñ¼ÇÂ¼Ç°ÒÆ£¬Í¬Ê±Çå¿ÕÒÑ½É·Ñ¼ÇÂ¼
    for( j = i; j < (stCarInfo.RecordNum - 1); j++)
    {
        memcpy(&stCarInfo.Record[j], &stCarInfo.Record[j+1], sizeof(PeccancyRecord));
    }

    memset(&stCarInfo.Record[stCarInfo.RecordNum -1], 0, sizeof(PeccancyRecord));
}

