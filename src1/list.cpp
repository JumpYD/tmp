#include <string.h>
#include "api.h"
#include "car.h"
#include "malloc.h"

/*************************************************
Function      : CreateList
Description   : ��������ͷ�ڵ�
Return        : �����ͷָ��
*************************************************/
AutoHistoryNodeTypeInfo* CreateList(void)
{
    AutoHistoryNodeTypeInfo *pHead = NULL;

    //�����ͷָ��
    pHead = (AutoHistoryNodeTypeInfo *)malloc(sizeof(AutoHistoryNodeTypeInfo));

    //�ж��Ƿ�����ɹ�
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
Description   : ����ָ������������ĳ���ڵ�
Input         : pHead �����ͷ�ڵ�ָ��
                Ҫ���ҵ�ѧ��ID
Return        : ��ȷ:����ָ���ڵ��ָ��
                ʧ��:���ؿ�ָ��
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
Description   : ��������β������ĳ���ڵ�
Input         : pHead        �����ͷ�ڵ�ָ��
                pStudentInfo ѧ����Ϣ
Return        : ��ȷ:����ͷ�ڵ�ָ��
                ʧ��:���ؿ�ָ��
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
Description   : ɾ����������
Input         : pHead �����ͷ�ڵ�ָ��
Return        : ��ȷ: RET_SUCC
                ʧ��: RET_FAILURE
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
