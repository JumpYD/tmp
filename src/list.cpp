#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "api.h"

/*************************************************
Function      : CreateList
Description   : 创建链表头节点
Return        : 链表的头指针
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
Description   : 根据卡号，查找某个节点
Input         : pHead 链表的头节点指针
                要查找的卡号
Return        : 正确:返回指定节点的指针
                失败:返回空指针
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
Description   : 向链表中尾部插入某个节点
Input         : pHead        链表的头节点指针
                pCardInfo    消费记录信息
Output        : 无
Return        : 正确:返回TRUE
                失败:返回FALSE
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
Description   : 从链表中删除指定卡号的记录
Input         : pHead       链表的头节点指针
                iCradNo     待删除的节点的卡号
Return        : 正确:返回TRUE
                失败:返回FALSE
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
Description   : 删除整个链表
Input         : pHead 链表的头节点指针
Return        : 正确:TRUE
                失败:FALSE
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

