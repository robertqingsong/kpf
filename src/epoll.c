#include "../inc/epoll.h"

#include "../inc/net_api.h"

#include "../inc/mem_api.h"

#include "../inc/log.h"

#include "../inc/oal_api.h"

#if (__OS_LINUX__)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#endif

#define MAX_EPOLL_SIZE  (1024 * 10)
#define MAX_EPOLL_RUN_TIMEOUT  (-1)

static void *engin_proc_task( void *pParam )
{
	void *pRetCode = NULL;
	CNetEngine *pEngine = NULL;
	int32_t iActiveSocketCount = -1, i = 0;
	static struct epoll_event *pEvents = NULL;
	
	if ( !pParam )
		return pRetCode;
		
	pEvents = mem_malloc( sizeof( struct epoll_event ) * MAX_EPOLL_SIZE );
	if ( !pEvents )
		return pRetCode;
		
	pEngine = (CNetEngine *)pParam;
	
	while ( pEngine->iIsRunning )
	{
		iActiveSocketCount = epoll_wait( pEngine->iEngineId, pEvents, MAX_EPOLL_SIZE, MAX_EPOLL_RUN_TIMEOUT );
		
		if ( iActiveSocketCount > 0 )
		{
			for ( i = 0; i < iActiveSocketCount; i++ )
			{
				int32_t iSocketId = pEvents[i].data.fd;
				void *pUserData = pEvents[i].data.ptr;
				
				if ( pEngine->pEngineCallback )
				{
					if ( pEngine->pEngineCallback( iSocketId, pUserData ) < 0 )
					{
						log_print( "engine callback returns < 0 failed?????????????????????" );	
					}
				}
			}
		}
	}
	
	mem_free( pEvents );
	pEvents = NULL;
	
	return pRetCode;	
}

int32u_t create_engine( void )
{
	int32u_t iRetCode = 0;
	
	CNetEngine *pNewEngine = NULL;
		
	pNewEngine = mem_malloc( sizeof( *pNewEngine ) + sizeof( void * ) );
	if ( pNewEngine )
	{
		memset( pNewEngine, 0x00, sizeof( *pNewEngine ) + sizeof( void * ) );
		
		//create epoll id.
		pNewEngine->iEngineId = epoll_create( MAX_EPOLL_SIZE );
		if ( pNewEngine->iEngineId >= 0 )
		{
			//create epoll task.
			pNewEngine->iIsRunning = 1;
			pNewEngine->iEngineTid = os_thread_create( engin_proc_task, pNewEngine, OS_THREAD_PRIORITY_NORMAL, 1024 * 10 );
			if ( pNewEngine->iEngineTid > 0 )
			{
				iRetCode = (((int8u_t *)pNewEngine) + sizeof( *pNewEngine ));
			}
			else 
				pNewEngine->iIsRunning = 0;
				
			if ( 0 == iRetCode)
			{
				mem_free( pNewEngine );
					
				close( pNewEngine->iEngineId );
				pNewEngine->iEngineId = -1;
					
				pNewEngine = NULL;
			}
		}	
	}
	
	return iRetCode;	
}

//register reactor callback.
int32_t register_engine_callback( int32u_t iEngineId, engine_callback_t callback )
{
	int32_t iRetCode = -1;
	
	if ( iEngineId && callback )
	{
		CNetEngine *pEngine = NULL;
		
		pEngine = iEngineId - sizeof( *pEngine );
		
		pEngine->pEngineCallback = callback;
		
		iRetCode = 0;
	}
	
	return iRetCode;	
}

void destroy_engine( int32u_t iEngineId )
{
	if ( iEngineId )
	{
		CNetEngine *pEngine = NULL;
		
		pEngine = iEngineId - sizeof( *pEngine );

		//close epoll or kqueue id.
		close( pEngine->iEngineId );
		pEngine->iEngineId = -1;
				
		//release memory.		
		mem_free( pEngine );
		pEngine = NULL;
	}
}

//add engine socket.
int32_t add_engine_socket( int32u_t iEngineId, int32_t iSocketId, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( iEngineId && iSocketId >= 0 )
	{
		CNetEngine *pEngine = NULL;
		struct epoll_event ev;
		
		pEngine = iEngineId - sizeof( *pEngine );
		
		//add socket to ...
		memset( &ev, 0x00, sizeof(ev) );
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = iSocketId;
		ev.data.ptr = pUserData;
		
		if ( epoll_ctl( pEngine->iEngineId, EPOLL_CTL_ADD, iSocketId, &ev ) >= 0 )
			iRetCode = 0;
	}
	
	return iRetCode;	
}

//remove engine socket.
int32_t remove_engine_socket( int32u_t iEngineId, int32_t iSocketId )
{
	int32_t iRetCode = -1;
	
	if ( iEngineId && iSocketId >= 0 )
	{
		CNetEngine *pEngine = NULL;
		struct epoll_event ev;
		
		pEngine = iEngineId - sizeof( *pEngine );
		
		memset( &ev, 0x00, sizeof(ev) );
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = iSocketId;
		//remove socket from ...
		epoll_ctl( pEngine->iEngineId, EPOLL_CTL_DEL, iSocketId, &ev );
		
		iRetCode = 0;
	}
	
	return iRetCode;	
}
