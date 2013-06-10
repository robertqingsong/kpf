#include "../inc/net_api.h"

#if (__OS_LINUX__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#endif

#include "../inc/select.h"
#include "../inc/epoll.h"
#include "../inc/kqueue.h"
#include "../inc/lock.h"

//net reactor.
typedef struct CReactor_t
{
	int32u_t iReactorId;	
	
	int32u_t iEngineId;//net engine which is used to handle all sockets event in this reactor.
	
	reactor_callback_t fReactorCallback;
	
	void *pUserData;
}CReactor;

//socket define.
typedef struct CSocket_t
{
	int32_t iSocketId;
	
	CReactor *pOwnerReactor;
	
	void *pUserData;
}CSocket;

typedef struct CReactorManager_t
{
	int32_t iInitFlag;
	
	CMutex Locker;
}CReactorManager;

static CReactorManager fg_ReactorManager = {
	0, 
	
};

//create socket.
int32u_t net_socket( const C_SOCKET_TYPE eSocketType, const int32_t iIsIPv6 )
{
	int32u_t iRetCode = 0;
	int32_t iSocketId = -1;
	
#if (__OS_LINUX__)
	if ( iIsIPv6 )
		iSocketId = socket( AF_INET6, SOCK_STREAM, 0 );
	else 
		iSocketId = socket( AF_INET, SOCK_STREAM, 0 );
	if ( iSocketId >= 0 )
	{
		CSocket *pNewSocket = NULL;
		
		pNewSocket = mem_malloc( sizeof( *pNewSocket ) );
		if ( pNewSocket )
		{
			memset( pNewSocket, 0x00, sizeof( *pNewSocket ) );
			
			pNewSocket->iSocketId = iSocketId;
			
			iRetCode = pNewSocket;
		}
		
		if ( 0 == iRetCode)
		{
			close( iSocketId );
			iSocketId = -1;
		}
	}
#endif
	
	return iRetCode;	
}

//close socket.
void net_close_socket( const int32u_t iSocketId )
{	
#if (__OS_LINUX__)
	
	if ( iSocketId )
	{
		CSocket *pSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
		
		if ( pSocket->iSocketId >= 0 )
		{
			close( pSocket->iSocketId );
			pSocket->iSocketId = -1;
		}
		
		mem_free( pSocket );
		pSocket = NULL;
	}
#endif
}

//set socket property.
int32_t net_set_socket( const int32u_t iSocketId, 
								const C_SOCKET_OPTION eOption, const CSocketParam *pSocketParam, const int32_t iParamSize )
{
	int32_t iRetCode = -1;
	CSocket *pSocket = NULL;
	
	if ( iSocketId < 0 )
		return iRetCode;
	
	pSocket = (CSocket *)iSocketId;
	switch ( eOption )
	{
	case SOCKET_OPTION_NONE_BLOCK:
	{
#if (__OS_LINUX__)

		int32_t iSocketFlags = fcntl( pSocket->iSocketId, F_GETFL, 0 );
   	if ( iSocketFlags >= 0 )
   	{
   		iSocketFlags = iSocketFlags | O_NONBLOCK;
   		
   		if ( fcntl( pSocket->iSocketId, F_SETFL, iSocketFlags ) >= 0 )
   			iRetCode = 0;
   	}

#endif
	}break ;
	default:
	{
		
	}break ;	
	}
		
	return iRetCode;
}

static int32_t is_reactor_manager_ready( void )
{
	int32_t iRetCode = -1;
	
	if ( fg_ReactorManager.iInitFlag )	
		iRetCode = 0;	
	
	return iRetCode;
}

static int32_t init_reactor_manager( void )
{
	int32_t iRetCode = -1;

	if ( !(fg_ReactorManager.iInitFlag) )	
	{
		if ( init_mutex( &( fg_ReactorManager.Locker ) ) >= 0 )
		{
			lock( &( fg_ReactorManager.Locker ) );
			
			fg_ReactorManager.iInitFlag = 1;
			
			iRetCode = 0;
			
			unlock( &( fg_ReactorManager.Locker ) );
		}
	}
	else  
		iRetCode = 0;
	
	return iRetCode;
}

static void release_reactor_manager( void )
{
	if ( is_reactor_manager_ready(  ) >= 0 )
	{
		lock( &( fg_ReactorManager.Locker ) );
		
		fg_ReactorManager.iInitFlag = 0;
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
}

//init reactor.
int32_t init_reactor( void )
{
	int32_t iRetCode = -1;
	
	iRetCode = init_reactor_manager(  );
	
	return iRetCode;	
}

//release reactor.
void release_reactor( void )
{
	release_reactor_manager(  );
}

static int32_t common_engine_callback( int32_t iSocketId, void *pUserData )
{
	int32_t iRetCode = -1;
	CSocket *pSocket = NULL;
	CReactor *pOwnerReactor = NULL;
	
	if ( !pUserData )
		return iRetCode;
	
	lock( &( fg_ReactorManager.Locker ) );
	
	pSocket = (CSocket *)pUserData;
	pOwnerReactor = pSocket->pOwnerReactor;
	if ( pOwnerReactor )
	{
		if ( pOwnerReactor->fReactorCallback )
		{
			if ( pOwnerReactor->fReactorCallback( pOwnerReactor->iReactorId, iSocketId, pOwnerReactor->pUserData ) >= 0 )
				iRetCode = 0;	
		}
	}
	
	unlock( &( fg_ReactorManager.Locker ) );
	
	return iRetCode;
}

//create reactor.
int32u_t net_reactor( void )
{
	int32u_t iRetCode = 0;
	CReactor *pNewReactor = NULL;
	
	lock( &( fg_ReactorManager.Locker ) );
	
	pNewReactor = mem_malloc( sizeof( *pNewReactor ) + sizeof( void * ) );
	if ( pNewReactor )
	{
		memset( pNewReactor, 0x00, sizeof( *pNewReactor ) + sizeof( void * ) );
		
		pNewReactor->iEngineId = create_engine(  );
		if ( pNewReactor->iEngineId > 0 )
		{
			if ( register_engine_callback( pNewReactor->iEngineId, common_engine_callback ) >= 0 )
			{
				iRetCode = (((int8u_t *)pNewReactor) + sizeof( *pNewReactor ));
				
				pNewReactor->iReactorId = iRetCode;
			}
		}
		
		if ( 0 == iRetCode )
		{
			if ( pNewReactor->iEngineId )
			{
				destroy_engine( pNewReactor->iEngineId );
				pNewReactor->iEngineId = NULL;
			}
			
			mem_free( pNewReactor );
			pNewReactor = NULL;
		}
	}
	
	unlock( &( fg_ReactorManager.Locker ) );
	
	return iRetCode;
}

//register reactor data callback.
int32_t register_reactor_callback( int32u_t iReactorId, reactor_callback_t callback, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( iReactorId > 0 && callback )
	{
		CReactor *pReactor = NULL;
		
		lock( &( fg_ReactorManager.Locker ) );
		
		pReactor = (CReactor *)(iReactorId - sizeof( *pReactor ));
		
		pReactor->fReactorCallback = callback;
		pReactor->pUserData = pUserData;
		
		iRetCode = 0;
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
	
	return iRetCode;	
}

//destroy reactor.
void net_close_reactor( int32u_t iReactorId )
{	
	if ( iReactorId )
	{
		CReactor *pReactor = NULL;
		
		lock( &( fg_ReactorManager.Locker ) );
		
		pReactor = (CReactor *)(iReactorId - sizeof( *pReactor ));
		
		//close net engine.
		destroy_engine( pReactor->iEngineId );
		pReactor->iEngineId = 0;
		
		mem_free( pReactor );
		pReactor = NULL;
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
}

//add reactor socket.
int32_t add_reactor_socket( int32u_t iReactorId, int32u_t iSocketId, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( iReactorId && iSocketId )
	{
		CReactor *pReactor = NULL;
		CSocket *pSocket = NULL;
		
		lock( &( fg_ReactorManager.Locker ) );
		
		pReactor = (CReactor *)(iReactorId - sizeof( *pReactor ));
		
		pSocket = (CSocket *)iSocketId;
		if ( pSocket )
		{	
			pSocket->pUserData = pUserData;
			pSocket->pOwnerReactor = pReactor;
			
			if ( add_engine_socket( pReactor->iEngineId, pSocket->iSocketId, pSocket ) >= 0 )
				iRetCode = 0;
		}
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
	
	return iRetCode;
}

//remove reactor socket.
int32_t remove_reactor_socket( int32u_t iReactorId, int32u_t iSocketId )
{
	int32_t iRetCode = -1;
	
	if ( iReactorId && iSocketId )
	{
		CReactor *pReactor = NULL;
		CSocket *pSocket = NULL;
		
		lock( &( fg_ReactorManager.Locker ) );
		
		pReactor = (CReactor *)(iReactorId - sizeof( *pReactor ));
		pSocket = (CSocket *)iSocketId;
		
		if ( remove_engine_socket( pReactor->iEngineId, pSocket->iSocketId ) >= 0 )
		{
			net_close_socket( iSocketId );
			iSocketId = 0;
			
			iRetCode = 0;
		}
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
	
	return iRetCode;	
}

