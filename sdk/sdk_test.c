#include "log.h"

#include "queue.h"

#include "pine.h"

#include "mem_api.h"

typedef struct CPerson_t
{
	EXTENDS_PINE

	int32_t (*on_destory_child)( void *pPerson );

	int32_t m_iAge;//age of person;
	int8_t m_btSex[2];//sex of person;
	int32_t m_btId[32];//identification of person;
}CPerson;

int32_t release_person( CPerson *pPerson );
CPerson *create_person( void )
{
	CPerson *pRetCode = NULL;
	
	pRetCode = (CPerson *)mem_malloc( sizeof( *pRetCode ) );
	if ( pRetCode )
	{
		if ( operator_init( pRetCode ) >= 0 )
		{
			pRetCode->on_destory = release_person;

			pRetCode->m_iAge = 28;
			pRetCode->m_btSex[0] = 'm';
			pRetCode->m_btSex[1] = '\0';
			pRetCode->m_btId[0] = '\0';

			log_print( "pPerson-->%u", pRetCode );
		}
	}

	return pRetCode;
}

int32_t release_person( CPerson *pPerson )
{
	int32_t iRetCode = -1;

	log_print( "person destory..............\r\n" );

	log_print( "pPerson-->%u", pPerson );

	if ( pPerson )
	{
	//	mem_free( pPerson );

	//	pPerson = NULL;
	}

	return iRetCode;
}

int main( int argc, char **argv )
{
	//CQueue queue;
	CPerson *pMe = NULL;

	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	pMe = create_person();
	if ( pMe )
	{
		operator_release( pMe );
	}
	//log_print( "Hello world");

#if 0
	if ( init_queue( &queue ) >= 0 )
	{
		CQueueNode tempQNode;

		set_queue_water_level( &queue, 100 );		

		memset( &tempQNode, 0x00, sizeof(tempQNode) );
		log_print("&tempQNode-->%u.\r\n", &tempQNode );
		if ( en_queue( &queue, &tempQNode ) >= 0 )
		{
			CQueueNode *pDelQNode = NULL;

			log_print( "en queu ok." );
			
			pDelQNode = de_queue( &queue );
			if ( pDelQNode )
			{
				log_print( "de queue ok..........." );

				log_print( "pDelQNode-->%u", pDelQNode );
			}
			else
				log_print( "de queue failed?????????????" );
		}
		else
			log_print( "en queu failed???????????" );
	}
#endif
	return 0;
}

