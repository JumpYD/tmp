/******************************************************************************

                  版权所有 (C), 2009-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : car.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2010年1月21日
  最近修改   :
  功能描述   : car.cpp 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2010年1月21日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

#ifndef __CAR_H__
#define __CAR_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define MAX_PECCANCY_RECORD_NUM           10           //最大违规记录数
#define MAX_SCORE_VALUE                   20           //初始积分
#define MAX_DAYS_VALUE                    1000         //最大时间（天）
#define MAX_MONEY_VALUE                   6000         //最大金额

#define RET_SUCC                          0
#define RET_FAILURE                       -1

/*违规类型*/
enum enPeccancyType
{
    PECCANCY_TYPE_0,            //违章停车
    PECCANCY_TYPE_1,            //超速
    PECCANCY_TYPE_2,            //闯红灯
    PECCANCY_TYPE_3,            //超载
    PECCANCY_TYPE_4,            //醉酒驾车
    PECCANCY_TYPE_BUTT
};

/*违规罚款缴费状态*/
enum enPayStat
{
    STAT_NO_PAY,                //未缴纳罚款
    STAT_HAVE_PAY               //已缴纳罚款
};

typedef struct PECCANCY_RECORD
{
    int PeccancyGlobalIndex;                            //全局违规序号
    int PeccancyType;                                   //违规类型
    int Days;                                           //时间
    int fee;                                            //罚款
}PeccancyRecord;

typedef struct CAR_INFO
{
    bool WithdrawFlag;                                  //车牌吊销标识
    int   Score;                                        //剩余积分
    int   RecordNum;                                    //违规记录数
    PeccancyRecord Record[MAX_PECCANCY_RECORD_NUM];     //违规记录，最多支持10条
}CarInfo;

typedef struct PECCANCY_RULE
{
    int Fee;                                            //罚款
    int Score;                                          //扣分
}PeccancyRule;

typedef struct AutoHistoryNodeType
{
    QueryResult                 data;   //历史信息结构
    struct AutoHistoryNodeType  *pNext;
}AutoHistoryNodeTypeInfo;

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
void opInit(void);

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
void opRecord(int PeccancyType, int Days);


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
void opQuery(void);


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
