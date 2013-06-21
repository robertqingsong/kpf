#include "task.h"

#include "log.h"

#include "session.h"

#include "oal_api.h"

int32_t iCount = 0;

void *task_test( void *pParam )
{
	CQueue *pTaskQ = (CQueue *)pParam;
	CQueueNode *pTaskQNode = NULL;
	CMessage *pMesg = NULL;
	
	if ( !pTaskQ )
		return NULL;
		
	while ( 1 )
	{
		pTaskQNode = de_queue( pTaskQ );
		
		if ( pTaskQNode )
		{
			pMesg = CONTAINER_OF_QUEUE( pMesg, CMessage );
			
			log_print( "task message received..............." );
		}
	}
	
	return NULL;	
}

int main( int argc, char **argv )
{
	void *pTaskHandle = NULL;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

	if( init_session(  ) < 0 )
		return -1;
		
	pTaskHandle = create_task( task_test, 5, 100 );
	if ( pTaskHandle )
	{
		CMessage stMsg;
		
		memset( &stMsg, 0x00, sizeof(stMsg) );
		
		post_task_msg( pTaskHandle, &stMsg );
		
		log_print( "create task ok................." );
	}
	else 
		log_print( "create task failed?????????????????????" );
	
	while ( 1 )
		os_sleep( 1000 );
		
	release_session(  );
	
	return 0;	
}
