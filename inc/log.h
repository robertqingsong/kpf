/********************************************************
 * file name: log.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: log for framework kpf
 * *****************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*******************************************************
* Function Name: log_print
* Input Param: const int8_t *pFormat, ...
* Return Type: int32_t 0:ok, -1:failed
* Description: show log.
* Writer: robert
********************************************************/
int32_t log_print( const int8_t *pFormat, ... );

#if defined(__cplusplus)
}
#endif

#endif/* __LOG_H__ */
