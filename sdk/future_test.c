#include "task.h"

#include "log.h"

#include "session.h"

#include "oal_api.h"

#include "future.h"

int32_t iCount = 0;

typedef struct CTestFuture_t
{
	CFuture stFuture;
	
}CTestFuture;

int32_t iIsRunning = 0;

void *task_test( void *pParam )
{
	CQueue *pTaskQ = (CQueue *)pParam;
	CQueueNode *pTaskQNode = NULL;
	CMessage *pMesg = NULL;
	CTestFuture *pFuture = NULL;
	
	log_print( "task_test:------------------->" );
	
	if ( !pTaskQ )
		return NULL;
		
	while ( iIsRunning )
	{
		pTaskQNode = de_queue( pTaskQ );
		
		if ( pTaskQNode )
		{
			pMesg = CONTAINER_OF_QUEUE( pTaskQNode, CMessage );
			
			pFuture = (CTestFuture *)pMesg;
			if ( pFuture->stFuture.future_callback )
				pFuture->stFuture.future_callback( pFuture );
		}
		else 
			os_sleep( 10 );
	}

	log_print( "task_test end<------------------------" );
	
	return NULL;	
}

int32_t future_callback( struct CFuture_t *pFuture )
{
	int32_t iRetCode = -1;
	
	log_print( "future callback calling...................." );
	
	return iRetCode;
}

int main( int argc, char **argv )
{
	void *pTaskHandle = NULL;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

	if( init_session(  ) < 0 )
		return -1;
	
	iIsRunning = 1;
	pTaskHandle = create_task( task_test, 5, 100 );
	if ( pTaskHandle )
	{
		CTestFuture stTestFuture;
		
		init_future( &stTestFuture, future_callback );
		
		log_print( "create task ok................." );
		
		post_task_msg( pTaskHandle, (CMessage *)&(stTestFuture) );
	}
	else 
		log_print( "create task failed?????????????????????" );
	
	while ( 1 )
		os_sleep( 1000 );
		
	release_session(  );
	
	return 0;	
}
