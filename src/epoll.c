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
#define MAX_EPOLL_RUN_TIMEOUT  (100)


extern int32_t common_engine_callback( const int32_t iSocketId, void *pUserData );

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
			log_print( "their have sockets available, iActiveSocketCount->%d..................", iActiveSocketCount );
			for ( i = 0; i < iActiveSocketCount; i++ )
			{
				int32_t iSocketId = pEvents[i].data.fd;
				void *pUserData = pEvents[i].data.ptr;
				
				log_print( "socket %d is active..............\r\n", iSocketId );
				if ( common_engine_callback( iSocketId, pUserData ) < 0 )
				{
					log_print( "engine callback returns < 0 failed?????????????????????" );	
				}
			}
		}
		//else 
			//log_print( "no active socket available................." );
	}
	
	mem_free( pEvents );
	pEvents = NULL;
	
	return pRetCode;	
}

CNetEngine *create_engine( void )
{
	CNetEngine *pRetCode = NULL;
	
	CNetEngine *pNewEngine = NULL;
		
	pNewEngine = mem_malloc( sizeof( *pNewEngine ) );
	if ( pNewEngine )
	{
		memset( pNewEngine, 0x00, sizeof( *pNewEngine ) );
		
		//create epoll id.
		pNewEngine->iEngineId = epoll_create( MAX_EPOLL_SIZE );
		if ( pNewEngine->iEngineId >= 0 )
		{
			//create epoll task.
			pNewEngine->iIsRunning = 1;
			pNewEngine->pEngineThread = os_thread_create( engin_proc_task, pNewEngine, OS_THREAD_PRIORITY_NORMAL, 1024 * 512 );
			if ( pNewEngine->pEngineThread )
			{
				pRetCode = pNewEngine;
			}
			else 
			{
				log_print( "%s %s:%d !if ( pNewEngine->iEngineTid > 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				pNewEngine->iIsRunning = 0;
			}
				
			if ( NULL == pRetCode)
			{
				close( pNewEngine->iEngineId );
				pNewEngine->iEngineId = -1;
				
				mem_free( pNewEngine );
					
				pNewEngine = NULL;
			}
		}
		else 
			log_print( "%s %s:%d !if ( pNewEngine->iEngineId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( pNewEngine ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return pRetCode;	
}

void destroy_engine( CNetEngine *pEngine )
{
	if ( pEngine )
	{
		pEngine->iIsRunning = 0;
		os_thread_wait( pEngine->pEngineThread );
		
		//close epoll or kqueue id.
		close( pEngine->iEngineId );
		pEngine->iEngineId = -1;
				
		//release memory.		
		mem_free( pEngine );
		pEngine = NULL;
	}
}

//add engine socket.
int32_t add_engine_socket( CNetEngine *pEngine, int32_t iSocketId, void *pUserData )
{
	int32_t iRetCode = -1;
	
	log_print( "add_engine_socket:-------------------------->" );
	if ( pEngine && iSocketId >= 0 )
	{
		struct epoll_event ev;
		
		//add socket to ...
		memset( &ev, 0x00, sizeof(ev) );
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = iSocketId;
		ev.data.ptr = pUserData;
		
		log_print( "start to add socket to epoll: pEngine->iEngineId->%d, iSocketId->%d...............", 
						pEngine->iEngineId, iSocketId );
		if ( epoll_ctl( pEngine->iEngineId, EPOLL_CTL_ADD, iSocketId, &ev ) >= 0 )
		{
			log_print( "add socket to epoll ok................." );
			iRetCode = 0;
		}
		else 
			log_print( "%s %s:%d !if ( epoll_ctl( pEngine->iE failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( iEngineId && iSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	log_print( "add_engine_socket<--------------------------------" );	
	
	return iRetCode;	
}

//remove engine socket.
int32_t remove_engine_socket( CNetEngine *pEngine, int32_t iSocketId )
{
	int32_t iRetCode = -1;
	
	if ( pEngine && iSocketId >= 0 )
	{
		struct epoll_event ev;
		
		memset( &ev, 0x00, sizeof(ev) );
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = iSocketId;
		//remove socket from ...
		
		log_print( "start to delete socket from epoll................" );
		epoll_ctl( pEngine->iEngineId, EPOLL_CTL_DEL, iSocketId, &ev );
		log_print( "remove socket from epoll ok.................." );
		
		iRetCode = 0;
	}
	else 
		log_print( "%s %s:%d !if ( iEngineId && iSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}
