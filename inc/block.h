/********************************************************
 * file name: block.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: block for framework kpf
 * *****************************************************/

#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "config.h"

#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//init block.
int32_t init_block( void );

//release block.
void release_block( void );

//create block.
void *block_create( int32_t iTimeInSeconds );

//destroy block.
int32_t block_destroy( void *pBlockHandle );

//request block current thread.
int32_t block_wait( void *pBlockHandle );

//block exiting.
int32_t block_exiting( void *pBlockHandle );

//release block.
int32_t block_post( void *pBlockHandle );

#if defined(__cplusplus)
}
#endif

#endif/* __BLOCK_H__ */
