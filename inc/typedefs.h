/********************************************************
 * file name: typedefs.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: typedefs for framework kpf
 * *****************************************************/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "defines.h"

#ifdef (__cplusplus)
extern "C"
{
#endif

typedef char int8_t;
typedef unsigned char int8u_t;
typedef short int16_t;
typedef unsigned short int16u_t;
typedef int int32_t;
typedef unsigned int int32u_t;
typedef long long int64_t;
typedef unsigned long long int64u_t;
typedef float float32_t;
typedef double float64_t;
	
#ifdef (__cplusplus)
}
#endif

#endif/* __TYPEDEFS_H__ */
