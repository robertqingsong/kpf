/********************************************************
 * file name: epoll.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: linux epoll for framework kpf
 * *****************************************************/

#ifndef __EPOLL_H__
#define __EPOLL_H__

#include "config.h"

#include "typedefs.h"

#include "net_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#if (__OS_LINUX__)

//create engine.
CNetEngine *create_engine( void );

//destroy engine socket.
void destroy_engine( CNetEngine *pEngine );

//add engine socket.
int32_t add_engine_socket( CNetEngine *EngineId, int32_t iSocketId, void *pUserData );

//remove engine socket.
int32_t remove_engine_socket( CNetEngine *pEngine, int32_t iSocketId );

#endif

#if defined(__cplusplus)
}
#endif

#endif/* __EPOLL_H__ */
