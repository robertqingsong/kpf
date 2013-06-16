#include "datacache.h"

#include "log.h"

int main( int argc, char **argv )
{
	CDataCache stDataCache;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( init_datacache( &stDataCache, 1000 * 5 ) >= 0 )
	{
		int8u_t pTempDataBuf[1000 * 5 - 12];
		int32_t iRetCode = -1;
		
		iRetCode = write_datacache( &stDataCache, pTempDataBuf, sizeof( pTempDataBuf ) );
		log_print( "write(1000 * 5 - 12): iRetCode-->%d\r\n", iRetCode );
		
		iRetCode = write_datacache( &stDataCache, pTempDataBuf, 13 );
		log_print( "write(13): iRetCode-->%d\r\n", iRetCode );
		
		iRetCode = read_datacache( &stDataCache, pTempDataBuf, 12 );
		log_print( "read(12): iRetCode-->%d\r\n", iRetCode );
		
		iRetCode = write_datacache( &stDataCache, pTempDataBuf, 12 );
		log_print( "write(12): iRetCode-->%d\r\n", iRetCode );
		
		iRetCode = read_datacache( &stDataCache, pTempDataBuf, 1000 * 5 );
		log_print( "read(1000 * 5): iRetCode-->%d\r\n", iRetCode );
		
		iRetCode = read_datacache( &stDataCache, pTempDataBuf, 1000 * 5 );
		log_print( "read(1000 * 5): iRetCode-->%d\r\n", iRetCode );
		
		reset_datacache( &stDataCache );	
	}

	return 0;	
}
