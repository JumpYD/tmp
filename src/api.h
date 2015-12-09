/******************************************************************************

                  版权所有 (C), 2009-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : api.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2009年9月9日
  最近修改   :
  功能描述   : api.cpp 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

#ifndef __API_H__
#define __API_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define CARD_COUNT   5       //卡的数量

#define MIN_AUTONO   10      //最小车牌号
#define MAX_AUTONO   20      //最大车牌号

typedef unsigned char UC;
typedef unsigned long UL;

//汽车类型
typedef enum
{
    CAR,     //小轿车
    BUS,     //大巴
    AUTO_TYPE_BUTT,
}ParkSysAutoType;

//卡状态
typedef enum
{
    CARD_IDLE  = 0,  //卡空闲
    CARD_INUSE = 1,  //卡已经使用
}CardStatus;

//操作码定义
typedef enum
{
    OP_E_CMD                    = 0,  //(考生不使用)无效的命令
    OP_RESET_SUCC               = 1,  //系统初始化成功
    OP_E_PARAMETER              = 2,  //参数不合法
    OP_E_PARKING_FULL           = 3,  //车位已满
    OP_E_EXISTED_ATUO           = 4,  //该车已经在停车场
    OP_E_MISMATCH_AUTO_TYPE     = 5,  //可能是套牌车不允许进入
    OP_E_NOT_IN_PARKING         = 6,  //该车不在停车场
    OP_E_AUTONO_MISMATCH_CARDNO = 7,  //车牌号和卡号不对应不允许离开
    OP_E_TIME_LOGIC             = 8,  //时间逻辑错误
    OP_PARKING_SUCC             = 9,  //停车操作成功
    OP_SPEC_CAR_PARKING         = 10, //指定的车辆在停车场内尚未停过
    OP_ANY_CAR_PAKING           = 11, //停车场内尚未有任何车辆停过
    OP_INTERNAL_ERR             =999, //内部系统错误
}ParkSysOpRet;

//时间
typedef struct
{
    UC hh;  //小时
    UC mm;  //分钟
}ParkSysTime;

//在停车场中的车辆信息
typedef struct
{
    UL              AutoNo;       //车牌号
    ParkSysTime     CurGetInTime; //当前进入时间
    UL              CardNo;       //卡号
    ParkSysAutoType AutoType;     //车型
    UC              ParkingCount; //停车次数
}ParkingAutoInfo;

//停车场中车辆历史停车信息
typedef struct
{
    UL              AutoNo;       //车牌号
    ParkSysAutoType AutoType;     //车辆类型
    UL              CardNo;       //卡号
    ParkSysTime     GetInTime;    //停车时间
    ParkSysTime     GetOutTime;   //取车时间
    UL              Fee;          //停车费用
}ParkingAutoHistoryInfo;


#define PARKING_SPACE 5

#define AUTO_COUNT   ((MAX_AUTONO) - (MIN_AUTONO) + 1)

#define TRUE  1
#define FALSE 0

#define MAX_HOURS          24
#define MINUTES_OF_ONEHOUR 60
#define MINUTES_OF_FREE    30     //前30分钟免费

#define FIRST_DISCOUNT_RATE   100 //第一次不打折
#define SECOND_DISCOUNT_RATE  80  //第二次及以后8折

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

//费率类型
typedef enum _BasicFeeRate
{
    LOW_FIX_FEE,   //前2小时固定费率类型
    STEP_FEE,      //中间每小时步长值类型
    HIGH_FIX_FEE,  //超过12小时固定费率类型
    BASI_FEE_BUTT,
}BasicFeeRate;


//车辆状态
typedef enum _AutoStatus
{
    AUTO_IN_PARKING,  //在停车场
    AUTO_OUT_PARKING, //不在停车场
    AUTO_NO_LICENSE,  //车牌执照吊销
}AutoStatus;

//车辆信息
typedef struct _AutoInfo
{
    UL              AutoNo;        //车牌号
    ParkSysAutoType AutoType;      //车辆类型
    UL              CardNo;        //卡号
    UC              EnterCount;    //进入次数
    AutoStatus      Status;        //车辆状态
    ParkSysTime     GetInTime;     //进入时间
    ParkSysTime     GetOutTime;    //离开时间
    ParkSysTime     LastLeaveTime; //最后一次离开时间
}AutoInfo;


//停车场信息
typedef struct _ParkingInfo
{
    UL           IdleParkSpaceNumber; //空闲车位数
    UC           bInitial;            //停车场是否初时状态置为TRUE,有车辆成功进出后在FALSE
                                      //在根据LastOpTime判断后续的操作时间是否是顺序的
    ParkSysTime  LastOpTime;          //最后一次操作时间
}ParkingInfo;

//历史停车信息链表结构
typedef struct tagAutoHistoryInfoNode
{
    ParkingAutoHistoryInfo          data;  //历史停车信息
    struct tagAutoHistoryInfoNode   *pNext;
}AutoHistoryInfoNode;

AutoHistoryInfoNode * CreateList(void);
AutoHistoryInfoNode * FindNodeByCardNo(AutoHistoryInfoNode *pHead, UL uAutoNo);
int PushBackNode(AutoHistoryInfoNode *pHead, ParkingAutoHistoryInfo *pAutoInfo);
int RemoveNodeByAutoNo(AutoHistoryInfoNode *pHead, UL AutoNo);
int RemoveList(AutoHistoryInfoNode *pHead);


/*****************************************************************************
 函 数 名  : api_parksys_ret
 功能描述  : 系统提供的函数,供考生调用,打印操作结果信息
 输入参数  : ParkSysOpRet OpRet操作码定义
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void api_parksys_ret(ParkSysOpRet OpRet);

/*****************************************************************************
 函 数 名  : api_parksys_fee
 功能描述  : 系统提供的函数,供考生调用,车辆离开停车场后收取的费用
 输入参数  : UL fee当次停车收取的费用
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void api_parksys_fee(UL fee);

/*****************************************************************************
 函 数 名  : api_parksys_idle_parkingspace_number
 功能描述  : 系统提供的函数,供考生调用,停车场空闲车位查询结果
 输入参数  : UL number空闲车位数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void api_parksys_idle_parkingspace_number(UL number);

/*****************************************************************************
 函 数 名  : api_parksys_card_status
 功能描述  : 系统提供的函数,供考生调用,指定卡状态查询结果
 输入参数  : CardStatus cardStatus指定卡状态
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void api_parksys_card_status(CardStatus cardStatus);

/*****************************************************************************
 函 数 名  : api_parksys_parking_auto
 功能描述  : 系统提供的函数,供考生调用,停车场中指定车辆信息查询结果
 输入参数  : ParkingAutoInfo *pAutoInfo停车场中指定车辆信息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void api_parksys_parking_auto(ParkingAutoInfo *pAutoInfo);

/*****************************************************************************
 函 数 名  : api_parksys_parking_auto_history_info
 功能描述  : 系统提供的函数,供考生调用,停车场中指定车辆信息查询结果
 输入参数  : ParkingAutoHistoryInfo *pAutoHistoryInfo停车场中历史车辆停车信息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年9月9日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
void api_parksys_parking_auto_history_info(ParkingAutoHistoryInfo *pAutoHistoryInfo);


/*****************************************************************************
 函 数 名  : api_server_start
 功能描述  : 启动socket服务端，以阻塞的方式接受用户输入
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
void apiServerStart(int argc, char* argv[]);


// 供考生实现的函数

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
