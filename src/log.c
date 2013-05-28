/********************************************************
 * file name: log.c
 * writer: robert
 * date time:2013-05-28 17:29
 * description: log api implementation.
 * *****************************************************/

#include "log.h"

int32_t log_print( const int8_t *pFormat, ... )
{
	int32_t iRetCode = -1;
	va_list args;
	int8_t pTempbuf[1024] = { 0x00, };
	
    va_start(args, pFormat);
    iRetCode = vsprintf(pTempbuf, pFormat, args);
    va_end(args);
	fwrite( pTempBuf, 1, iRetCode, stdout );
	
	return iRetCode;
}