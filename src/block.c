#include "../inc/block.h"

#include "../inc/lock.h"

#include "../inc/list.h"

#include "../inc/queue.h"

#include "../inc/timer.h"

#include "../inc/mem_api.h"

//block type define.
typedef struct CBlock_t
{
	
	int32_t iBlockTimeInSeconds;//block time in second.
	int32_t iCurrentTimeInMileSeconds;
	
	int32_t iBlockFlag;//block flag. 0 no block, 1 blocking.

	int32_t iIsExiting;//is exiting.	
	
	CListNode LNode;//list node.
}CBlock;

//block manager define.
typedef struct CBlockManager_t
{
	int32_t iInitFlag;
	
	CListNode *pBlockLHead;//block list head node.
	
	CQueue stRemoveQ;//remove block q.
	
	int32u_t iBlockTimeOutTimerId;//block time out timer id.100ms
	
	CMutex Locker;
}CBlockManager;

static CBlockManager fg_BlockManager = {
	0, 	
};

static int32_t block_timer_callback( int32u_t iTimerId, void *pUserData )
{
	CListNode *pTempLNode = NULL;
	CBlock *pTempBlock = NULL;
	
	lock( &( fg_BlockManager.Locker ) );
	
	pTempLNode = fg_BlockManager.pBlockLHead;
	while ( pTempLNode )
	{
		pTempBlock = CONTAINER_OF_LIST( pTempLNode, CBlock );			
		
		pTempLNode = pTempLNode->Next;
		
		if ( pTempBlock->iBlockFlag )
		{
			pTempBlock->iCurrentTimeInMileSeconds += 100;
			if ( pTempBlock->iCurrentTimeInMileSeconds >= pTempBlock->iBlockTimeInSeconds * 1000 )
			{
				//time out.
				if ( !(pTempBlock->iIsExiting) )
					pTempBlock->iBlockFlag = 0;//write protect.
			}
		}
	}
	
	unlock( &( fg_BlockManager.Locker ) );

	return 0;
}

static int32_t is_block_manager_ready( void )
{
	int32_t iRetCode = -1;
	
	if ( fg_BlockManager.iInitFlag )
	{
		iRetCode = 0;
	}
	
	return iRetCode;
}
static int32_t init_block_manager( void )
{
	int32_t iRetCode = -1;
	
	if ( !(fg_BlockManager.iInitFlag) )
	{
		if ( init_mutex( &( fg_BlockManager.Locker ) ) >= 0 )
		{
			lock( &( fg_BlockManager.Locker ) );
			
			if ( init_queue( &( fg_BlockManager.stRemoveQ ) ) >= 0 )
			{
				if ( set_queue_water_level( &( fg_BlockManager.stRemoveQ ), 100 ) >= 0 )
				{
					fg_BlockManager.iBlockTimeOutTimerId = register_timer( 100, block_timer_callback, NULL );
					
					if ( fg_BlockManager.iBlockTimeOutTimerId > 0 )
					{
						fg_BlockManager.pBlockLHead = NULL;
						fg_BlockManager.iInitFlag = 1;
						
						iRetCode = 0;
					}
				}
				
				if ( iRetCode < 0 )
				{
					reset_queue( &( fg_BlockManager.stRemoveQ ) );	
				}
			}
			
			unlock( &( fg_BlockManager.Locker ) );
		}
	}
	else 
		iRetCode = 0;
		
	return iRetCode;
}

static void release_block_manager( void )
{
	if ( is_block_manager_ready() >= 0 )
	{
		lock( &( fg_BlockManager.Locker ) );
		
		unregister_timer( fg_BlockManager.iBlockTimeOutTimerId );
		fg_BlockManager.iBlockTimeOutTimerId = 0;
		
		reset_queue( &( fg_BlockManager.stRemoveQ ) );
		fg_BlockManager.pBlockLHead = NULL;
		
		fg_BlockManager.iInitFlag = 0;
		
		unlock( &( fg_BlockManager.Locker ) );
	}
}

//init block.
int32_t init_block( void )
{
	int32_t iRetCode = -1;
	
	if ( is_block_manager_ready() < 0 )
	{
		iRetCode = init_block_manager(  );
	}
	else 
		iRetCode = 0;
	
	return iRetCode;
}

//release block.
void release_block( void )
{
	release_block_manager(  );
}

//create block.
void *block_create( int32_t iTimeInSeconds )
{
	void *pRetCode = NULL;
	CBlock *pNewBlock = NULL;
		
	if ( is_block_manager_ready(  ) < 0 )
		return pRetCode;
	
	if ( iTimeInSeconds > 0 )
	{
		lock( &( fg_BlockManager.Locker ) );
		
		pNewBlock = mem_malloc( sizeof( *pNewBlock ) + sizeof( void * ) );
		if ( pNewBlock )
		{
			memset( pNewBlock, 0x00, sizeof( *pNewBlock ) + sizeof( void * ) );
			
			pNewBlock->iBlockTimeInSeconds = iTimeInSeconds;//write protect.s;
			pNewBlock->iBlockFlag = 1;//is blocking.
			pNewBlock->iIsExiting = 0;
			
			if ( insert_list_head_rear( &( fg_BlockManager.pBlockLHead ), &( pNewBlock->LNode ) ) >= 0 )
			{
				pRetCode = (void *)(((int8u_t *)pNewBlock) + sizeof( *pNewBlock ));
			}
			
			if ( 0 == pRetCode )
			{
				mem_free( pNewBlock );
				pNewBlock = NULL;
			}
		}
		
		unlock( &( fg_BlockManager.Locker ) );
	}
	
	return pRetCode;
}

//destroy block.
int32_t block_destroy( void *pBlockHandle )
{
	int32_t iRetCode = -1;
	
	if ( is_block_manager_ready(  ) < 0 )
		return iRetCode;
		
	if ( pBlockHandle )
	{
		CBlock *pBlock = NULL;
		
		lock( &( fg_BlockManager.Locker ) );
		
		pBlock = (CBlock *)((int8u_t *)pBlockHandle - sizeof( *pBlock ));
		
		if ( remove_list_head_node( &( fg_BlockManager.pBlockLHead ), &( pBlock->LNode ) ) >= 0 )
		{
			mem_free( pBlock );
			pBlock = NULL;
			
			iRetCode = 0;	
		}
		
		unlock( &( fg_BlockManager.Locker ) );
	}
	
	return iRetCode;
}

//request block current thread.
int32_t block_wait( void *pBlockHandle )
{
	int32_t iRetCode = -1;
	
	if ( is_block_manager_ready(  ) < 0 )
		return iRetCode;
		
	if ( pBlockHandle )
	{
		CBlock *pBlock = NULL;
		
		pBlock = (int8u_t *)pBlockHandle - sizeof( *pBlock );
		while ( pBlock->iBlockFlag )//shared read.
			os_sleep( 10 );
			
		iRetCode = 0;
	}
	
	return iRetCode;
}

//block exiting.
int32_t block_exiting( void *pBlockHandle )
{
	int32_t iRetCode = -1;

	if ( is_block_manager_ready(  ) < 0 )
		return iRetCode;
		
	if ( pBlockHandle )
	{
		CBlock *pBlock = NULL;
		
		pBlock = (int8u_t *)pBlockHandle - sizeof( *pBlock );

		pBlock->iIsExiting = 1;
			
		iRetCode = 0;
	}
	
	return iRetCode;	
}

//release block.
int32_t block_post( void *pBlockHandle )
{
	int32_t iRetCode = -1;
	
	if ( is_block_manager_ready(  ) < 0 )
		return iRetCode;
		
	if ( pBlockHandle )
	{
		CBlock *pBlock = NULL;
		
		lock( &( fg_BlockManager.Locker ) );

		pBlock = (int8u_t *)pBlockHandle - sizeof( *pBlock );
		pBlock->iBlockFlag = 0;//write protect.
			
		iRetCode = 0;
		
		unlock( &( fg_BlockManager.Locker ) );
	}
	
	return iRetCode;
}
