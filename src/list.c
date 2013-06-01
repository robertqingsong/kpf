#include "../inc/list.h"

//front insert node to list.
int32_t insert_list_head_front( CListNode **ppListHead, CListNode *pNewNode )
{
	int32_t iRetCode = -1;

	if ( ppListHead && pNewNode )
	{
		CListNode *pLHead = *ppListHead;

		pNewNode->Prev = pNewNode->Next = NULL;
		if ( pLHead )
		{
			pNewNode->Next = pLHead;
			pNewNode->Prev = NULL;
			pLHead->Prev = pNewNode;
			*ppListHead = pNewNode;

			iRetCode = 0;
		}
		else
		{
			
			*ppListHead = pNewNode;

			iRetCode = 0;
		}
	}

	return iRetCode;
}

//rear insert node to list.
int32_t insert_list_head_rear( CListNode **ppListHead, CListNode *pNewNode )
{
	int32_t iRetCode = -1;

	if ( ppListHead && pNewNode )
	{
		CListNode *pLHead = *ppListHead;

		pNewNode->Prev = pNewNode->Next = NULL;
		if ( pLHead )
		{
			pNewNode->Next = pLHead->Next;
			if ( pLHead->Next )
				pLHead->Next->Prev = pNewNode;
			
			pNewNode->Prev = pLHead;
			pLHead->Next = pNewNode;

			iRetCode = 0;
		}
		else
		{
			*ppListHead = pNewNode;

			iRetCode = 0;
		}
	}

	return iRetCode;
}

//remove node from list.
CListNode *remove_list_head_node( CListNode **ppListHead, CListNode *pRemoveNode )
{
	CListNode *pRetCode = NULL;

	if ( ppListHead && pRemoveNode )
	{
		CListNode *pLHead = *ppListHead;

		if ( pLHead )
		{
			//list head is valid.
			if ( pLHead == pRemoveNode )
			{
				//delete head node.
				(*ppListHead) = pRemoveNode->Next;
				if ( *ppListHead )
					(*ppListHead)->Prev = NULL;
				
				pRemoveNode->Prev = pRemoveNode->Next = NULL;

				pRetCode = pRemoveNode;
			}
			else
			{
				//delete normal node.
				pRemoveNode->Prev->Next = pRemoveNode->Next;
				if ( pRemoveNode->Next )
					pRemoveNode->Next->Prev = pRemoveNode->Prev;

				pRemoveNode->Prev = pRemoveNode->Next = NULL;

				pRetCode = pRemoveNode;
			}
		}
	}

	return pRetCode;
}

