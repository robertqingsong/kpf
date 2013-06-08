#include "../inc/oal_api.h"

#if (__OS_LINUX__)

#include <time.h>

#include <pthread.h>

#endif

#include "../inc/lock.h"
#include "../inc/btree.h"

#include "../inc/mem_api.h"

#include "../inc/pine.h"

typedef struct CThread_t
{
	EXTENDS_PINE

	os_thread_t thread;
	C_THREAD_PRIORITY ePriority;
	void *pParam;

	int64u_t iTid;

	int32_t iQuitFlag;

}CThread;


//thread methods.
static void init_thread( CPine *pPine );
static void release_thread( CPine *pPine );
static void *os_common_thread( void *pParam );

//public function defines.
//get handle
int32_t get_handle( void )
{
#define MAX_HANDLE_SIZE  (0xefffffff)
	int32_t iRetCode = -1;

	srand( (int) time(0) );

	iRetCode = rand() % MAX_HANDLE_SIZE;

	if ( iRetCode < 0 )
		iRetCode = -iRetCode;
	else if ( 0 == iRetCode )
		iRetCode = 100;

	return iRetCode;
}

//os thread api.
int32u_t os_thread_create( os_thread_t thread, void *pParam,  
                                C_THREAD_PRIORITY eThreadPriority )
{
	int32u_t iRetCode = 0;


	if ( thread )
	{
#if (__OS_LINUX__)

		CThread *pThread = NULL;
		CPineMethod ThreadMethod;
		

		pThread = (CThread *)create_pine( sizeof( *pThread ) );

		memset( &ThreadMethod, 0x00, sizeof( ThreadMethod ) );
		ThreadMethod.init = init_thread;
		ThreadMethod.release = release_thread;
		if ( pThread && pine_init( ( CPine * )pThread, &ThreadMethod ) >= 0 )
		{
			pThread->thread = thread;
			pThread->pParam = pParam;
			pThread->iQuitFlag = 0;
			pThread->ePriority = eThreadPriority;
		
			if ( pthread_create( (pthread_t *)&(pThread->iTid), NULL, os_common_thread, pThread ) >= 0 )
			{
				iRetCode = (int32u_t)( pThread );
			}
	
			if ( iRetCode <= 0 )
			{
				log_print( "%s %s-%d: !if ( iRetCode < 0 ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				mem_free( pThread );
				pThread = NULL;
			}
		}

#endif
	}

	return iRetCode;
}

//quit thread.
int32_t os_thread_wait( int32u_t iThreadId )
{
	int32_t iRetCode = -1;

	if ( iThreadId > 0 )
	{
		CThread *pThread = (CThread *)iThreadId;
		while ( !(pThread->iQuitFlag) )
		{
			os_sleep( 10 );
		}
			
		log_print( "%s %s-%d: start to call release thread .....\r\n", __FILE__, __FUNCTION__, __LINE__ );
		iRetCode = pine_release( (CPine *)pThread );
	}
	else 
		log_print( "%s %s-%d: iThreadId->%u ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__, iThreadId );

	return iRetCode;
}

//sleep api.
void os_sleep( int32_t iTimeInMileSeconds )
{
#if (__OS_LINUX__)

	if ( iTimeInMileSeconds > 0 )
	{
		usleep( iTimeInMileSeconds * 1000 );	
	}

#endif	
}

//thread methods.
static void init_thread( CPine *pPine )
{

}

static void release_thread( CPine *pPine )
{
	log_print( "thread quit.................." );
}

static void *os_common_thread( void *pParam )
{
	void *pRetCode = NULL;
	CThread *pThread = (CThread *)pParam;

	if ( !pThread )
		return pRetCode;
	
	if ( pThread->thread )
		pRetCode = pThread->thread( pThread->pParam );

	pThread->iQuitFlag = 1;

	return pRetCode;
}


