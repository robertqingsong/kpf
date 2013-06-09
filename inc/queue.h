#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "list.h"

#include "lock.h"

#include "defines.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//queue node type define.
typedef struct CQueueNode_t
{
	

	CListNode LNode;
}CQueueNode;

//queue type define.
typedef struct CQueue_t
{
	CQueueNode *Front;
	CQueueNode *Rear;

	int32_t iMaxQueueWaterLevel;//max queue node size.
	int32_t iCurrentQueueWaterLevel;//Current queue node size.

	CMutex Locker;
}CQueue;

#define CONTAINER_OF_QUEUE( addr, type ) \
				CONTAINER_OF( addr, QNode, type )

//init queue.
int32_t init_queue( CQueue *pQueue );

//set queue water level.
int32_t set_queue_water_level( CQueue *pQueue, int32_t iMaxQWaterLevel );

//en queue.
int32_t en_queue( CQueue *pQueue, CQueueNode *pNewQNode );

//out queue.
CQueueNode *de_queue( CQueue *pQueue );

//is queue empty: 1 empty, 0 not empty.
int32_t is_queue_empty( CQueue *pQueue );

//reset queue.
int32_t reset_queue( CQueue *pQueue );

#if defined(__cplusplus)
}
#endif

#endif

