/******************************************************************************

                  版权所有 (C), 2009-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : api.h
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 2011年8月18日
  最近修改   :
  功能描述   : api.cpp 的头文件
******************************************************************************/

#ifndef __HOSPITAL_H__
#define __HOSPITAL_H__


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */
// 贷款编号的边界值
#define LOANID_MIN  1
#define LOANID_MAX  10

// 月收入的边界值
// 低收入的最小值
#define  INCOME_LOW_MIN   1000
// 低等收入的最大值
#define  INCOME_LOW_MAX   5000
// 高等收入的最小值
#define  INCOME_HIGH_MIN  5001
// 高等收入的最大值
#define  INCOME_HIGH_MAX   10000

// 贷款年限的边界值
// 最小值
#define  INSTALLMENTS_MIN   12
// 低收入人群年限的最大值
#define  INSTALLMENTS_LOW_MAX   120
// 低收入人群年限的最大值
#define  INSTALLMENTS_HIGH_MAX   240
// 年限的利率折扣拐点
#define  INSTALLMENTS_RATE_DISCOUNT   120

// 贷款金额的边界值
// 最小值
#define  AMOUNT_MIN   100000
// 低收入人群金额的最大值
#define  AMOUNT_LOW_MAX   500000
// 低收入人群金额的最大值
#define  AMOUNT_HIGH_MAX   1000000
// 贷款折扣的拐点
#define  AMOUNT_RATE_DISCOUNT   400000

// 申请时间的边界值
#define  APPLY_TIME_MIN   1
#define  APPLY_TIME_MAX   10

// 查询时间的边界值
#define  QUERY_TIME_MIN   1
#define  QUERY_TIME_MAX   24

// 年利率的种类
#define  LOAN_RATE_TYPE_0_24   0.024
#define  LOAN_RATE_TYPE_0_48   0.048

// 利率的折扣类型
#define  LOAD_RATE_DISCOUNT_TYPE_0_5   0.5

// 银行的贷款限额
#define  BANK_LOAN_AMOUNT_LIMIT   3000000
// 银行能够发生贷款的份数限制
#define  BANK_LOAN_COUNT_LIMIT   5

/* -----------------------
供考生自定义实现的函数
------------------------ */

/* 系统初始化命令 */
void CmdReset();

/* 贷款申请命令 */
void ReqeustLoan(int loanIdx, float salary, int principal, int years, int reqTime);

/* 查询 */
void CmdLst(int loanIdx, int month, int fund, int lstTime);

void SysRun(int inTime, bool bQry);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __HOSPITAL_H__ */