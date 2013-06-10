/********************************************************
 * file name: mem_api.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: mem_api for framework kpf
 * *****************************************************/

#ifndef __MEM_API_H__
#define __MEM_API_H__

#include "config.h"

#define IMPLEMENT_MEM_API_SUPPORT  (1)

#if defined(__cplusplus)
extern "C"
{
#endif


#if (__OS_LINUX__)

#if (KERNEL_DEV_SUPPORT)
#define mem_malloc kmalloc
#define mem_free kfree
#else

#if (IMPLEMENT_MEM_API_SUPPORT)

//malloc memory.
void *mem_malloc( int32_t iMemSize );

//free memory.
int32_t mem_free( void *pMem );

#else

#define mem_malloc  malloc
#define mem_free free
#endif

#endif

#endif

#if defined(__cplusplus)
}
#endif

#endif/* __MEM_API_H__ */
