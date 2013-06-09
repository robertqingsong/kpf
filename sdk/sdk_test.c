#if 0
#include "log.h"

#include "queue.h"

#include "pine.h"

#include "mem_api.h"

typedef struct CPerson_t
{
	EXTENDS_PINE

	int32_t (*on_destory_child)( void *pPerson );

	int32_t m_iHasPersonChild;		

	int32_t m_iAge;//age of person;
	int8_t m_btSex[2];//sex of person;
	int32_t m_btId[32];//identification of person;
}CPerson;

#define EXTENDS_PERSON \
	EXTENDS_PINE \
	int32_t (*on_destory_child)( void *pPerson ); \
	int32_t m_iHasPersonChild;	\
	int32_t m_iAge; \
	int8_t m_btSex[2]; \
	int32_t m_btId[32]; \

#define CHILD_ADDR_OF_PERSON(person) \
		(( int8u_t * )( ((int8u_t *)person) + sizeof( CPerson ) ))

typedef struct CChinese_t
{
	EXTENDS_PERSON

	int32_t (*on_destory_chinese)( void *pChinese );
	
	int8_t m_btNationality[32];
		
}CChinese;

int32_t release_chinese( void *pChinese );

int32_t release_person( void *pPerson );

CChinese *create_chinese( void )
{
	CChinese *pRetCode = NULL;
	
	log_print( "create_chinese:--------------------->" );
	pRetCode = mem_malloc( sizeof(*pRetCode) );
	if ( pRetCode )
	{
		if ( pine_init( (CPine *)pRetCode ) >= 0 )
		{
			pRetCode->on_destory_child = release_person;

			pRetCode->on_destory_chinese = release_chinese;

			pRetCode->m_btNationality[0] = '1';
			
			pRetCode->m_iHasChild = 1;
			pRetCode->m_iHasPersonChild = 1;
		}
		else
			log_print( "init chinese base pine failed???????????????" );
	}
	else
		log_print( "malloc chinese failed????????????" );

	log_print( "create_chinese<----------------------------" );

	return pRetCode;
}

int32_t release_chinese( void *pChinese )
{
	int32_t iRetCode = -1;
	
	log_print( "release_chinese:------------------->" );	
	if ( pChinese )
	{
		CChinese *pCh = (CChinese *)pChinese;

		log_print( "pCh->%u", pCh );

		//mem_free( pCh );
		//pCh = NULL;
	}
	else
		log_print( "release_chinese:param is NULL." );

	log_print( "release_chinese<-------------------------" );

	return iRetCode;
}

CPerson *create_person( void )
{
	CPerson *pRetCode = NULL;
	
	pRetCode = (CPerson *)mem_malloc( sizeof( *pRetCode ) );
	if ( pRetCode )
	{
		if ( pine_init( (CPine *)pRetCode ) >= 0 )
		{
			pRetCode->on_destory_child = release_person;

			log_print( "pRerson->on_destory_child-->%u", pRetCode->on_destory_child );			

			pRetCode->m_iAge = 28;
			pRetCode->m_btSex[0] = 'm';
			pRetCode->m_btSex[1] = '\0';
			pRetCode->m_btId[0] = '\0';

			pRetCode->m_iHasChild = 0;

			log_print( "pPerson-->%u", pRetCode );
		}
	}

	return pRetCode;
}

int32_t release_person( void *pPerson )
{
	int32_t iRetCode = -1;

	log_print( "release_person:-------------------->" );

	log_print( "pPerson-->%u", pPerson );

	if ( pPerson )
	{
		CPerson *pPer = (CPerson *)pPerson;
		
		int32_t (*on_destory_person_child)( void *pChild ) = NULL;
		
		if ( pPer->m_iHasPersonChild )
		{
			memcpy( &on_destory_person_child, CHILD_ADDR_OF_PERSON(pPerson), sizeof(on_destory_person_child) );		
	
			if ( on_destory_person_child )
				on_destory_person_child( pPer );
		}

		mem_free( pPerson );

		pPerson = NULL;
	}
	
	log_print( "release_person<---------------------------------" );

	return iRetCode;
}

int main( int argc, char **argv )
{
	//CQueue queue;
	//CPerson *pMe = NULL;

	CChinese *pMe = NULL;

	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

		
	pMe = create_chinese( );
	if ( pMe )
	{
		CChinese *pMe2 = (CChinese *)pine_den( (CPine *)pMe );
		if ( pMe2 )
		{
			pine_release( (CPine *)pMe2 );
		}

		pine_release( (CPine *)pMe );
	}
#if 0
	pMe = create_person();
	if ( pMe )
	{
		CPerson *pMe2 = (CPerson *)operator_den( (CPine *)pMe );
		
		if ( pMe2 )
		{
			log_print( "pMe2->%u", pMe2 );

			operator_release( (CPine *)pMe2 );
		}
		else
			log_print( "pMe2 is NULL." );

		operator_release( (CPine *)pMe );
	}
	else
		log_print( "pMe is NULL." );
	//log_print( "Hello world");
#endif
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

#endif

#if 0

#include "pine.h"

#include "mem_api.h"

#include "log.h"

typedef struct CPerson_t
{
	EXTENDS_PINE
	
	int32_t m_iAge;
	int8_t pm_Name[32];
	int8_t pm_Sex[2];
}CPerson;

#define EXTENDS_PERSON \
	CPerson PersonSuper;

typedef struct CChinese_t
{
	EXTENDS_PERSON
	
	int8_t Nationality[32];
}CChinese;

void init_person( CPine *pPine )
{
	printf( "init_person:---------------->\r\n" );
	
	printf( "init_person end<---------------\r\n" );
}

void release_person( CPine *pPine )
{
	printf( "release_person:------------------->\r\n" );

	printf( "release_person end<--------------------\r\n" );	
}

void init_chinese( CPine *pPine )
{
	CChinese *pMe = (CChinese *)pPine;
	printf( "init_chinese:---------------->\r\n" );
	
	if ( pMe )
	{
		memcpy( pMe->Nationality, "ch", 3 );
	}
	
	printf( "init_chinese end<---------------\r\n" );
}

void release_chinese( CPine *pPine )
{
	printf( "release_chinese:------------------->\r\n" );

	printf( "release_chinese end<--------------------\r\n" );	
}

void show_chinese( CChinese *pChinese )
{
	if ( pChinese )
	{
		printf( "pChinese->Nationality: %s\r\n", pChinese->Nationality );	
	}
}

////add pine method fron base to child.
//int32_t add_pine_method( CPineMethod *pHeadMethod, CPineMethod *pNewMethod );

int main( int argc, char **argv )
{
	CChinese *pPerson = (CPerson *)create_pine( sizeof(*pPerson) );

	printf( "main:---------------------->\r\n" );
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( pPerson )
	{
		CPineMethod *pMethod = NULL, method, method2;
		
		//base method.
		memset( &method, 0x00, sizeof( method ) );
		method.init = init_person;
		method.release = release_person;
		
		add_pine_method( &pMethod, &method );
		
		//child method.
		memset( &method2, 0x00, sizeof( method2 ) );
		method2.init = init_chinese;
		method2.release = release_chinese;
		add_pine_method( &pMethod, &method2 );
		
		printf( "start to init pine......................\r\n" );
		if ( pine_init( (CPine *)pPerson, pMethod ) >= 0 )
		{
			printf( "pine_init call ok...............\r\n" );
			
			show_chinese( pPerson );
			
			printf( "start to release person.............\r\n" );
			pine_release( (CPine *)pPerson );
		}
		else 
			printf( "pine_init failed???????????????????????\r\n" );
	}
	else
		printf( "pPerson is NULL?????????????????????????\r\n" );
	
	printf( "main<----------------------------\r\n" );
	
	return 0;	
}

#endif

#if 0
#include "oal_api.h"

#include "log.h"

#include "lock.h"

int32_t iQuitCount = 0;

CMutex locker;

void *test( void *pParam )
{
	int32_t iCounter = 0;
	
	while ( 1 )
	{
		iCounter++;
		//log_print( "counter-->%d\r\n", iCounter );		
		
		os_sleep( 10 );
		
		if ( iCounter > 50 )
			break ;
	}
	
	lock( &locker );
	iQuitCount++;
	unlock( &locker );

	return NULL;	
}

int main( int argc, char ** argv )
{
	if ( init_pine_system(  ) < 0 )
		return -1;	
	
	if ( init_mutex( &locker ) < 0 )
		return -1;
		
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	int32u_t tid1 = os_thread_create( test, NULL, OS_THREAD_PRIORITY_NORMAL, 20 * 1024 );
	int32u_t tid2 = os_thread_create( test, NULL, OS_THREAD_PRIORITY_NORMAL, 512 * 1024 );
	int32u_t tid3 = os_thread_create( test, NULL, OS_THREAD_PRIORITY_NORMAL, 100 * 1024 );
	
	
	
	while ( iQuitCount < 3 )
		os_sleep( 10 );
		
	os_thread_wait( tid1 );
	os_thread_wait( tid2 );
	os_thread_wait( tid3 );
	
	log_print( "exit........" );
	
	release_pine_system(  );
	
	return 0;	
}

#endif

#if 0
#include "timer.h"
#include "oal_api.h"

#include "log.h"

int count = 0;
int32u_t timerId;

int32_t timer_callback( int32u_t iTimerId, void *pUserData )
{
	printf( "timer callback:\r\n" );
	
	count++;
	if ( count > 3 )
		unregister_timer( timerId );
	
	return 0;
}

int main( int argc, char **argv )
{
	
	
	
		if ( init_pine_system(  ) < 0 )
		return -1;	
	
		
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( init_timer(  ) < 0 )
		return -1;
	printf( "init timer ok.............\r\n" );
	
	timerId = register_timer( 1000, timer_callback, NULL );
	printf( "timerId-->%u\r\n", timerId );

	
	while ( 1 )
		os_sleep(100);
		
	release_timer(  );
	
		release_pine_system(  );

	return 0;	
}

#endif

#include "timer.h"
#include "oal_api.h"

#include "log.h"

#include "block.h"

#include "pine.h"

int main( int argc, char **argv )
{
	int32u_t iBlockId = 0;
	
	if ( init_pine_system(  ) < 0 )
		return -1;	
	
		
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );

	iBlockId = block_create( 1 );
	if ( iBlockId > 0 )
	{
		block_wait( iBlockId );
		
		log_print( "thread continues.............." );
		
		block_destroy( iBlockId );	
	}
	
		
	release_pine_system(  );

	return 0;	
}


