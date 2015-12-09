#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "api.h"

/*************************************************
Function      : CreateList
Description   : ��������ͷ�ڵ�
Return        : �����ͷָ��
*************************************************/
AutoHistoryInfoNode* CreateList(void)
{
    AutoHistoryInfoNode *pHead = NULL;

    pHead = (AutoHistoryInfoNode *)malloc(sizeof(AutoHistoryInfoNode));
    if (NULL == pHead)
    {
        return NULL;
    }

    pHead->data.AutoNo = 0;
    pHead->pNext = NULL;

    return pHead;
}

/*************************************************
Function      : FindNodeByCardNo
Description   : ���ݿ��ţ�����ĳ���ڵ�
Input         : pHead �����ͷ�ڵ�ָ��
                Ҫ���ҵĿ���
Return        : ��ȷ:����ָ���ڵ��ָ��
                ʧ��:���ؿ�ָ��
*************************************************/
AutoHistoryInfoNode * FindNodeByCardNo(AutoHistoryInfoNode *pHead, UL uAutoNo)
{
    AutoHistoryInfoNode *pNode = NULL;

    if ((NULL == pHead) || (uAutoNo < MIN_AUTONO) || (uAutoNo > MAX_AUTONO))
    {
        return NULL;
    }

    pNode = pHead->pNext;
    while ((NULL != pNode))
    {
        if (pNode->data.AutoNo == uAutoNo)
        {
            break;
        }
        pNode = pNode->pNext;
    }

    return pNode;
}


/*************************************************
Function      : PushBackNode
Description   : ��������β������ĳ���ڵ�
Input         : pHead        �����ͷ�ڵ�ָ��
                pCardInfo    ���Ѽ�¼��Ϣ
Output        : ��
Return        : ��ȷ:����TRUE
                ʧ��:����FALSE
*************************************************/
int PushBackNode(AutoHistoryInfoNode *pHead, ParkingAutoHistoryInfo *pAutoInfo)
{
    AutoHistoryInfoNode* pNode = NULL;
    AutoHistoryInfoNode* pNewNode = NULL;

    if ((NULL == pHead) || (NULL == pAutoInfo))
    {
        return FALSE;
    }

    pNode = pHead;
    while (pNode->pNext != NULL)
    {
        pNode = pNode->pNext;
    }

    pNewNode = (AutoHistoryInfoNode *)malloc(sizeof(AutoHistoryInfoNode));
    if (NULL == pNewNode)
    {
        return FALSE;
    }

    pNode->pNext = pNewNode;
    pNewNode->pNext = NULL;

    memcpy(&(pNewNode->data), pAutoInfo, sizeof(ParkingAutoHistoryInfo));

    return TRUE;
}

/*************************************************
Function      : RemoveNodeByAutoNo
Description   : ��������ɾ��ָ�����ŵļ�¼
Input         : pHead       �����ͷ�ڵ�ָ��
                iCradNo     ��ɾ���Ľڵ�Ŀ���
Return        : ��ȷ:����TRUE
                ʧ��:����FALSE
*************************************************/
int RemoveNodeByAutoNo(AutoHistoryInfoNode *pHead, UL AutoNo)
{
    AutoHistoryInfoNode* pNode = NULL;
    AutoHistoryInfoNode* pDelNode = NULL;

    if ((NULL == pHead) || (AutoNo < MIN_AUTONO) || (AutoNo > MAX_AUTONO))
    {
        return FALSE;
    }

    pNode = pHead;
    while (NULL != pNode->pNext)
    {
        if (pNode->pNext->data.AutoNo == AutoNo)
        {
            break;
        }

        pNode = pNode->pNext;
    }

    pDelNode = pNode->pNext;
    if (NULL == pDelNode)
    {
        return FALSE;
    }

    pNode->pNext = pDelNode->pNext;
    free(pDelNode);

    pDelNode = NULL;

    return TRUE;
}


/*************************************************
Function      : RemoveList
Description   : ɾ����������
Input         : pHead �����ͷ�ڵ�ָ��
Return        : ��ȷ:TRUE
                ʧ��:FALSE
*************************************************/
int RemoveList(AutoHistoryInfoNode *pHead)
{
    AutoHistoryInfoNode *pNode = NULL;
    AutoHistoryInfoNode *pb = NULL;

    if (NULL == pHead)
    {
        return FALSE;
    }

    pNode = pHead;

    pb = pNode->pNext;
    if (NULL == pb)
    {
        free(pNode);
    }
    else
    {
        while (NULL != pb)
        {
            free(pNode);
            pNode = pb;
            pb = pb->pNext;
        }

        free(pNode);
    }

    pNode = NULL;

    return TRUE;
}

