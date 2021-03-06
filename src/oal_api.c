#include "../inc/oal_api.h"

#if (__OS_LINUX__)

#include <time.h>

#include <pthread.h>

#endif

#include "../inc/lock.h"
#include "../inc/btree.h"

#include "../inc/mem_api.h"



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
CThread *os_thread_create( os_thread_t thread, void *pParam,  
                                C_THREAD_PRIORITY eThreadPriority, int32_t iStackSize )
{
	CThread *pRetCode = NULL;

	if ( iStackSize <= 0 )
		iStackSize = DEFAULT_STACK_SIZE;
	else 
		iStackSize = (iStackSize / 4) * 4;
		
//	log_print( "iStackSize-->%d.", iStackSize );

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
			pthread_attr_t attr;
			
		//	log_print( "init pine Thread ok................." );
			
			pThread->thread = thread;
			pThread->pParam = pParam;
			pThread->iQuitFlag = 0;
			pThread->ePriority = eThreadPriority;
		
			if ( 0 ==  pthread_attr_init( &attr ) )
			{
				int32_t iOpRet = -1;
				
				//log_print( "init thread attr ok..............." );
				iOpRet = pthread_attr_setstacksize( &attr, iStackSize );
				if ( 0 == iOpRet )
				{
				//	log_print( "set thread stack size ok....................." );
					
				//	log_print( "start to create thread ..................." );
					if ( pthread_create( (pthread_t *)&(pThread->iTid), &attr, os_common_thread, pThread ) == 0 )
					{
						//log_print(" create thread ok!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
						pRetCode = pThread;
						
					//	log_print( "Thread Id----------------------------->%u", iRetCode );
					}
					else 
						log_print( "create thread failed----------------------------------------?" );
						
					//log_print( "create thread end............................" );	
				}
				else 
					log_print( "set stack size failed, iOpRet->%d", iOpRet );
				
				pthread_attr_destroy( &attr );
			}
	
			if ( NULL == pRetCode )
			{
				log_print( "%s %s-%d: create thread failed.iRetCode->%u\r\n", __FILE__, __FUNCTION__, __LINE__, pRetCode );
				
				pine_release( pThread );
				pThread = NULL;
			}
		}

#endif
	}

	return pRetCode;
}

//quit thread.
int32_t os_thread_wait( CThread *pThread )
{
	int32_t iRetCode = -1;

	if ( pThread )
	{
		while ( !(pThread->iQuitFlag) )
		{
			os_sleep( 10 );
		}
			
		//log_print( "%s %s-%d: start to call release thread .....\r\n", __FILE__, __FUNCTION__, __LINE__ );
		iRetCode = pine_release( (CPine *)pThread );
	}
	else 
		log_print( "%s %s-%d: iThreadId->%u ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__, pThread );

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
	//log_print( "thread quit.................." );
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


