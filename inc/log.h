/********************************************************
 * file name: log.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: log for framework kpf
 * *****************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include "typedefs.h"

#define LOG_SUPPORT  (1)

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

typedef enum
{
  LOG_TYPE_START, 

  LOG_TYPE_FILE, 
  LOG_TYPE_CONSOLE, 
  LOG_TYPE_NET, 

  LOG_TYPE_END
}C_LOG_TYPE;

int32_t enable_log( int32_t iIsEnable );

int32_t set_log( const C_LOG_TYPE eLogType, const void *pParam, const int32_t iParamSize );

int32_t log_print( const int8_t *pFormat, ... );

#if defined(__cplusplus)
}
#endif

#endif/* __LOG_H__ */
