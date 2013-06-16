#include "log.h"

#include "oal_api.h"
#include "net_api.h"

#include "mem_api.h"

#include "session.h"

FILE *pHandle = NULL;

int32_t stream_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;
	
	if ( pInData )
	{
		CHttpData *pHttpData = (CHttpData *)pInData;
		
		if ( pHttpData )
		{
			//fwrite( pHttpData->pHttpData, 1, pHttpData->iCurrentHttpDataLen, pHandle );
			//fflush( pHandle );
			
			//log_print( "pHttpData->iCurrentHttpDataLen-->%d", pHttpData->iCurrentHttpDataLen );
			fwrite( pHttpData->pHttpData, 1, pHttpData->iCurrentHttpDataLen, stdout );
			fflush( stdout );
		}
		
		iRetCode = 0;
	}
	
	return iRetCode;	
}

int32_t stream_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize )
{
	int32_t iRetCode = -1;
	
	
	if ( pThis && pEventParam )
	{
		if ( EVENT_READ_ERROR == eEvent )
		{
			//log_print( "read http data error??????????????????????????" );
			
			remove_session_socket( pThis, pEventParam->pSocket );
		}
	}
	
	return iRetCode;	
}


int main( int argc, char **argv )
{
	CSession *pSession = NULL;
	CSessionParam stSessionParam;
	const int8_t *pPeerIP = "www.lelsen.sinaapp.com";
	//const int8_t *pPeerIP = "www.baidu.com";
	const int8_t *pURL = "/business/api/punch.php";
	//const int8_t *pURL = "/index.html";
	const int16u_t iPeerPort = 80;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	pHandle = fopen( "./log.txt", "wb" );
	if ( !pHandle )
		return -1;
	
	if ( init_session(  ) < 0 )
		return -1;
	
	log_print( "init_session ok.................." );
		
	memset( &stSessionParam, 0x00, sizeof(stSessionParam) );
	memcpy( stSessionParam.pIP, pPeerIP, strlen(pPeerIP) + 1 );
	stSessionParam.iPort = iPeerPort;
	
	log_print( "start to create session............" );
	pSession = create_session( SESSION_TYPE_HTTP_CLIENT, &stSessionParam );
	
	if ( pSession )
	{
		log_print( "create tcp session ok................" );
		if ( set_session_business( pSession, stream_session_business ) >= 0 )
		{
			if ( set_session_event( pSession, stream_session_event ) >= 0 )
			{
				CHttpData stHttpData;
				
				memset( &stHttpData, 0x00, sizeof(stHttpData) );
				stHttpData.pHost = pPeerIP;
				stHttpData.pURL = pURL;
				stHttpData.eHttpAction = SESSION_HTTP_GET;

				while ( 1 )
				{
					int i, iNSent = -1;
					
					//printf( "Please enter 1 to send http request:\r\n" );
					i = 0;
				//	scanf( "%d", &i );
					//if ( i == 1 )
						iNSent = send_session_data( pSession, &stHttpData, sizeof(stHttpData), NULL );
						
					os_sleep( 1000 * 2);
				}
			}
		}
	}
	else 
		log_print( "create udp session failed????????????????????????" );
		
	while ( 1 )
		os_sleep( 1000 );
		
	destroy_session( pSession );
	pSession = NULL;
		
	release_session(  );	
	
	return 0;	
}
