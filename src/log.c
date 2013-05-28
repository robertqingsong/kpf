/********************************************************
 * file name: log.c
 * writer: robert
 * date time:2013-05-28 17:29
 * description: log api implementation.
 * *****************************************************/

#include "log.h"

#include <stdarg.h>

int32_t enable_log( int32_t iIsEnable )
{
  int32_t iRetCode = -1;

  return iRetCode;
}

int32_t set_log( const C_LOG_TYPE eLogType, const void *pParam, const int32_t iParamSize )
{
  int32_t iRetCode = -1;

  return iRetCode;
}

int32_t log_print( const int8_t *pFormat, ... )
{
	int32_t iRetCode = -1;
	va_list args;
	int8_t pTempBuf[1024] = { 0x00, };
	
	va_start(args, pFormat);
	iRetCode = vsnprintf(pTempBuf, sizeof(pTempBuf), pFormat, args);
	va_end(args);
	fwrite( pTempBuf, 1, iRetCode, stdout );
	fwrite( "\r\n", 1, 2, stdout );
	
	return iRetCode;
}
