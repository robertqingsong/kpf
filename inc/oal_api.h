/********************************************************
 * file name: oal_api.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: oal_api for framework kpf
 * *****************************************************/

#ifndef __OAL_API_H__
#define __OAL_API_H__

#include "config.h"
#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum
{
	OS_THREAD_PRIORITY_LOW, 
	OS_THREAD_PRIORITY_NORMAL, 
	OS_THREAD_PRIORITY_HIGH
	
}C_THREAD_PRIORITY;

typedef void *(*os_thread_t)( void *pParam );

//get handle
int32_t get_handle( void );

//os thread api.
int32u_t os_thread_create( os_thread_t thread, void *pParam, 
				C_THREAD_PRIORITY eThreadPriority, int32_t iStackSize );

//quit thread.
int32_t os_thread_wait( int32u_t iThreadId );

//sleep api.
void os_sleep( int32_t iTimeInMileSeconds );

#if defined(__cplusplus)
}
#endif

#endif/* __OAL_API_H__ */
