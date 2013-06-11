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

#include "pine.h"

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

typedef struct CThread_t
{
	EXTENDS_PINE

	os_thread_t thread;
	C_THREAD_PRIORITY ePriority;
	void *pParam;

	int64u_t iTid;

	int32_t iQuitFlag;

}CThread;

//get handle
int32_t get_handle( void );

//os thread api.
CThread *os_thread_create( os_thread_t thread, void *pParam, 
				C_THREAD_PRIORITY eThreadPriority, int32_t iStackSize );

//quit thread.
int32_t os_thread_wait( CThread *pThread );

//sleep api.
void os_sleep( int32_t iTimeInMileSeconds );

#if defined(__cplusplus)
}
#endif

#endif/* __OAL_API_H__ */
