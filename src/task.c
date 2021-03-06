#include "../inc/task.h"

#include "../inc/mem_api.h"

#include "../inc/oal_api.h"

typedef struct CTask_t
{
	task_callback_t task;
	
	CThread **ppTaskThreadArray;
	int32_t iTaskThreadCount;
	
	CQueue stTaskQ;
}CTask;

//create task.
void *create_task( const task_callback_t task, const int32_t iMaxThreadCount, int32_t iMaxWaterLevel )
{
	void *pRetCode = NULL;

	if ( task && iMaxThreadCount > 0 && iMaxWaterLevel > 0 )
	{	
		CTask *pNewTask = NULL;
	
		pNewTask = mem_malloc( sizeof(*pNewTask) );
		if ( pNewTask )
		{
			memset( pNewTask, 0x00, sizeof(*pNewTask) );
		
			if ( init_queue( &(pNewTask->stTaskQ) ) >= 0 )
			{
			if ( set_queue_water_level( &(pNewTask->stTaskQ), iMaxWaterLevel ) >= 0 )
			{
			pNewTask->task = task; 
			pNewTask->ppTaskThreadArray = mem_malloc( sizeof(CThread *) * iMaxThreadCount );
			if ( pNewTask->ppTaskThreadArray )
			{
				int i = 0;
				int32_t iOpRet = 0;
				
				pNewTask->iTaskThreadCount = iMaxThreadCount;
				
				memset( pNewTask->ppTaskThreadArray, 0x00, sizeof( CThread * ) * iMaxThreadCount );
				
				for ( i = 0; i < pNewTask->iTaskThreadCount; i++ )
				{
					pNewTask->ppTaskThreadArray[i] = os_thread_create( task, &(pNewTask->stTaskQ), OS_THREAD_PRIORITY_NORMAL, 1024 * 20 );
					if ( !pNewTask->ppTaskThreadArray[i] )
					{
						iOpRet = -1;
						break ;
					}
				}
				
				if ( iOpRet < 0 )
				{
					for ( i = 0; i < pNewTask->iTaskThreadCount; i++ )
					{
						if ( pNewTask->ppTaskThreadArray[i] )
						{
							os_thread_wait( pNewTask->ppTaskThreadArray[i] );
						}
					}
					
					mem_free( pNewTask->ppTaskThreadArray );
					pNewTask->ppTaskThreadArray = NULL;
					
					mem_free( pNewTask );
					pNewTask = NULL;
				}
				
				pRetCode = (void *)pNewTask;
			}
			}
			}
		}
	}
	
	return pRetCode;
}

//destroy task.
void destroy_task( void *pTaskHandle )
{
	if ( pTaskHandle )
	{
		CTask *pTask = (CTask *)pTaskHandle;
		int32_t i = 0;
		
		for ( i = 0; i < pTask->iTaskThreadCount; i++ )
		{
			if ( pTask->ppTaskThreadArray[i] )
			{
				os_thread_wait( pTask->ppTaskThreadArray[i] );
			}
		}
		
		mem_free( pTask->ppTaskThreadArray );
		pTask->ppTaskThreadArray = NULL;
					
		mem_free( pTask );
		pTask = NULL;
	}
}

//post task message.
int32_t post_task_msg( void *pTaskHandle, const CMessage *pNewMsg )
{
	int32_t iRetCode = -1;
	
	if ( pTaskHandle && pNewMsg )
	{
		CTask *pTask = (CTask *)pTaskHandle;
		
		iRetCode = en_queue( &(pTask->stTaskQ), &( pNewMsg->QNode ) );
	}
	
	return iRetCode;	
}
