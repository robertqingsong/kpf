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
int32u_t create_engine( reactor_callback_t callback );

//destroy engine socket.
void destroy_engine( int32u_t iEngineId );

//add engine socket.
int32_t add_engine_socket( int32u_t iEngineId, int32_t iSocketId, void *pUserData );

//remove engine socket.
int32_t remove_engine_socket( int32u_t iEngineId, int32_t iSocketId );

#endif

#if defined(__cplusplus)
}
#endif

#endif/* __EPOLL_H__ */
