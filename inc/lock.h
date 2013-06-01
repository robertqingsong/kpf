/********************************************************
 * file name: lock.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: lock for framework kpf
 * *****************************************************/

#ifndef __LOCK_H__
#define __LOCK_H__

/*********************************************************************************
* header files include.
*********************************************************************************/
#include "config.h"
#include "typedefs.h"


#if defined(__cplusplus)
extern "C"
{
#endif

#if (__OS_LINUX__)

#if (KERNEL_DEV_SUPPORT)

#include <linux/spinlock.h>

/*********************************************************************************
* type definitions.
**********************************************************************************/
typedef spinlock_t CMutex;

/*********************************************************************************
* mutex api declaration.
**********************************************************************************/

/*********************************************************************************
* function name: init_mutex
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to init a mutex.
**********************************************************************************/
int32_t init_mutex( CMutex *pMutex );

/*********************************************************************************
* function name: lock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to lcok a mutex.
**********************************************************************************/
int32_t lock( CMutex *pMutex );

/*********************************************************************************
* function name: unlock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to unlock a mutex.
**********************************************************************************/
int32_t unlock( CMutex *pMutex );

/*********************************************************************************
* function name: try_lock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to try lock a mutex.
**********************************************************************************/
int32_t try_lock( CMutex *pMutex );

#else

#include <pthread.h>

/*********************************************************************************
* type definitions.
**********************************************************************************/
typedef pthread_mutex_t CMutex;

/*********************************************************************************
* mutex api declaration.
**********************************************************************************/

/*********************************************************************************
* function name: init_mutex
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to init a mutex.
**********************************************************************************/
int32_t init_mutex( CMutex *pMutex );

/*********************************************************************************
* function name: lock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to lcok a mutex.
**********************************************************************************/
int32_t lock( CMutex *pMutex );

/*********************************************************************************
* function name: unlock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to unlock a mutex.
**********************************************************************************/
int32_t unlock( CMutex *pMutex );

/*********************************************************************************
* function name: try_lock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to try lock a mutex.
**********************************************************************************/
int32_t try_lock( CMutex *pMutex );

#endif

#endif

#if defined(__cplusplus)
}
#endif

#endif/* __LOCK_H__ */
