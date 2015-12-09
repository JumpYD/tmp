#include <string.h>
#include "api.h"
#include "car.h"
#include "malloc.h"

/*************************************************
Function      : CreateList
Description   : 创建链表头节点
Return        : 链表的头指针
*************************************************/
AutoHistoryNodeTypeInfo* CreateList(void)
{
    AutoHistoryNodeTypeInfo *pHead = NULL;

    //申请的头指针
    pHead = (AutoHistoryNodeTypeInfo *)malloc(sizeof(AutoHistoryNodeTypeInfo));

    //判断是否申请成功
    if (NULL == pHead)
    {
        return NULL;
    }

    pHead->data.Index = 0;
    pHead->pNext = NULL;

    return pHead;
}

/*************************************************
Function      : FindNodeByGlobalIndex
Description   : 根据指定参数，查找某个节点
Input         : pHead 链表的头节点指针
                要查找的学生ID
Return        : 正确:返回指定节点的指针
                失败:返回空指针
*************************************************/
AutoHistoryNodeTypeInfo* FindNodeByGlobalIndex(AutoHistoryNodeTypeInfo *pHead, int iGlobalIndex)
{
    AutoHistoryNodeTypeInfo *pNode = NULL;

    if ((NULL == pHead) || (iGlobalIndex < 0))
    {
        return NULL;
    }

    pNode = pHead->pNext;
    while ((NULL != pNode))
    {
        if (pNode->data.Index == iGlobalIndex)
        {
            break;
        }
        pNode = pNode->pNext;
    }

    return pNode;
}


/*************************************************
Function      : Insert2ListTail
Description   : 向链表中尾部插入某个节点
Input         : pHead        链表的头节点指针
                pStudentInfo 学生信息
Return        : 正确:返回头节点指针
                失败:返回空指针
*************************************************/
AutoHistoryNodeTypeInfo* Insert2ListTail(AutoHistoryNodeTypeInfo *pHead, QueryResult *pAutoInfo)
{
    AutoHistoryNodeTypeInfo* pNode = NULL;
    AutoHistoryNodeTypeInfo* pNewNode = NULL;

    if ((NULL == pHead) || (NULL == pAutoInfo))
    {
        return NULL;
    }

    pNode = pHead;
    while (pNode->pNext != NULL)
    {
        pNode = pNode->pNext;
    }

    pNewNode = (AutoHistoryNodeTypeInfo *)malloc(sizeof(AutoHistoryNodeTypeInfo));
    if (NULL == pNewNode)
    {
        return NULL;
    }

    pNode->pNext = pNewNode;
    pNewNode->pNext = NULL;
    memcpy(&(pNewNode->data), pAutoInfo, sizeof(QueryResult));

    return pHead;
}

/*************************************************
Function      : RemoveList
Description   : 删除整个链表
Input         : pHead 链表的头节点指针
Return        : 正确: RET_SUCC
                失败: RET_FAILURE
*************************************************/
int RemoveList(AutoHistoryNodeTypeInfo *pHead)
{
    AutoHistoryNodeTypeInfo *pNode = NULL;
    AutoHistoryNodeTypeInfo *pb = NULL;

    if (NULL == pHead)
    {
        return RET_FAILURE;
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

    return RET_SUCC;
}
