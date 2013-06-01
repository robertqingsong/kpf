/********************************************************
 * file name: log.c
 * writer: robert
 * date time:2013-05-28 17:29
 * description: log api implementation.
 * *****************************************************/

/*******************************************************************************************
* header files include.
*******************************************************************************************/
#include "../inc/log.h"

#include "../inc/lock.h"

#include <stdarg.h>

//if log enabled.
static int32_t fg_iIsEnabled = 0;

//log type.
static C_LOG_TYPE fg_eLogType = LOG_TYPE_START;

//log mutex
static CMutex fg_LogMutex;

#if (KERNEL_DEV_SUPPORT)

//Kernel dev log api implementation.
/*******************************************************************************************
function name: enable_log
*input param: int32_t iIsEnable //if enable log or not.
*return value type: int32_t //0 success, -1 faile.
*description:to enable or release log.
********************************************************************************************/
int32_t enable_log( int32_t iIsEnable )
{
	int32_t iRetCode = -1;

	memset( &fg_LogMutex, 0x00, sizeof(fg_LogMutex) );
	if ( init_mutex( &fg_LogMutex ) >= 0 )
	{
		lock( &fg_LogMutex );

		fg_iIsEnabled = iIsEnable;
		iRetCode = 0;

		unlock( &fg_LogMutex );
	}

	return iRetCode;
}

int32_t set_log( const C_LOG_TYPE eLogType, const void *pParam, const int32_t iParamSize )
{
	int32_t iRetCode = -1;

	if ( fg_iIsEnabled )
	{
		lock( &fg_LogMutex );

		if ( LOG_TYPE_CONSOLE == eLogType )
		{
			fg_eLogType = eLogType;

			iRetCode = 0;
		}

		unlock( &fg_LogMutex );
	}

	return iRetCode;
}

int32_t log_print( const int8_t *pFormat, ... )
{
        int32_t iRetCode = -1;        
	
	if ( fg_iIsEnabled )
	{
		va_list args;
		int8_t pTempBuf[128] = { 0x00, };

		lock( &fg_LogMutex );

		va_start( args, pFormat );
		iRetCode = vsnprintf(pTempBuf, sizeof(pTempBuf), pFormat, args);
		va_end(args);

		switch ( fg_eLogType )
		{
		case LOG_TYPE_CONSOLE:
		{
			printk( KERN_DEBUG "%s\r\n", pTempBuf );	
		}break ;
		default:
		{

		}break ;
		}

		unlock( &fg_LogMutex );
	}

	return iRetCode;
}



#else

/******************************************************************************************
* file global variable definition.
******************************************************************************************/

//File log handler.
static FILE *pfg_FileLogHandler = NULL;
//Net log tcp socket id.
static int32_t fg_iNetLogHandler = -1;
//log locker
static CMutex fg_LogMutex;

/********************************************************************************************
*function name: enable_log
*input param: int32_t iIsEnable //if enable log or not.
*return value type: int32_t //0 success, -1 faile.
*description:to enable or release log.
********************************************************************************************/
int32_t enable_log( int32_t iIsEnable )
{
	int32_t iRetCode = -1;
	
	memset( &fg_LogMutex, 0x00, sizeof(fg_LogMutex) );
	if ( init_mutex( &fg_LogMutex ) >= 0 )
	{
		lock( &fg_LogMutex );

		fg_iIsEnabled = iIsEnable;

		if ( iIsEnable )
		{
			iRetCode = 0;	
		}
  		else ( !fg_iIsEnabled )
 		{
    			switch ( fg_eLogType )
    			{
    			case LOG_TYPE_FILE:
    			{
      				if ( pfg_FileLogHandler )
      				{
					fclose( pfg_FileLogHandler );
					pfg_FileLogHandler = NULL;

      				}
				iRetCode = 0;
   			}break ;
    			case LOG_TYPE_CONSOLE:
    			{

    			}break ;
  			case LOG_TYPE_NET:
   			{
      				if ( fg_iNetLogHandler >= 0 )
      				{	  
					//close net socket id.
	
	
      				}
      
    			}break ;
  			default:
    			{

    			}break;
			}
		}

		unlock( &fg_LogMutex );
	}

	return iRetCode;
}

int32_t set_log( const C_LOG_TYPE eLogType, const void *pParam, const int32_t iParamSize )
{
	int32_t iRetCode = -1;

	if ( fg_iIsEnabled )
	{
		lock( &fg_LogMutex );
		
  		fg_eLogType = eLogType;

  		switch ( eLogType )
  		{
  		case LOG_TYPE_FILE:
    		{
      			if ( !pfg_FileLogHandler && pParam )
      			{
				const int8_t *pLogFileName = (const int8_t *)pParam;

				pfg_FileLogHandler = fopen( pLogFileName, "wb" );
				if ( pfg_FileLogHandler )
	  				iRetCode = 0;
      			}
    		}break ;
  		case LOG_TYPE_CONSOLE:
    		{
     			iRetCode = 0;
    		}break ;
  		case LOG_TYPE_NET:
    		{

    		}break ;
  		default:
    		{

    		}break;	
		}

		unlock( &fg_LogMutex );
	}

  	return iRetCode;
}

int32_t log_print( const int8_t *pFormat, ... )
{
	int32_t iRetCode = -1;
	va_list args;
	int8_t pTempBuf[1024] = { 0x00, };
	
	if ( fg_iIsEnabled )
	{
		lock( &fg_LogMutex );

		va_start( args, pFormat );
		iRetCode = vsnprintf(pTempBuf, sizeof(pTempBuf), pFormat, args);
		va_end(args);

		switch ( fg_eLogType )
		{
		case LOG_TYPE_FILE:
	  	{
	    		if ( pfg_FileLogHandler )
	    		{
	      			fwrite( pTempBuf, 1, iRetCode, pfg_FileLogHandler );
	      			fflush( pfg_FileLogHandler );
	   		}
	  	}break ;
		case LOG_TYPE_CONSOLE:
	  	{
#if (__OS_WIN32__)
	   	 	fwrite( pTempBuf, 1, strlen(pTempBuf), stdout );
			fwrite( "\r\n", 1, 2, stdout );
	    		fflush( stdout );
	    
#endif
	  	}break ;
		case LOG_TYPE_NET:
	  	{

	  	}break ;
		default:
	  	{

	 	}break;
		}

		unlock( &fg_LogMutex );
	}

	return iRetCode;
}


#endif

