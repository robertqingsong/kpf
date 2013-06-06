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

	CBTreeNode BTNode;
}CThread;

typedef struct CThreadManager_t
{
	int32_t iInitFlag;

	CBTree *pThreadTree;

	CMutex Locker;
}CThreadManager;

//static global variable define.
static CThreadManager fg_ThreadManager = {
	0, 
	NULL, 
};

//static function declaration.
static int32_t is_thread_manager_ready( void );
static int32_t init_thread_manager( void );

static int32_t thread_btree_comp( const void *pValA, const void *pValB,
					void *pParam );

static void *os_common_thread( void *pParam );

//thread methods.
static void init_thread( CPine *pPine );
static void release_thread( CPine *pPine );

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

	if ( is_thread_manager_ready() < 0 )
		if ( init_thread_manager() < 0 )
			return iRetCode;

	if ( thread )
	{
#if (__OS_LINUX__)

		CThread *pThread = NULL;
		CPineMethod ThreadMethod;
		
		lock( &( fg_ThreadManager.Locker ) );

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
				if ( add_btree_node( fg_ThreadManager.pThreadTree, &( pThread->BTNode ) ) >= 0 )
				{
					iRetCode = (int32u_t)&( pThread->BTNode );
				}
			}
	
			if ( iRetCode <= 0 )
			{
				log_print( "%s %s-%d: !if ( iRetCode < 0 ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				mem_free( pThread );
				pThread = NULL;
			}
		}

		unlock( &( fg_ThreadManager.Locker ) );
#endif
	}

	return iRetCode;
}

//quit thread.
int32_t os_thread_wait( int32u_t iThreadId )
{
	int32_t iRetCode = -1;

	if ( is_thread_manager_ready() < 0 )
		if ( init_thread_manager() < 0 )
		{
			log_print( "%s %s-%d: !if ( init_thread_manager() < 0 ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
			return iRetCode;
		}
		
	if ( iThreadId > 0 )
	{
		CBTreeNode *pBTNode = NULL;
		
		lock( &( fg_ThreadManager.Locker ) );
		pBTNode = remove_btree_node( fg_ThreadManager.pThreadTree, iThreadId );
		unlock( &( fg_ThreadManager.Locker ) );
		
		if ( pBTNode )
		{
			CThread *pThread = CONTAINER_OF_BTNODE( pBTNode, CThread );
			while ( !(pThread->iQuitFlag) )
			{
				os_sleep( 10 );
			}
			
			log_print( "%s %s-%d: start to call release thread .....\r\n", __FILE__, __FUNCTION__, __LINE__ );
			iRetCode = pine_release( (CPine *)pThread );
		}
		else 
			log_print( "%s %s-%d: !if ( pBTNode ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
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

static int32_t is_thread_manager_ready( void )
{
	int32_t iRetCode = -1;

	if ( fg_ThreadManager.iInitFlag )
		iRetCode = 0;

	return iRetCode;
}

//static function defines.
static int32_t init_thread_manager( void )
{
	int32_t iRetCode = -1;

	if ( !(fg_ThreadManager.iInitFlag) )
	{
		if ( init_mutex( &( fg_ThreadManager.Locker ) ) >= 0 )
		{
			fg_ThreadManager.pThreadTree = create_btree( thread_btree_comp );
			if ( fg_ThreadManager.pThreadTree )
			{
				fg_ThreadManager.iInitFlag = 1;

				iRetCode = 0;
			}
		}
	}
	else
	{
		iRetCode = 0;
	}

	return iRetCode;
}

static int32_t thread_btree_comp( const void *pValA, const void *pValB,
                                        void *pParam )
{
	int32_t iRetCode = -1;

	if ( pValA && pValB )
	{
		int64u_t A = (int64u_t)pValA, B = (int64u_t)pValB;

		if ( A == B )
			iRetCode = 0;
		else if ( A > B )
			iRetCode = 1;
		else
			iRetCode = -1;
	}

	return iRetCode;
}

