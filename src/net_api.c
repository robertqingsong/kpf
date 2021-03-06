#include "../inc/net_api.h"



#include "../inc/select.h"
#include "../inc/epoll.h"
#include "../inc/kqueue.h"

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
				
				//printf( "inIP-->%u.\r\n", inIP );
				//printf( "pIP-->%s.\r\n", pIP );
			
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
CSocket *net_socket( const C_SOCKET_TYPE eSocketType, const int32_t iIsIPv6 )
{
	CSocket *pRetCode = NULL;
	int32_t iSocketId = -1;
	
#if (__OS_LINUX__)
	if ( iIsIPv6 )
	{
		if ( SOCKET_TYPE_STREAM == eSocketType )
			iSocketId = socket( AF_INET6, SOCK_STREAM, 0 );
		else if ( SOCKET_TYPE_DGRAM == eSocketType )
			iSocketId = socket( AF_INET6, SOCK_DGRAM, 0 );
		else if ( SOCKET_TYPE_MULTICAST == eSocketType )
		{
			iSocketId = socket( AF_INET6, SOCK_DGRAM, 0 );
		}
	}
	else 
	{
		if ( SOCKET_TYPE_STREAM == eSocketType )
			iSocketId = socket( AF_INET, SOCK_STREAM, 0 );
		else if ( SOCKET_TYPE_DGRAM == eSocketType )
			iSocketId = socket( AF_INET, SOCK_DGRAM, 0 );
		else if ( SOCKET_TYPE_MULTICAST == eSocketType )
		{
			iSocketId = socket( AF_INET, SOCK_DGRAM, 0 );
		}
	}
	
	if ( iSocketId >= 0 )
	{
		CSocket *pNewSocket = NULL;
		
		pNewSocket = mem_malloc( sizeof( *pNewSocket ) );
		if ( pNewSocket )
		{
			memset( pNewSocket, 0x00, sizeof( *pNewSocket ) );
			
			pNewSocket->iSocketId = iSocketId;
		
			
			pRetCode = pNewSocket;
		}
		else 
			log_print( "%s %s:%d !if ( pNewSocket ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
		if ( NULL == pRetCode)
		{
			if ( pNewSocket )
			{
				mem_free( pNewSocket );
				pNewSocket = NULL;				
			}
			
			close( iSocketId );
			iSocketId = -1;
		}
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
#endif
	
	return pRetCode;	
}

//close socket.
void net_close_socket( CSocket *pSocket )
{	
#if (__OS_LINUX__)
	
	if ( pSocket )
	{	
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
int32_t net_set_socket( const CSocket *pSocket, 
								const C_SOCKET_OPTION eOption, const CSocketParam *pSocketParam, const int32_t iParamSize )
{
	int32_t iRetCode = -1;
	
	if ( !pSocket )
		return iRetCode;
	
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
   		{
				//log_print( "pSocket->iSocketId-->%d, set none block ok.................", pSocket->iSocketId );
   			iRetCode = 0;
   		}
   		else 
   		{
   			log_print( "%s %s:%d !if ( fcntl( pSocket->iSocketId, F_SETFL failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
   		}
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
			log_print( "set reuse address ok, pSocket->iSocketId->%d.............................", pSocket->iSocketId );
			iRetCode = 0;
		}
		else 
			log_print( "%s %s:%d !if ( setsockopt( pSocket->iSocketId, SOL failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
#endif
	}break ;
	case SOCKET_OPTION_SET_MULTICAST:
	{
#if (__OS_LINUX__)

		if ( pSocketParam && iParamSize == sizeof( *pSocketParam ) )
		{
			if ( net_set_socket( pSocket, SOCKET_OPTION_REUSE_ADDRESS, NULL, 0 ) >= 0 )
			{
				CNetAddr stNetAddr;
				
				memset( &stNetAddr, 0x00, sizeof(stNetAddr) );
				
				memcpy( stNetAddr.pIP, pSocketParam->pIP, strlen(pSocketParam->pIP) );
				stNetAddr.iPort = pSocketParam->iPort;
				
				if ( net_bind( pSocket, &stNetAddr ) >= 0 )
				{
					struct ip_mreq mreq;
					
					memset( &mreq, 0x00, sizeof(mreq) );
					
					/* use setsockopt() to request that the kernel join a multicast group */
    				mreq.imr_multiaddr.s_addr = net_ip2n( pSocketParam->pIP );
     				mreq.imr_interface.s_addr= net_h2ns( INADDR_ANY );
     				if (setsockopt( pSocket->iSocketId, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq) ) >= 0)
     					iRetCode = 0;
				}
			}
		}

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
		
			
			memset( &addr, 0x00, sizeof(addr) );
			
			ppAddrList = (struct in_addr **)pHost->h_addr_list;
			addr.sin_addr = *ppAddrList[0];
		
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
int32_t net_bind( const CSocket *pSocket, const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;
	int32_t iOkFlag = 0;
	
#if (__OS_LINUX__)
	
	if ( pSocket && pNetAddr )
	{
		struct sockaddr_in addr;
		
      //     configure ip & port for listen
      memset( &addr, 0x00, sizeof( addr ) );
      addr.sin_family = PF_INET;
      addr.sin_port = htons( pNetAddr->iPort );
      
      log_print( "%s:%d", pNetAddr->pIP, pNetAddr->iPort );
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
      {
      	perror( "bind failed:" );
      	log_print( "%s %s:%d !if ( iOkFlag && (bind( pSocket->iS failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
      }
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pNetAddr ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
#endif

	return iRetCode;
}

//listen socket.
int32_t net_listen( const CSocket *pSocket, const int32_t iListenCount )
{
	int32_t iRetCode = -1;

	if ( pSocket && iListenCount > 0 )
	{
#if (__OS_LINUX__)
	
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
CSocket *net_accept( const CSocket *pSocket, CNetAddr *pClientAddr )
{
	CSocket *pRetCode = NULL;

	if ( pSocket && pClientAddr )
	{
#if (__OS_LINUX__)
		CSocket *pNewSocket = NULL;
	
		pNewSocket = mem_malloc( sizeof( CSocket ) );
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
	
				pNewSocket->iSocketId = iNewSocketId;
				pNewSocket->pOwnerReactor = pSocket->pOwnerReactor;
				
				if ( net_n2ip( addr.sin_addr.s_addr, pClientIP, sizeof(pClientIP) ) >= 0 )
				{
					memcpy( pClientAddr->pIP, pClientIP, strlen(pClientIP) + 1 );
					
					pClientAddr->iPort = net_n2hs( addr.sin_port );
					
					log_print( "accept ok, pNewSocket->iSocketId->%d", pNewSocket->iSocketId );
					
					pRetCode = pNewSocket;
				}
				else 
					log_print( "%s %s:%d !if ( net_n2ip( addr.sin_addr.s failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			}
			else 
				log_print( "%s %s:%d !if ( iNewSocketId >= 0 ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			
			if ( NULL == pRetCode )
			{
				log_print( "accept failed????????????????????????????" );
				mem_free( pNewSocket );
				pNewSocket = NULL;
				pRetCode = NULL;
			}
		}
		else 
			log_print( "%s %s:%d !if ( pNewSocket ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
#endif
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pCli failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return pRetCode;	
}

//bind socket and address.
int32_t net_connect( const CSocket *pSocket, const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;
	int32_t iOkFlag = 0;
	
#if (__OS_LINUX__)
	
	if ( pSocket && pNetAddr )
	{
		struct sockaddr_in addr;

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
int32_t net_send( const CSocket *pSocket, const int8u_t *pData, const int32_t iDataLen )
{
	int32_t iRetCode = -1;
	
	if ( pSocket && pData && iDataLen > 0 )
	{
#if (__OS_LINUX__)

		iRetCode = send( pSocket->iSocketId, pData, iDataLen, 0 );
		
		if ( 0 == iRetCode )
		{
			log_print( "%s %s:%d !if ( 0 == iRetCode )failed, pSocket->iSocketId-->%d????????????????", 
						__FILE__, __FUNCTION__, __LINE__, pSocket->iSocketId );
			iRetCode = SOCKET_ERROR;	
		}
		else if ( iRetCode < 0 )
		{
			if ( EAGAIN != errno )
			{
				log_print( "%s %s:%d !if ( EAGAIN != errno ) failed, pSocket->iSocketId-->%d????????????????", 
								__FILE__, __FUNCTION__, __LINE__, pSocket->iSocketId );
				
				iRetCode = SOCKET_ERROR;	
			}
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pData & failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	
	return iRetCode;	
}

//receive tcp data.
int32_t net_recv( const CSocket *pSocket, int8u_t *pRecvDataBuf, const int32_t iRecvBufLen )
{
	int32_t iRetCode = -1;
	
	if ( pSocket && pRecvDataBuf && iRecvBufLen > 0 )
	{
#if (__OS_LINUX__)
		
		iRetCode = recv( pSocket->iSocketId, pRecvDataBuf, iRecvBufLen, 0 );
		
		if ( 0 == iRetCode )
		{
			//log_print( "%s %s:%d !if ( 0 == iRetCode )failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			iRetCode = SOCKET_ERROR;	
		}
		else if ( iRetCode < 0 )
		{
			if ( EAGAIN != errno )
			{
				perror( "--------------->" );
				log_print( "read failed:errno-->%d", errno );

				iRetCode = SOCKET_ERROR;	
			}
			else 
				iRetCode = SOCKET_ERROR_AGAIN;
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pRecvDa failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//send udp data.
int32_t net_sendto( const CSocket *pSocket, const int8u_t *pData, const int32_t iDataLen, CNetAddr *pPeerAddr )
{
	int32_t iRetCode = -100;
	
	
	if ( pSocket && pData && iDataLen > 0 )
	{
#if (__OS_LINUX__)
		struct sockaddr_in addr;
		int32_t iHasAddr = 0;
		
		if ( pPeerAddr )
		{
      	//     configure ip & port for listen
      	memset( &addr, 0x00, sizeof( addr ) );
     		addr.sin_family = PF_INET;
     		addr.sin_port = htons( pPeerAddr->iPort );
     	 	if ( pPeerAddr->pIP[0] >= '0' && pPeerAddr->pIP[0] <= '9' )
    	  	{
      		addr.sin_addr.s_addr = net_ip2n( pPeerAddr->pIP );
      		
      		iHasAddr = 1;
      	
     	 	}
      	else 
      	{
      		int8_t pIP[32] = { 0x00, };
      	
      		if ( net_get_domain_ip( pPeerAddr->pIP, pIP, sizeof(pIP) ) >= 0 )
      		{
      			addr.sin_addr.s_addr = net_ip2n( pIP );
      			
      			iHasAddr = 1;
      		
      		}
    	  }
		}
		
		if ( iHasAddr )
			iRetCode = sendto( pSocket->iSocketId, pData, iDataLen, 0, &addr, sizeof(addr) );
		else  
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
int32_t net_recvfrom( const CSocket *pSocket, int8u_t *pRecvDataBuf, const int32_t iRecvBufLen, CNetAddr *pPeerAddr )
{
	int32_t iRetCode = -1;
	
	if ( pSocket && pRecvDataBuf && iRecvBufLen > 0 && pPeerAddr )
	{
#if (__OS_LINUX__)
		struct sockaddr_in addr;
		int32_t iLen = sizeof(addr);
		
		memset( &addr, 0x00, sizeof(addr) );
		iRetCode = recvfrom( pSocket->iSocketId, pRecvDataBuf, iRecvBufLen, 0, (struct sockaddr *)&addr, &iLen );
		
		if ( iRetCode <= 0 )
		{
			if ( EAGAIN != errno )
			{
				log_print( "%s %s:%d !if ( EAGAIN != errno ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				iRetCode = SOCKET_ERROR;	
			}
		}
		else 
		{
			int8_t pTempIPBuf[32] = { 0x00, };
			
			if ( net_n2ip( addr.sin_addr.s_addr, pTempIPBuf, sizeof(pTempIPBuf) ) >= 0 )
			{
				memcpy( pPeerAddr->pIP, pTempIPBuf, strlen(pTempIPBuf) + 1 );
				pPeerAddr->iPort = net_n2hs( addr.sin_port );
			}
		}

#endif	
	}
	else 
		log_print( "%s %s:%d !if ( iSocketId && pRecvDataB failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//--------------------------------------------------------------------------->
typedef struct CRemoveSocket_t
{
	CSocket *pSocket;
	
	CReactor *pReactor;
	
	CQueueNode QNode;
}CRemoveSocket;

int32_t init_reactor( void )
{
	int32_t iRetCode = -1;
	
	iRetCode = 0;	
	
	return iRetCode;	
}

void release_reactor( void )
{
	
}

int32_t common_engine_callback( const int32_t iSocketId, void *pUserData, int32_t iEndFlag )
{
	int32_t iRetCode = -1;
	CSocket *pSocket = NULL;
	CReactor *pOwnerReactor = NULL;
	CRemoveSocket *pRemoveSocket = NULL;
	CQueueNode *pQNode = NULL;
			
	if ( iSocketId < 0 || !pUserData )
	{
		log_print( "iSocketId-->%d, pUserData-->%u", iSocketId, pUserData );
		return iRetCode;
	}
		
	pSocket = ((CSocket *)pUserData);
	pOwnerReactor = pSocket->pOwnerReactor;
	if ( !pOwnerReactor )
	{
		log_print( "pOwnerReactor is NULL?????????????????????????????????" );
		return iRetCode;
	}
		
	lock( &( pOwnerReactor->Locker ) );
	
	if ( pOwnerReactor->fReactorCallback )
	{
		if ( pOwnerReactor->fReactorCallback( pOwnerReactor, pSocket, pOwnerReactor->pUserData ) >= 0 )
				iRetCode = 0;	
	}
	else 
		log_print( "reactor callback is NULL??????????????????????????" );
	
	if ( iEndFlag )
	{
		while ( (pQNode = de_queue( &( pOwnerReactor->stSocketRemoveQ ) ) ) )
		{
			pRemoveSocket = CONTAINER_OF_QUEUE( pQNode, CRemoveSocket );
			
			if ( remove_engine_socket( pRemoveSocket->pReactor->pEngine, pRemoveSocket->pSocket->iSocketId ) >= 0 )
			{
				net_close_socket( pRemoveSocket->pSocket );
				pRemoveSocket->pSocket = 0;
			
			}
			else 
				log_print( "%s %s:%d !if ( remove_engine_socket( pRe failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
			
			mem_free( pRemoveSocket );
			pRemoveSocket = NULL;
		}	
	}
	
	unlock( &( pOwnerReactor->Locker ) );
	
	return iRetCode;
}


//register reactor data callback.
int32_t register_reactor_callback( CReactor *pReactor, reactor_callback_t callback, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( pReactor > 0 && callback )
	{
		lock( &( pReactor->Locker ) );

		pReactor->fReactorCallback = callback;
		pReactor->pUserData = pUserData;
		
		iRetCode = 0;
		
		unlock( &( pReactor->Locker ) );
	}
	else 
		log_print( "%s %s:%d !if ( iReactorId > 0 && callback ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return iRetCode;	
}

//create reactor.
CReactor *net_reactor( void )
{
	CReactor *pRetCode = NULL;
	CReactor *pNewReactor = NULL;
	
	pNewReactor = mem_malloc( sizeof( *pNewReactor ) );
	if ( pNewReactor )
	{
		memset( pNewReactor, 0x00, sizeof( *pNewReactor ) );
		
		if ( init_queue( &( pNewReactor->stSocketRemoveQ ) ) >= 0 )
		{
			if ( set_queue_water_level( &(pNewReactor->stSocketRemoveQ), 1024 * 100 ) >= 0 )
			{
				if ( init_mutex( &( pNewReactor->Locker ) ) >= 0 )
				{
					pNewReactor->pEngine = create_engine(  );
					if ( pNewReactor->pEngine )
					{
						log_print( "create reactor ok..................." );
						pRetCode = pNewReactor;
					}
					else 
						log_print( "%s %s:%d !if ( pNewReactor->iEn failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
				}		
			}	
		}
	
		if ( NULL == pRetCode )
		{
			log_print( "%s %s:%d !if ( 0 == iRetCode ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );

			if ( pNewReactor->pEngine )
			{
				destroy_engine( pNewReactor->pEngine );
				pNewReactor->pEngine = NULL;
			}
			
			mem_free( pNewReactor );
			pNewReactor = NULL;
		}	
	}
	else 
		log_print( "%s %s:%d !if ( pNewReactor ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	
	return pRetCode;
}

//destroy reactor.
void net_close_reactor( CReactor *pReactor )
{	
	if ( pReactor )
	{	
		lock( &( pReactor->Locker ) );
		
		//close net engine.
		destroy_engine( pReactor->pEngine );
		pReactor->pEngine = 0;
		
		mem_free( pReactor );
		pReactor = NULL;
		
		unlock( &( pReactor->Locker ) );
	}
	else 
		log_print( "%s %s:%d !if ( iReactorId ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
}

//add reactor socket.
int32_t add_reactor_socket( CReactor *pReactor, CSocket *pSocket, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( pReactor && pSocket )
	{
		pSocket->pUserData = pUserData;
		pSocket->pOwnerReactor = pReactor;
			
		if ( add_engine_socket( pReactor->pEngine, pSocket->iSocketId, pSocket ) >= 0 )
		{
			//log_print( "add socket to engine ok, pSocket->iSocketId->%d...............", pSocket->iSocketId );
			iRetCode = 0;
		}
		else 
			log_print( "%s %s:%d !if ( add_engine_socket( pReactor->iE failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s:%d !if ( iReactorId && iSocketId ) failed????????????????", __FILE__, __FUNCTION__, __LINE__ );
		
	
	return iRetCode;
}

//remove reactor socket.
int32_t remove_reactor_socket( CReactor *pReactor, CSocket *pSocket )
{
	int32_t iRetCode = -1;
	
	if ( pReactor && pSocket )
	{
		CRemoveSocket *pRemoveSocket = NULL;
		
		pRemoveSocket = mem_malloc( sizeof( *pRemoveSocket ) );
		if ( pRemoveSocket )
		{
			memset( pRemoveSocket, 0x00, sizeof( *pRemoveSocket ) );
			
			pRemoveSocket->pSocket = pSocket;
			pRemoveSocket->pReactor = pReactor;
			
			while ( en_queue( &( pReactor->stSocketRemoveQ ), &( pRemoveSocket->QNode ) ) < 0 )
			{
				os_sleep( 100 );				
			}
			
			iRetCode = 0;
		}
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

