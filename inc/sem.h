/********************************************************
 * file name: sem.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: sem for framework kpf
 * *****************************************************/

#ifndef __SEM_H__
#define __SEM_H__

#include "config.h"
#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#if (__OS_LINUX__)

#include <semaphore.h>

typedef sem_t CSem;

#endif

//init sem.
int32_t init_sem( CSem *pSem, int32_t iMaxValue );

//release sem.
void release_sem( CSem *pSem );

//wait sem.
int32_t wait_sem( CSem *pSem );

//post sem.
int32_t post_sem( CSem *pSem );

#if defined(__cplusplus)
}
#endif

#endif/* __SEM_H__ */
