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


#if defined(__cplusplus)
extern "C"
{
#endif

//socket type.
typedef enum
{
	SOCKET_TYPE_STREAM = 1, 
	SOCKET_TYPE_DGRAM
}C_SOCKET_TYPE;

//socket property.
typedef enum
{
	SOCKET_OPTION_NONE_BLOCK, 
	SOCKET_OPTION_REUSE_ADDRESS
}C_SOCKET_OPTION;

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
	int32_t i;
}CSocketParam;

typedef int32_t (*engine_callback_t)( int32_t iSocketId, void *pUserData );

//net engine.select, epoll, kqueue.
typedef struct CNetEngine_t
{
	int32_t iEngineId;//epoll id, or kqueue id.
	
	int32_t iIsRunning;
	int32u_t iEngineTid;//engine thread id.
	
	engine_callback_t pEngineCallback;//point to engine callback.
}CNetEngine;

typedef int32_t (*reactor_callback_t)( int32u_t iReactorId, int32_t iSocketId, void *pUserData );

//ip address to int.
int64u_t net_ip2n( const int8_t *pIP );

//get domain's ip address.
int32_t net_get_domain_ip( const int8_t *pDomainName, int8_t *pIP, const int32_t iIPBufLen );

//n to ip.
int32_t net_n2ip( const int64u_t inIP, int8_t *pIP, const int32_t iIPBufLen );

//create socket.
int32u_t net_socket( const C_SOCKET_TYPE eSocketType, const int32_t iIsIPv6 );

//close socket.
void net_close_socket( const int32u_t iSocketId );

//bind socket and address.
int32_t net_bind( const int32u_t iSocketId, const CNetAddr *pNetAddr );

//bind socket and address.
int32_t net_connect( const int32u_t iSocketId, const CNetAddr *pNetAddr );

//set socket property.
int32_t net_set_socket( const int32u_t iSocketId, 
								const C_SOCKET_OPTION eOption, const CSocketParam *pSocketParam, const int32_t iParamSize );
							
//init reactor.
int32_t init_reactor( void );

//release reactor.
void release_reactor( void );
	
//create reactor.
int32u_t net_reactor( void );

//register reactor data callback.
int32_t register_reactor_callback( int32u_t iReactorId, reactor_callback_t callback, void *pUserData );

//destroy reactor.
void net_close_reactor( int32u_t iReactorId );

//add reactor socket.
int32_t add_reactor_socket( int32u_t iReactorId, int32u_t iSocketId, void *pUserData );

//remove reactor socket.
int32_t remove_reactor_socket( int32u_t iReactorId, int32u_t iSocketId );

#if defined(__cplusplus)
}
#endif

#endif/* __NET_API_H__ */
