#include <stdio.h>
#include <string.h>
#include "api.h"


//车型,时长对应的费率表
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

    //前面已经保证了Out的时间肯定大于In的时间
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

    //前30分钟免费
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

//时间逻辑检查Ta是否是否早于Tb
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
    启动Socket服务侦听5555端口(sapi_server_start函数在lib库已实现)。
    lib库已实现从Socket接收到字符串后的命令分发处理；
    */
    apiServerStart(argc,argv);
    // 此处不会执行到，注意不要在此处添加代码
}

/*****************************************************************************
 函 数 名  : OpReset
 功能描述  : 需要考生实现的接口,系统初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpReset(void)
{
    Exam_InitAutoInfo();

    Exam_InitCardStatus();

    gParkingInfo.IdleParkSpaceNumber = PARKING_SPACE;
    gParkingInfo.bInitial            = TRUE;

    //删除链表
    if (NULL != g_pAutoHistoryInfo)
    {
        if (TRUE != RemoveList(g_pAutoHistoryInfo))
        {
            api_parksys_ret(OP_INTERNAL_ERR);
            return;
        }
        g_pAutoHistoryInfo = NULL;
    }

    //创建链表
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
 函 数 名  : OpGetin
 功能描述  : 需要考生实现的接口,车辆进入停车场
 输入参数  : UL AutoNo 车牌号
             ParkSysAutoType AutoType 车辆类型
             ParkSysTime InTime 车辆进入停车场时间
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpGetin(UL AutoNo, ParkSysAutoType AutoType, ParkSysTime InTime)
{
    UL MinimumIdleCardNo;
    UL TimeLogicFlag;
    //UL CardNo;
    UL ActAutoNo = AutoNo - MIN_AUTONO; //车牌号有偏移

//参数合法性检查
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoNo, MIN_AUTONO, MAX_AUTONO);
    //EXAM_CHK_PRAR_DIGITAL_RETURN(CardNo, 0, CARD_COUNT - 1);
    EXAM_CHK_PRAR_TIME_RETURN(InTime.hh, InTime.mm);
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoType, CAR, BUS);

//逻辑依赖检查
    //有空闲车位才允许车辆进入
    EXAM_TRUE_RETURN(gParkingInfo.IdleParkSpaceNumber < 1, OP_E_PARKING_FULL);

    //同一车牌号的车辆可多次进出停车场,但需出停车场后,才能再次进入停车场
    EXAM_TRUE_RETURN(AUTO_IN_PARKING == gAutoInfo[ActAutoNo].Status, OP_E_EXISTED_ATUO);

    if (gAutoInfo[ActAutoNo].EnterCount > 0)
    {
        //同一辆车重复进入停车场时,车辆类型必须与上一次一样
        if (gAutoInfo[ActAutoNo].AutoType != AutoType)
        {
            api_parksys_ret(OP_E_MISMATCH_AUTO_TYPE);

            //车辆套牌，清空此车辆的所有信息，因前面算法已经实现了同一车牌的车辆必须先出停车场，因此此处不用再释放卡和车位
            //清除此车辆的所有历史停车信息
            if (TRUE != RemoveNodeByAutoNo(g_pAutoHistoryInfo, AutoNo))
            {
                api_parksys_ret(OP_INTERNAL_ERR);
                return;
            }

            return;
        }
    }

    //时间检查,进入的时间>=上次离开的时间
    if (gAutoInfo[ActAutoNo].EnterCount > 0)
    {
        TimeLogicFlag = Exam_TaLessThanTb(InTime, gAutoInfo[ActAutoNo].LastLeaveTime);
        EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);
    }

    //时间检查,当前进或出停车场的时间>=上一次进或出停车场的时间
    if (!gParkingInfo.bInitial)
    {
        TimeLogicFlag = Exam_TaLessThanTb(InTime, gParkingInfo.LastOpTime);
        EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);
    }

    //目前的规格不会出现超过上限的情况
    MinimumIdleCardNo = Exam_GetMinimumCardNo();

    //设置停车信息
    gAutoInfo[ActAutoNo].AutoType   = AutoType;
    gAutoInfo[ActAutoNo].EnterCount++;
    gAutoInfo[ActAutoNo].CardNo     = MinimumIdleCardNo;
    gAutoInfo[ActAutoNo].Status     = AUTO_IN_PARKING;
    gAutoInfo[ActAutoNo].GetInTime  = InTime;

    EXAM_SET_CARD_STATUS(MinimumIdleCardNo, CARD_INUSE);

    gParkingInfo.IdleParkSpaceNumber--;
    gParkingInfo.bInitial   = FALSE;
    gParkingInfo.LastOpTime = InTime;

    //记录到历史信息中
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
 函 数 名  : OpGetout
 功能描述  : 需要考生实现的接口,车辆离开停车场
 输入参数  : UL AutoNo 车牌号
             UL CardNo 卡号
             ParkSysTime InTime 车辆进入停车场时间
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpGetout(UL AutoNo, UL CardNo, ParkSysTime OutTime)
{
    UL fee;
    UL TimeLogicFlag;
    UL ActAutoNo = AutoNo - MIN_AUTONO; //车牌号有偏移

//参数合法性检查
    EXAM_CHK_PRAR_DIGITAL_RETURN(AutoNo, MIN_AUTONO, MAX_AUTONO);
    EXAM_CHK_PRAR_DIGITAL_RETURN(CardNo, 0, CARD_COUNT - 1);
    EXAM_CHK_PRAR_TIME_RETURN(OutTime.hh, OutTime.mm);

//逻辑依赖检查
    //车辆必须先进入停车场后才可以离开
    EXAM_TRUE_RETURN(AUTO_IN_PARKING != gAutoInfo[ActAutoNo].Status, OP_E_NOT_IN_PARKING);

    //车牌号和卡号需要与进入时的信息一致
    EXAM_TRUE_RETURN(CardNo != gAutoInfo[ActAutoNo].CardNo, OP_E_AUTONO_MISMATCH_CARDNO);

    //时间逻辑,进入的时间>=上次离开的时间
    TimeLogicFlag = Exam_TaLessThanTb(OutTime, gAutoInfo[ActAutoNo].GetInTime);
    EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);

    //时间逻辑,当前进或出停车场的时间>=上一次进或出停车场的时间
    if (!gParkingInfo.bInitial)
    {
        TimeLogicFlag = Exam_TaLessThanTb(OutTime, gParkingInfo.LastOpTime);
        EXAM_TRUE_RETURN(TimeLogicFlag, OP_E_TIME_LOGIC);
    }

    //离开停车场设置
    gAutoInfo[ActAutoNo].Status        = AUTO_OUT_PARKING;
    gAutoInfo[ActAutoNo].GetOutTime    = OutTime;
    gAutoInfo[ActAutoNo].LastLeaveTime = OutTime;

    EXAM_SET_CARD_STATUS(CardNo, CARD_IDLE);

    gParkingInfo.IdleParkSpaceNumber++;
    gParkingInfo.bInitial   = FALSE;
    gParkingInfo.LastOpTime = OutTime;

    //费用计算
    fee  = Exam_GetFee(&gAutoInfo[ActAutoNo]);

    //刷新取车时间及费用
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
 函 数 名  : OpList
 功能描述  : 需要考生实现的接口,停车场空闲车位查询
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpListIdleParkingSpaceNumber(void)
{
    api_parksys_idle_parkingspace_number(gParkingInfo.IdleParkSpaceNumber);

    return;
}

/*****************************************************************************
 函 数 名  : OpList
 功能描述  : 需要考生实现的接口,指定卡状态查询
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpListCardStatus(UL CardNo)
{
    //参数合法性检查
    EXAM_CHK_PRAR_DIGITAL_RETURN(CardNo, 0, CARD_COUNT - 1);

    api_parksys_card_status(gCardStatus[CardNo]);

    return;
}

/*****************************************************************************
 函 数 名  : OpList
 功能描述  : 需要考生实现的接口,停车场中指定车辆信息查询
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpListParkingAuto(UL AutoNo)
{
    UL ActAutoNo = AutoNo - MIN_AUTONO; //车牌号有偏移
    ParkingAutoInfo AutoInfo;

    //参数合法性检查
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
 函 数 名  : OpListParkingAutoHistoryInfo
 功能描述  : 需要考生实现的接口,停车场中历史所有车辆停车信息查询
 输入参数  : UL AutoNo 车牌号
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void OpListParkingAutoHistoryInfo(UL AutoNo)
{

    AutoHistoryInfoNode *pNode = NULL;

    //参数合法性检查
    if ((0 != AutoNo) && (AutoNo < MIN_AUTONO) || (AutoNo > MAX_AUTONO))
    {
        api_parksys_ret(OP_E_PARAMETER);
        return;
    }

    if (0 != AutoNo)
    {
        //先查找是否有对应卡号的记录
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
             //先查找是否有对应卡号的记录
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

