#include <string.h>
#include "stdio.h"
#include "api.h"
#include "car.h"

/*违规类型对应的罚款和扣分*/
PeccancyRule Rule[]=
{
    {100,1},    //违规类型0
    {200,2},    //违规类型1
    {300,3},    //违规类型2
    {400,4},    //违规类型3
    {500,5},    //违规类型4
    {0,0}       //BUTT值
};


CarInfo         stCarInfo;          //车辆信息全局记录表
int             CurrentDays;        //当前操作时间
bool            IsSystemInit=false; //系统初始化标志
int             g_iGlobalIndex = 0; //记录录入的违规记录数全局序号

AutoHistoryNodeTypeInfo *g_pAutoHistoryInfoHead = NULL;   //车辆历史信息指针头

void main(int argc, char* argv[])
{
    /* 启动Socket服务侦听5555端口(sapi_server_start函数在lib库已实现)。
     * lib库已实现从Socket接收到字符串后的命令分发处理；
     * 考生只需要实现分发后的各命令即可。
    */
    api_server_start(argc, argv);
	// 此处不会执行到，注意不要在此处添加代码
}

/*****************************************************************************
 函 数 名  : opInit
 功能描述  : 考生需要实现的接口
             完成“系统初始化”操作
             命令实例：i
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年1月21日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void opInit(void)
{
    //恢复车牌未吊销标志
    stCarInfo.WithdrawFlag = false;

    //恢复车辆积分初始值
    stCarInfo.Score = MAX_SCORE_VALUE;

    //车辆违规记录数清空
    stCarInfo.RecordNum = 0;

    //清空历史违规记录
    memset(stCarInfo.Record, 0, (sizeof(PeccancyRecord) * MAX_PECCANCY_RECORD_NUM));

    //当前时间清0
    CurrentDays = 0;

    //置系统初始化标志位true
    IsSystemInit = true;

    //录入的全局违规序号清0
    g_iGlobalIndex = 0;

    //清空车辆历史违规信息记录  --xhj
    if (NULL != g_pAutoHistoryInfoHead)
    {
        if (RET_SUCC != RemoveList(g_pAutoHistoryInfoHead))
        {
            api_print_result(E999);
            return;
        }
        g_pAutoHistoryInfoHead = NULL;
    }

    //创建空链表用于存储历史违规信息
    g_pAutoHistoryInfoHead = CreateList();
    if (NULL == g_pAutoHistoryInfoHead)
    {
        api_print_result(E999);
        return;
    }

    //初始化成功
    api_print_result(S001);

    return;
}

/*****************************************************************************
 函 数 名  : opRecord
 功能描述  : 考生需要实现的接口
             完成“录入违规记录”操作
             命令实例：r 0-1
 输入参数  : int Peccancy  :    违规类型
             int Days      :    时间
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年1月21日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void opRecord(int PeccancyType, int Days)
{
    QueryResult stOneCarInfo = { 0 };

    //系统尚未初始化
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    //输入的违规类型不合法
    if ((PeccancyType < PECCANCY_TYPE_0) || (PECCANCY_TYPE_4 < PeccancyType))
    {
        api_print_result(E003);
        return;
    }

    //输入的时间不合法
    if ((Days <= 0) || (MAX_DAYS_VALUE < Days))
    {
        api_print_result(E002);
        return;
    }

    //输入的时间小于当前时间
    if (Days < CurrentDays)
    {
        api_print_result(E008);
        return;
    }

    //尚未缴费的违规记录已满
    if (MAX_PECCANCY_RECORD_NUM <=  stCarInfo.RecordNum)
    {
        api_print_result(E009);
        return;
    }

    //车牌已经吊销
    if (stCarInfo.WithdrawFlag == true)
    {
        api_print_result(E004);
        return;
    }

    //记录违规信息到车辆信息统计表中
    stCarInfo.Record[stCarInfo.RecordNum].PeccancyType = PeccancyType;
    stCarInfo.Record[stCarInfo.RecordNum].Days = Days;
    stCarInfo.Record[stCarInfo.RecordNum].fee = Rule[PeccancyType].Fee;

    //如果此次违规所扣积分大于剩余积分，积分直接清0，置车牌吊销
    if (Rule[PeccancyType].Score >= stCarInfo.Score)
    {
        stCarInfo.WithdrawFlag = true;
        stCarInfo.Score = 0;
    }
    else
    {
        stCarInfo.Score = stCarInfo.Score - Rule[PeccancyType].Score;
    }

    //违规序号默认从1开始，0为无效
    g_iGlobalIndex++;
    stCarInfo.Record[stCarInfo.RecordNum].PeccancyGlobalIndex = g_iGlobalIndex;

    if (true == stCarInfo.WithdrawFlag)
    {
        //车牌吊销，清空历史所有违规记录信息 --xhj
        if (RET_SUCC != RemoveList(g_pAutoHistoryInfoHead))
        {
            api_print_result(E999);
            return;
        }

        //违规记录数清空
        g_iGlobalIndex = 0;
        g_pAutoHistoryInfoHead = NULL;
    }
    else
    {
        //添加违规记录到历史违规信息列表中 --xhj
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

    //记录违规次数及当前时间�
    stCarInfo.RecordNum++;
    CurrentDays = Days;

    api_print_result(S002);

    return;
}

/*****************************************************************************
 函 数 名  : opQuery
 功能描述  : 考生需要实现的接口
             完成查询违规记录操作
             命令实例：q
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年1月21日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void opQuery(void)
{
    //系统尚未初始化
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

    //无任何违规记录
    if ((0 == g_iGlobalIndex) || (NULL == g_pAutoHistoryInfoHead))
    {
        api_print_result (E010);
        return;
    }

    //打印历史违规信息到socket  --xhj
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
 函 数 名  : opPay
 功能描述  : 考生需要实现的接口
             完成缴纳违规罚款操作
             命令实例：p 100
 输入参数  : int  Money       :    缴纳金额
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年1月21日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void opPay(int Money)
{
    PayResult stResult;
    int i;
    int TmpMoney;

    //系统尚未初始化
    if (IsSystemInit == false)
    {
        api_print_result(E001);
        return;
    }

    //输入的金额不合法
    if ((Money <= 0) || (MAX_MONEY_VALUE < Money))
    {
        api_print_result(E005);
        return;
    }

    //无违规记录，缴纳无效
    if (stCarInfo.RecordNum == 0)
    {
        api_print_result(E007);
        return;
    }

    //车牌已经吊销
    if (stCarInfo.WithdrawFlag == true)
    {
        api_print_result(E004);
        return;
    }

    //计算费用
    TmpMoney = Money;
    opPayFee(&TmpMoney);

    //缴费失败
    if (TmpMoney == Money)
    {
        api_print_result(E006);
        return;
    }

    //输出缴费结果
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

    //遍历付费金额和违规罚款金额

    for (i = 0; i < stCarInfo.RecordNum;)
    {
        if (stCarInfo.Record[i].fee <= *pMoney)
        {
            //处理一条缴费记录
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

    //遍历是否有相同一天的记录
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

    //缴费失败，不再处理
    if (*pMoney == iTempMoney)
    {
        return;
    }

    //刷新历史记录表  --xhj
    pNode = FindNodeByGlobalIndex (g_pAutoHistoryInfoHead, stCarInfo.Record[RecordNum].PeccancyGlobalIndex);
    if (NULL == pNode)
    {
        api_print_result(E999);
        return;
    }
    pNode->data.PayFlag = STAT_HAVE_PAY;

    //清空已缴费记录
    opDeleteRecord(RecordNum);

    stCarInfo.RecordNum = stCarInfo.RecordNum -1;

    return;
}

void opDeleteRecord(int i)
{
    int j;

    //把未缴费记录前移，同时清空已缴费记录
    for( j = i; j < (stCarInfo.RecordNum - 1); j++)
    {
        memcpy(&stCarInfo.Record[j], &stCarInfo.Record[j+1], sizeof(PeccancyRecord));
    }

    memset(&stCarInfo.Record[stCarInfo.RecordNum -1], 0, sizeof(PeccancyRecord));
}

