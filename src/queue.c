#include "../inc/queue.h"

//init queue.
int32_t init_queue( CQueue *pQueue )
{
	int32_t iRetCode = -1;

	if ( pQueue )
	{
		memset( pQueue, 0x00, sizeof(*pQueue) );

		iRetCode = init_mutex( &(pQueue->Locker) );
	}

	return iRetCode;
}

//set queue water level.
int32_t set_queue_water_level( CQueue *pQueue, int32_t iMaxQWaterLevel )
{
	int32_t iRetCode = -1;

	if ( pQueue && iMaxQWaterLevel > 0 )
	{

		lock( &(pQueue->Locker) );

		pQueue->iMaxQueueWaterLevel = iMaxQWaterLevel;

		iRetCode = 0;

		unlock( &(pQueue->Locker) );

	}

	return iRetCode;
}

//en queue.
int32_t en_queue( CQueue *pQueue, CQueueNode *pNewQNode )
{
	int32_t iRetCode = -1;

	if ( pQueue && pNewQNode )
	{
		lock( &(pQueue->Locker) );
		
		if ( pQueue->iCurrentQueueWaterLevel < pQueue->iMaxQueueWaterLevel )
		{
			CListNode *pQLNode = NULL;

			pQLNode = &(pNewQNode->LNode);
			pQLNode->Next = pQLNode->Prev = NULL;
			if ( (pQueue->Front == pQueue->Rear) && !(pQueue->Front) )
			{
				//queue is empty.
				pQueue->Front = pQueue->Rear = pNewQNode;
				
				(pQueue->iCurrentQueueWaterLevel)++;

				iRetCode = 0;
			}
			else
			{
				//queue is not empty.
				CListNode *pQHeadLNode = &( pQueue->Rear->LNode );

				iRetCode = insert_list_head_rear( &pQHeadLNode, pQLNode );
				if ( iRetCode >= 0 )
				{
					pQueue->Rear = pNewQNode;
					(pQueue->iCurrentQueueWaterLevel)++;
				}
			}

		}

		unlock( &(pQueue->Locker) );
	}

	return iRetCode;
}

//out queue.
CQueueNode *de_queue( CQueue *pQueue )
{
	CQueueNode *pRetCode = NULL;

	if ( pQueue )
	{
		CListNode *pQLHeadNode = NULL, *pRemoveLNode = NULL, *pOpRet = NULL;

		lock( &( pQueue->Locker ) );
		
		if ( pQueue->Front )
		{
			pQLHeadNode = &( pQueue->Front->LNode );

			pRemoveLNode = &(pQueue->Front->LNode);
			pOpRet = remove_list_head_node( &pQLHeadNode, pRemoveLNode );
			if ( pOpRet )
			{
				//remove list node ok.
				if ( pQLHeadNode )
					pQueue->Front = CONTAINER_OF( pQLHeadNode, LNode, CQueueNode );
				else
					pQueue->Front = pQueue->Rear = NULL;

				pRetCode = CONTAINER_OF( pOpRet, LNode, CQueueNode );

				(pQueue->iCurrentQueueWaterLevel)--;
			}
		}

		unlock( &( pQueue->Locker ) );
	}

	return pRetCode;
}

//reset queue.
int32_t reset_queue( CQueue *pQueue )
{
	int32_t iRetCode = -1;

	if ( pQueue )
	{

		iRetCode = 0;	

	}

	return iRetCode;
}


