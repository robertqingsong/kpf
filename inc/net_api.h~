/********************************************************
 * file name: net_api.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: net_api for framework kpf
 * *****************************************************/

#ifndef __NET_API_H__
#define __NET_API_H__

#include "config.h"
#include "typedefs.h"

#include "oal_api.h"

#include "lock.h"

#include "queue.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//socket type.
typedef enum
{
	SOCKET_TYPE_STREAM = 1, 
	SOCKET_TYPE_DGRAM, 
	SOCKET_TYPE_MULTICAST
}C_SOCKET_TYPE;

//socket property.
typedef enum
{
	SOCKET_OPTION_NONE_BLOCK, 
	SOCKET_OPTION_REUSE_ADDRESS, 
	SOCKET_OPTION_SET_MULTICAST
}C_SOCKET_OPTION;

#define SOCKET_ERROR  (-1)
#define SOCKET_ERROR_AGAIN  (-2)

//network address define.
typedef struct CNetAddr_t
{
	int8_t pIP[32];
	int16u_t iPort;
	
	int16u_t iLocalPort;
}CNetAddr;

//socket param.
typedef struct CSocketParam_t
{
	int8_t *pIP[32];
	int16u_t iPort;
	
}CSocketParam;

//net engine.select, epoll, kqueue.
typedef struct CNetEngine_t
{
	int32_t iEngineId;//epoll id, or kqueue id.
	
	int32_t iIsRunning;
	CThread *pEngineThread;//engine thread id.

}CNetEngine;

struct CReactor_t;
struct CSocket_t;
typedef int32_t (*reactor_callback_t)( const struct CReactor_t *pReactorId, const struct CSocket_t *pSocketId, void *pUserData );

//net reactor.
typedef struct CReactor_t
{	
	CNetEngine *pEngine;//net engine which is used to handle all sockets event in this reactor.
	
	reactor_callback_t fReactorCallback;
	
	CQueue stSocketRemoveQ;
	
	CMutex Locker;
	
	void *pUserData;
}CReactor;

//socket define.
typedef struct CSocket_t
{
	int32_t iSocketId;
	
	CReactor *pOwnerReactor;
	
	void *pUserData;
}CSocket;

//get local ip.
int32_t net_get_local_ip( int8_t *pIPBuf, const int32_t iIPBufLen );

//ip address to int.
int64u_t net_ip2n( const int8_t *pIP );

//get domain's ip address.
int32_t net_get_domain_ip( const int8_t *pDomainName, int8_t *pIP, const int32_t iIPBufLen );

//n to ip.
int32_t net_n2ip( const int64u_t inIP, int8_t *pIP, const int32_t iIPBufLen );

//create socket.
CSocket *net_socket( const C_SOCKET_TYPE eSocketType, const int32_t iIsIPv6 );

//close socket.
void net_close_socket( CSocket *pSocketId );

//bind socket and address.
int32_t net_bind( const CSocket *pSocketId, const CNetAddr *pNetAddr );

//listen socket.
int32_t net_listen( const CSocket *pSocketId, const int32_t iListenCount );

//accept socket.
CSocket *net_accept( const CSocket *pSocketId, CNetAddr *pClientAddr );

//bind socket and address.
int32_t net_connect( const CSocket *pSocketId, const CNetAddr *pNetAddr );

//set socket property.
int32_t net_set_socket( const CSocket *pSocketId, 
								const C_SOCKET_OPTION eOption, const CSocketParam *pSocketParam, const int32_t iParamSize );
								
//send tcp data.
int32_t net_send( const CSocket *pSocketId, const int8u_t *pData, const int32_t iDataLen );

//receive tcp data.
int32_t net_recv( const CSocket *pSocketId, int8u_t *pRecvDataBuf, const int32_t iRecvBufLen );

//send udp data.
int32_t net_sendto( const CSocket *pSocketId, const int8u_t *pData, const int32_t iDataLen, CNetAddr *pPeerAddr );

//receive upp data.
int32_t net_recvfrom( const CSocket *pSocketId, int8u_t *pRecvDataBuf, const int32_t iRecvBufLen, CNetAddr *pPeerAddr );
	
//reactor api.
int32_t init_reactor( void );	
void release_reactor( void );	

//create reactor.
CReactor *net_reactor( void );

//register reactor data callback.
int32_t register_reactor_callback( CReactor *pReactor, reactor_callback_t callback, void *pUserData );

//destroy reactor.
void net_close_reactor( CReactor *pReactorId );

//add reactor socket.
int32_t add_reactor_socket( CReactor *pReactorId, CSocket *pSocketId, void *pUserData );

//remove reactor socket.
int32_t remove_reactor_socket( CReactor *pReactorId, CSocket *iSocketId );

//network to host.
int16u_t net_n2hs( int16u_t iInData );

//host to network.
int16u_t net_h2ns( int16u_t iInData );

#if defined(__cplusplus)
}
#endif

#endif/* __NET_API_H__ */
