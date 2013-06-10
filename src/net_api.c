#include "../inc/net_api.h"



#include "../inc/select.h"
#include "../inc/epoll.h"
#include "../inc/kqueue.h"
#include "../inc/lock.h"

#include "../inc/mem_api.h"

#include "../inc/log.h"

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
#include <netdb.h>

#endif

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

//get local ip.
int32_t net_get_local_ip( int8_t *pIPBuf, const int32_t iIPBufLen )
{
	int32_t iRetCode = -1;

	if ( pIPBuf && iIPBufLen > 0 )
	{
		int8_t szHostName[128] = { 0x00, };
		if ( gethostname(szHostName, 128 ) == 0 )
		{	 
			 struct hostent *pHost = gethostbyname(szHostName);
			 
			 if ( pHost )
			 {
			 	struct sockaddr_in addr;
				struct in_addr **ppAddrList = NULL;
			
				memset( &addr, 0x00, sizeof(addr) );

				ppAddrList = (struct in_addr **)pHost->h_addr_list;
				addr.sin_addr = *ppAddrList[0];
		
				iRetCode = net_n2ip( addr.sin_addr.s_addr, pIPBuf, iIPBufLen );
			 }
			 else 
			 	log_print( "%s %s:%d !if ( pHost ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		}
		else 
			log_print( "%s %s:%d !if ( gethostname(szHostName, 128 ) == 0 )failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( pIPBuf && iIPBufLen > 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//ip address to int.
int64u_t net_ip2n( const int8_t *pIP )
{
	int64u_t iRetCode = 0;
	
	if ( pIP )
	{
		iRetCode = inet_addr( pIP );
	}
	else 
		log_print( "%s %s:%d !if ( pIP ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
	return iRetCode;	
}

//n to ip.
int32_t net_n2ip( const int64u_t inIP, int8_t *pIP, const int32_t iIPBufLen )
{
	int32_t iRetCode = -1;
	
	if ( pIP && iIPBufLen > 0 )
	{
		const int8_t *pTempIP = NULL;
		struct in_addr addr;
		int32_t iLen = 0;
		
		memset( &addr, 0x00, sizeof(addr) );
		memcpy( &addr, &inIP, sizeof(addr) );
		pTempIP = inet_ntoa( addr );
		
		if ( pTempIP )
		{
			iLen = strlen( pTempIP );
			if ( iLen < iIPBufLen )
			{
				memcpy( pIP, pTempIP, iLen + 1 );
				
				printf( "inIP-->%u.\r\n", inIP );
				printf( "pIP-->%s.\r\n", pIP );
			
				iRetCode = 0;
			}
			else 
				log_print( "%s %s:%d !if ( iLen < iIPBufLen ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		}
		else 
			log_print( "%s %s:%d !if ( pTempIP ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( pIP && iIPBufLen > 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//create socket.
int32u_t net_socket( const C_SOCKET_TYPE eSocketType, const int32_t iIsIPv6 )
{
	int32u_t iRetCode = 0;
	int32_t iSocketId = -1;
	
#if (__OS_LINUX__)
	if ( iIsIPv6 )
	{
		if ( SOCKET_TYPE_STREAM == eSocketType )
			iSocketId = socket( AF_INET6, SOCK_STREAM, 0 );
		else if ( SOCKET_TYPE_DGRAM == eSocketType )
			iSocketId = socket( AF_INET6, SOCK_DGRAM, 0 );
	}
	else 
	{
		if ( SOCKET_TYPE_STREAM == eSocketType )
			iSocketId = socket( AF_INET, SOCK_STREAM, 0 );
		else if ( SOCKET_TYPE_DGRAM == eSocketType )
			iSocketId = socket( AF_INET, SOCK_DGRAM, 0 );
	}
	
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
		else 
			log_print( "%s %s:%d !if ( pNewSocket ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
		if ( 0 == iRetCode)
		{
			close( iSocketId );
			iSocketId = -1;
		}
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
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
		else 
			log_print( "%s %s:%d !if ( pSocket->iSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
		mem_free( pSocket );
		pSocket = NULL;
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
#endif
}

//set socket property.
int32_t net_set_socket( const int32u_t iSocketId, 
								const C_SOCKET_OPTION eOption, const CSocketParam *pSocketParam, const int32_t iParamSize )
{
	int32_t iRetCode = -1;
	CSocket *pSocket = NULL;
	
	if ( !iSocketId )
		return iRetCode;
	
	pSocket = (CSocket *)iSocketId;
	log_print( "net_set_socket: pSocket-->%u", pSocket );
	log_print( "pSocket->iSocketId->%d", pSocket->iSocketId );
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
   		else 
   			log_print( "%s %s:%d !if ( fcntl( pSocket->iSocketId, F_SETFL failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
   	}
   	else 
   		log_print( "%s %s:%d !if ( iSocketFlags >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );

#endif
	}break ;
	case SOCKET_OPTION_REUSE_ADDRESS:
	{
#if (__OS_LINUX__)
		int32_t yes = 1;
		
		if ( setsockopt( pSocket->iSocketId, SOL_SOCKET, SO_REUSEADDR, &yes,
								sizeof(yes)) >= 0 ) {
			iRetCode = 0;
		}
		else 
			log_print( "%s %s:%d !if ( setsockopt( pSocket->iSocketId, SOL failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
#endif
	}break ;
	default:
	{
		log_print( "%s %s:%d !unknown option failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}break ;	
	}
		
	return iRetCode;
}

//get domain's ip address.
int32_t net_get_domain_ip( const int8_t *pDomainName, int8_t *pIP, const int32_t iIPBufLen )
{
	int32_t iRetCode = -1;

	if ( pDomainName && pIP && iIPBufLen > 0 )
	{
		struct hostent *pHost = NULL;
		
		pHost = gethostbyname( pDomainName );
		if ( pHost )
		{
			struct sockaddr_in addr;
			struct in_addr **ppAddrList = NULL;
			
			printf( "pDomainName-->%s.\r\n", pDomainName );
			
			memset( &addr, 0x00, sizeof(addr) );
			
			printf( "start to get domain ip................\r\n" );
			ppAddrList = (struct in_addr **)pHost->h_addr_list;
			addr.sin_addr = *ppAddrList[0];
			
			printf( "get domain ip end..................\r\n" );
		
			iRetCode = net_n2ip( addr.sin_addr.s_addr, pIP, iIPBufLen );
		}
		else 
			log_print( "%s %s:%d !if ( pHost ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( pDomainName && pIP && iIPB failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//bind socket and address.
int32_t net_bind( const int32u_t iSocketId, const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;
	int32_t iOkFlag = 0;
	
#if (__OS_LINUX__)
	
	if ( iSocketId && pNetAddr )
	{
		CSocket *pSocket = NULL;
		struct sockaddr_in addr;
		
		pSocket = (CSocket *)iSocketId;
		
      //     configure ip & port for listen
      memset( &addr, 0x00, sizeof( addr ) );
      addr.sin_family = PF_INET;
      addr.sin_port = htons( pNetAddr->iPort );
      if ( pNetAddr->pIP[0] >= '0' && pNetAddr->pIP[0] <= '9' )
      {
      	addr.sin_addr.s_addr = net_ip2n( pNetAddr->pIP );
      	iOkFlag = 1;
      }
      else 
      {
      	int8_t pIP[32] = { 0x00, };
      	
      	if ( net_get_domain_ip( pNetAddr->pIP, pIP, sizeof(pIP) ) >= 0 )
      	{
      		addr.sin_addr.s_addr = net_ip2n( pIP );
      		iOkFlag = 1;
      	}
      }

      //     size of address
      
      if ( iOkFlag && (bind( pSocket->iSocketId, (struct sockaddr *)&addr, sizeof(addr) ) >= 0) )
      	iRetCode = 0;
      else 
      	log_print( "%s %s:%d !if ( iOkFlag && (bind( pSocket->iS failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pNetAddr ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
#endif

	return iRetCode;
}

//listen socket.
int32_t net_listen( const int32u_t iSocketId, const int32_t iListenCount )
{
	int32_t iRetCode = -1;

	if ( iSocketId && iListenCount > 0 )
	{
#if (__OS_LINUX__)
	
		CSocket *pSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
	
		if ( listen( pSocket->iSocketId, iListenCount ) >= 0 )
			iRetCode = 0;
		else 
			log_print( "%s %s:%d !if ( listen( pSocket->iSocketId, failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
#endif
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && iListenCount > 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//accept socket.
int32u_t net_accept( const int32u_t iSocketId, CNetAddr *pClientAddr )
{
	int32u_t iRetCode = 0;

	if ( iSocketId && pClientAddr )
	{
#if (__OS_LINUX__)
		CSocket *pSocket = NULL, *pNewSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
		
		log_print( "pSocket-->%u", pSocket );
		
		pNewSocket = mem_malloc( sizeof( *pNewSocket ) );
		if ( pNewSocket )
		{
			int32_t iNewSocketId = -1;
			struct sockaddr_in addr;
			int32_t iLen = sizeof(addr);
			
			memset( pNewSocket, 0x00, sizeof(*pNewSocket) );
			
			iNewSocketId = accept( pSocket->iSocketId, (struct sockaddr *)&addr, &iLen );
			if ( iNewSocketId >= 0 )
			{
				const int8_t pClientIP[32] = { 0x00, };
				
				log_print( "iNewSocket-->%d", iNewSocketId );
				
				pNewSocket->iSocketId = iNewSocketId;
				pNewSocket->pOwnerReactor = pSocket->pOwnerReactor;
				
				if ( net_n2ip( addr.sin_addr.s_addr, pClientIP, sizeof(pClientIP) ) >= 0 )
				{
					memcpy( pClientAddr->pIP, pClientIP, strlen(pClientIP) + 1 );
					
					pClientAddr->iPort = net_n2hs( addr.sin_port );
					
					log_print( "pNewSocket-->%u", pNewSocket );
					log_print( "pNewSocket->iSocketId-->%d", pNewSocket->iSocketId );
					
					iRetCode = pNewSocket;
				}
				else 
					log_print( "%s %s:%d !if ( net_n2ip( addr.sin_addr.s failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			}
			else 
				log_print( "%s %s:%d !if ( iNewSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			
			if ( 0 == iRetCode )
			{
				log_print( "accept failed????????????????????????????" );
				mem_free( pNewSocket );
				pNewSocket = NULL;
			}
		}
		else 
			log_print( "%s %s:%d !if ( pNewSocket ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
#endif
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pCli failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//bind socket and address.
int32_t net_connect( const int32u_t iSocketId, const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;
	int32_t iOkFlag = 0;
	
#if (__OS_LINUX__)
	
	if ( iSocketId && pNetAddr )
	{
		CSocket *pSocket = NULL;
		struct sockaddr_in addr;
		
		pSocket = (CSocket *)iSocketId;
		
      //     configure ip & port for listen
      memset( &addr, 0x00, sizeof( addr ) );
      addr.sin_family = PF_INET;
      addr.sin_port = htons( pNetAddr->iPort );
      if ( pNetAddr->pIP[0] >= '0' && pNetAddr->pIP[0] <= '9' )
      {
      	addr.sin_addr.s_addr = net_ip2n( pNetAddr->pIP );
      	iOkFlag = 1;
      }
      else 
      {
      	int8_t pIP[32] = { 0x00, };
      	
      	if ( net_get_domain_ip( pNetAddr->pIP, pIP, sizeof(pIP) ) >= 0 )
      	{
      		addr.sin_addr.s_addr = net_ip2n( pIP );
      		
#if (LOG_SUPPORT)
{
				int8_t logBuf[128] = { 0x00, };
				
				printf( "pIP->%s.\r\n", pIP );
				
      		printf( "addr.sin_addr.s_addr-->%u\r\n", addr.sin_addr.s_addr );

				net_n2ip( addr.sin_addr.s_addr, logBuf, sizeof(logBuf) );
	
				printf( "ip-->%s.\r\n", logBuf );
		
}
#endif      		
      		
      		iOkFlag = 1;
      	}
      }

      //     size of address
      
      if ( iOkFlag && (connect( pSocket->iSocketId, (struct sockaddr *)&addr, sizeof(addr) ) >= 0) )
      	iRetCode = 0;
      else 
      	log_print( "%s %s:%d !if ( iOkFlag && (connect( pSocket-> failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pNetAddr ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
#endif

	return iRetCode;
}

//send tcp data.
int32_t net_send( int32u_t iSocketId, const int8u_t *pData, const int32_t iDataLen )
{
	int32_t iRetCode = -1;
	
	log_print( "net_send:------------------------_>" );
	if ( iSocketId && pData && iDataLen > 0 )
	{
#if (__OS_LINUX__)
		CSocket *pSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
		
		log_print( "pSocket-->%u", pSocket );
		
		iRetCode = send( pSocket->iSocketId, pData, iDataLen, 0 );
		
		if ( 0 == iRetCode )
		{
			log_print( "%s %s:%d !if ( 0 == iRetCode )failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			iRetCode = SOCKET_ERROR;	
		}
		else if ( iRetCode < 0 )
		{
			if ( EAGAIN != errno )
			{
				log_print( "%s %s:%d !if ( EAGAIN != errno ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				
				iRetCode = SOCKET_ERROR;	
			}
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pData & failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	log_print( "net_send:<------------------------" );
	
	return iRetCode;	
}

//receive tcp data.
int32_t net_recv( int32u_t iSocketId, int8u_t *pRecvDataBuf, const int32_t iRecvBufLen )
{
	int32_t iRetCode = -1;
	
	if ( iSocketId && pRecvDataBuf && iRecvBufLen > 0 )
	{
#if (__OS_LINUX__)
		CSocket *pSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
		
		iRetCode = recv( pSocket->iSocketId, pRecvDataBuf, iRecvBufLen, 0 );
		
		if ( 0 == iRetCode )
		{
			log_print( "%s %s:%d !if ( 0 == iRetCode )failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			iRetCode = SOCKET_ERROR;	
		}
		else if ( iRetCode < 0 )
		{
			if ( EAGAIN != errno )
			{
				log_print( "%s %s:%d !if ( EAGAIN != errno ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				iRetCode = SOCKET_ERROR;	
			}
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pRecvDa failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//send udp data.
int32_t net_sendto( int32u_t iSocketId, const int8u_t *pData, const int32_t iDataLen )
{
	int32_t iRetCode = -1;
	
	if ( iSocketId && pData && iDataLen > 0 )
	{
#if (__OS_LINUX__)
		CSocket *pSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
		
		iRetCode = sendto( pSocket->iSocketId, pData, iDataLen, 0, NULL, 0 );
		
		if ( iRetCode < 0 )
		{
			if ( EAGAIN != errno )
			{
				log_print( "%s %s:%d !if ( EAGAIN != errno ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				
				iRetCode = SOCKET_ERROR;	
			}
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pData && failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//receive upp data.
int32_t net_recvfrom( int32u_t iSocketId, int8u_t *pRecvDataBuf, const int32_t iRecvBufLen )
{
	int32_t iRetCode = -1;
	
	if ( iSocketId && pRecvDataBuf && iRecvBufLen > 0 )
	{
#if (__OS_LINUX__)
		CSocket *pSocket = NULL;
		
		pSocket = (CSocket *)iSocketId;
		
		iRetCode = recvfrom( pSocket->iSocketId, pRecvDataBuf, iRecvBufLen, 0, NULL, 0 );
		
		if ( iRetCode < 0 )
		{
			if ( EAGAIN != errno )
			{
				log_print( "%s %s:%d !if ( EAGAIN != errno ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				iRetCode = SOCKET_ERROR;	
			}
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pRecvDataB failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
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
		else 
			log_print( "%s %s:%d !if ( init_mutex( &( fg_ReactorM failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
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
			else 
				log_print( "%s %s:%d !if ( register_engine_callback failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		}
		else 
			log_print( "%s %s:%d !if ( pNewReactor->iEn failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
		if ( 0 == iRetCode )
		{
			log_print( "%s %s:%d !if ( 0 == iRetCode ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			
			if ( pNewReactor->iEngineId )
			{
				destroy_engine( pNewReactor->iEngineId );
				pNewReactor->iEngineId = NULL;
			}
			
			mem_free( pNewReactor );
			pNewReactor = NULL;
		}
	}
	else 
		log_print( "%s %s:%d !if ( pNewReactor ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
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
	else 
		log_print( "%s %s:%d !if ( iReactorId > 0 && callback ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
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
	else 
		log_print( "%s %s:%d !if ( iReactorId ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
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
			else 
				log_print( "%s %s:%d !if ( add_engine_socket( pReactor->iE failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		}
		else 
			log_print( "%s %s:%d !if ( pSocket ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
	else 
		log_print( "%s %s:%d !if ( iReactorId && iSocketId ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
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
		else 
			log_print( "%s %s:%d !if ( remove_engine_socket( pRe failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
		unlock( &( fg_ReactorManager.Locker ) );
	}
	else 
		log_print( "%s %s:%d !if ( iReactorId && iSocketId ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//network to host.
int16u_t net_n2hs( int16u_t iInData )
{
	int16u_t iRetCode = 0;
	
	iRetCode = ntohs( iInData );
	
	return iRetCode;	
}

//host to network.
int16u_t net_h2ns( int16u_t iInData )
{
	int16u_t iRetCode = 0;
	
	iRetCode = htons( iInData );
	
	return iRetCode;	
}

