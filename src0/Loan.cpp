#include <stdio.h>
#include <string.h>
#include "api.h"
#include "Loan.h"
#include <math.h>


void main(int argc, char* argv[])
{
    CmdReset(); 
    /* 
    ����Socket��������5555�˿�(sapi_server_start������lib����ʵ��)��
    lib����ʵ�ִ�Socket���յ��ַ����������ַ�����
    */
    api_server_start(argc, argv);
    // �˴�����ִ�е���ע�ⲻҪ�ڴ˴���Ӵ���
}

/* -----------------------
�������Զ���ʵ�ֵĺ���
------------------------ */

LoanInfo g_loanInfo[LOANID_MAX];
LoanInfo g_loanInfoQry[LOANID_MAX];
int      g_sysTime;
BankInfo g_bankInfo;
BankInfo g_bankInfoQry;
int      g_loanNum;  //����˳����ˮ��

/*****************************************************************************
 �� �� ��  : CmdReset
 ��������  : ��������ʵ�֣�ʵ��ϵͳ��ʼ��
*****************************************************************************/
void CmdReset()
{
    g_sysTime                   = 0;
    g_bankInfo.grantMoney       = 0;
    g_bankInfo.grantNum         = 0;
    g_bankInfoQry.grantMoney    = 0;
    g_bankInfoQry.grantNum      = 0;    
    g_loanNum                   = 0;
    
    memset(g_loanInfo, 0, sizeof(LoanInfo) * LOANID_MAX);
    memset(g_loanInfoQry, 0, sizeof(LoanInfo) * LOANID_MAX);

    api_loansys_ret(OP_E_INIT_SUCCESS);
    
    return;
}

/*****************************************************************************
 �� �� ��  : ReqeustLoan
 ��������  : ��������ʵ�֣�ʵ�ִ�����������
 �������  : loanIdx:������
             salary:������������
             principal:�����ܶ�
             reqTime:����ʱ��
*****************************************************************************/
void ReqeustLoan(int loanIdx, float salary, int principal, int years, int reqTime)
{
    int         i               = 0;
    int         idx             = 0xff;     //��ʹ�õĶ����±�
    int         rate            = 0;
    double      tempMoney       = 0;        //��������м���������ڴ�����������
    int         tempMoneyInt    = 0;
    LoanInfo    iniloanInfo[LOANID_MAX];    //�����ֳ�����ʧ�ܻع�
    
    if ((LOANID_MIN > loanIdx) || (LOANID_MAX < loanIdx))
    {
        api_loansys_ret(OP_E_IDX);
        return;
    }
    
    if ((INCOME_LOW_MIN > salary) || (INCOME_HIGH_MAX < salary))
    {
        api_loansys_ret(OP_E_SALARY);
        return;
    }
    
    if ((10 > principal) || (100 < principal))
    {
        api_loansys_ret(OP_E_PRINCIPAL);
        return;
    }
    if ((1 > years) || (20 < years))
    {
        api_loansys_ret(OP_E_YEAR);
        return;
    }
    
    if ((1 > reqTime) || (10 < reqTime))
    {
        api_loansys_ret(OP_E_APPLY_TIME);
        return;
    }
    
    if (reqTime < g_sysTime)
    {
        api_loansys_ret(OP_E_APPLYTIME_SUB);
        return;
    }
    
    memcpy(&iniloanInfo, &g_loanInfo, sizeof(LoanInfo) * LOANID_MAX);

    //����ʱ�����
    SysRun(reqTime,false);
    
    for ( i = 0 ; i < LOANID_MAX ; i++ )
    {
        if ( g_loanInfo[i].loanIdx == loanIdx )
        {
            if ( WAIT_CHECK == g_loanInfo[i].status
                || WAIT_GRANT == g_loanInfo[i].status
                || SUCC_GRANT == g_loanInfo[i].status)
            {
                //g_sysTime = reqTime;
                //�ֳ��ع�
                memcpy(&g_loanInfo, &iniloanInfo, sizeof(LoanInfo) * LOANID_MAX);
                api_loansys_ret(OP_E_MODIFY);
                return;
            }
            else
            {
                idx = i;
                break;
            }
        }
        
        if ((0 == g_loanInfo[i].loanIdx) && (idx == 0xff))  //δʹ�ù��Ķ���
        {
            idx = i;
        }
    }

    if ( idx == 0xff )
    {
        return;
    }    

    rate = ( 1 <= years && 10 >= years ) ? 24 : 48;
    rate = ( ( 10 <= principal ) && ( 40 >= principal ) ) ? rate : ( rate / 2 );
    rate = rate / 12; //ת����������
    
    g_loanNum++;
    g_loanInfo[idx].loanIdx     = loanIdx;
    g_loanInfo[idx].status      = WAIT_CHECK;
    g_loanInfo[idx].loanNo      = g_loanNum;
    g_loanInfo[idx].principal   = principal;
    g_loanInfo[idx].monthAll    = years * 12;
    g_loanInfo[idx].rate        = rate;
    
    //����� + �����*ʵ��������*����������+1��/2
    tempMoney       = principal * 10000 * ((double)rate / 1000) * (years * 12 + 1) / 2;
    tempMoney       += (double)principal * 10000 ; 
    tempMoneyInt    = (int)( tempMoney * 10 ) % 10;    
    g_loanInfo[idx].allMoney    = (int)( ( tempMoneyInt >= 5 ) ? ceil(tempMoney) : floor(tempMoney) );
        
    g_loanInfo[idx].salary      = salary;

    g_sysTime                   = reqTime;
    g_loanInfo[idx].reqTime     = reqTime;

    api_loansys_ret(OP_E_SUCCESS);
    return;
}

/*****************************************************************************
 �� �� ��  : CmdLst
 ��������  : ��������ʵ�֣�ʵ�ִ�����Ϣ��ѯ
 �������  : loanIdx:������
             month:��ѯ���·�
             fund:�����𻹿��
             lstTime:��ѯʱ��
*****************************************************************************/
void CmdLst(int loanIdx, int month, int fund, int lstTime)
{
    int         i;
    int         findIdx         = 0xff; //��ǰ�����±�������
    double      monthPrincipal  = 0;    //ÿ�»����
    double      tempMoney       = 0;    //��������м���������ڴ�����������
    int         tempMoneyInt    = 0;
    LoanInfo    outPutInfo;
    
    if ( ( LOANID_MIN > loanIdx )
        || ( LOANID_MAX < loanIdx ) )
    {
        api_loansys_ret(OP_E_IDX);
        return;
    }
    
    //�������ڲ�ѯģ���������
    memcpy(&g_loanInfoQry, &g_loanInfo, sizeof(LoanInfo) * 10);
    memcpy(&g_bankInfoQry, &g_bankInfo, sizeof(BankInfo));
    
    for ( i = 0 ; i < LOANID_MAX ; i++ )
    {
        if (g_loanInfoQry[i].loanIdx == loanIdx)
        {
            findIdx = i;
            break;
        }
    }
    
    if ( 0xFF == findIdx)
    {
        api_loansys_ret(OP_E_NO_VALIDAPPLY);
        return;
    }
    
    if ( ( 1 > month )
        || ( 240 < month )
        || ( month > g_loanInfoQry[findIdx].monthAll ) )
    {
        api_loansys_ret(OP_E_MONTH);
        return;
    }

    if ( ( 0 > fund )
        || ( 100 < fund )
        || ( fund > g_loanInfoQry[findIdx].principal ) )
    {
        api_loansys_ret(OP_E_FUND_MONEY);
        return;
    }

    if ( ( 1 > lstTime )
        || ( 24 < lstTime ) )
    {
        api_loansys_ret(OP_E_QRY_TIME);
        return;
    }
    
    if ( ( lstTime < g_loanInfoQry[findIdx].reqTime ) )
    {
        api_loansys_ret(OP_E_QRYTIME_SUB);
        return;
    }

    //����ʱ�����
    SysRun(lstTime, true);

    g_loanInfoQry[findIdx].monthCur = month;

    //����������ֵ
    monthPrincipal  = ( (double)g_loanInfoQry[findIdx].principal * 10000 ) / g_loanInfoQry[findIdx].monthAll;    
    tempMoney
        = monthPrincipal + ( monthPrincipal * ( g_loanInfoQry[findIdx].monthAll - month + 1 ) * g_loanInfoQry[findIdx].rate / 1000 );

    tempMoneyInt    = (int)( tempMoney * 10 ) % 10;    
    g_loanInfoQry[findIdx].monthCurMoney    = (int) ( ( tempMoneyInt >= 5 ) ? ceil(tempMoney) : floor(tempMoney) );    
    g_loanInfoQry[findIdx].ownMoney         = g_loanInfoQry[findIdx].allMoney - ( fund * 10000 / 2 );

    memcpy(&outPutInfo, &g_loanInfoQry[findIdx], sizeof(LoanInfo));
   
    if ( lstTime <= outPutInfo.checkTime )
    {
        outPutInfo.status = WAIT_CHECK;
    }
    else if ( lstTime <= outPutInfo.grantTime )
    {
        outPutInfo.status = WAIT_GRANT;
    }   
    
    if ( SUCC_GRANT != outPutInfo.status )
    {
        outPutInfo.rate             = 0;        
        outPutInfo.allMoney         = 0;
        outPutInfo.monthCurMoney    = 0;
        outPutInfo.ownMoney         = 0;
    }

    api_loansys_loaninfo(outPutInfo);

    return;
}

/*****************************************************************************
 �� �� ��  : SysRun
 ��������  : ϵͳʱ������¼��Զ�����
*****************************************************************************/
void SysRun(int inTime, bool bQry)
{
    int         i               = 0;
    int         j               = 0;
    int         idx             = 0xff; //��ǰ��˴�����
    int         timePass        = 0;
    LoanInfo    *pLoanInfo      = g_loanInfo;
    LoanInfo    *pLoanInfoCur   = 0;
    LoanInfo    *pLoanInfoIdx   = 0;
    BankInfo    *pBankInfoCur   = &g_bankInfo;
    int         curTime         = g_sysTime;

    if ( bQry )
    {
        pLoanInfo       = g_loanInfoQry;
        pBankInfoCur    = &g_bankInfoQry;
    }

    timePass = inTime - curTime;
    for ( i = 0 ; i < timePass ; i++ )
    {
        idx = 0xff; //��ʼ�����
        for ( j = 0 ; j < LOANID_MAX ; j++ )
        {
            //������
            pLoanInfoCur = pLoanInfo + j;
            if ( WAIT_GRANT == pLoanInfoCur->status )
            {
                if ( BANK_LOAN_COUNT_LIMIT <= pBankInfoCur->grantNum)
                {
                    pLoanInfoCur->status = RE_LOAN;
                    pLoanInfoCur->reason = SYS_DISABLE;
                    continue;                    
                }
                
                if ( ( BANK_LOAN_AMOUNT_LIMIT / 10000 ) < ( pBankInfoCur->grantMoney + pLoanInfoCur->principal ) )
                {
                    pLoanInfoCur->status = RE_LOAN;
                    pLoanInfoCur->reason = BANK_NOMOMEY;
                    continue;
                }

                pLoanInfoCur->status        = SUCC_GRANT;
                pBankInfoCur->grantMoney    += pLoanInfoCur->principal;
                pBankInfoCur->grantNum++;                
                pLoanInfoCur->grantTime     = curTime;
            }

            //��������  ����ֻ��ȡһ�������˵��±�(���ڶ�������˵ȴ����������)��jѭ���⴦����˵�����
            if ( WAIT_CHECK == pLoanInfoCur->status )
            {
                if ( 0xff == idx )
                {
                    idx = j;
                }
                else
                {
                    pLoanInfoIdx    = pLoanInfo + idx;
                    //ȡ�ȴ���ˮ��С��
                    idx             = ( pLoanInfoCur->loanNo < pLoanInfoIdx->loanNo ) ? j : idx;       
                }
            }
        }    

        //�ѻ�ȡ�����ˣ����������
        if ( 0xff != idx )
        {
            pLoanInfoIdx = pLoanInfo + idx;
            //������
            if ( ( 1000 <= pLoanInfoIdx->salary )
                && ( 5000 >= pLoanInfoIdx->salary ) )
            {
                if ( 50 < pLoanInfoIdx->principal )
                {
                    pLoanInfoIdx->status = RE_LOAN;
                    pLoanInfoIdx->reason = OVER_PRINCIPAL;
                    continue;
                }
                
                if ( 120 < pLoanInfoIdx->monthAll )
                {
                    pLoanInfoIdx->status = RE_LOAN;
                    pLoanInfoIdx->reason = OVER_YEAR;
                    continue;
                }                
            }
            
            pLoanInfoIdx->status = WAIT_GRANT;
           
            pLoanInfoIdx->checkTime = curTime;                
        }
        
        curTime++;
    }    
}

