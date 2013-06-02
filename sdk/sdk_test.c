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

