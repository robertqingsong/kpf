#include "log.h"

#include "oal_api.h"
#include "net_api.h"

#include "mem_api.h"

#include "session.h"

int32_t dgram_service_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;

	log_print( "dgram_service session_business:------------------------>" );	
	
	if ( pInData )
	{
		char pRespBuf[1024] = { 0x00, };
		
		log_print( pInData );
		
		sprintf( pRespBuf, "%s:%d %s\r\n", pNetAddr->pIP, pNetAddr->iPort, pInData );

		net_sendto( pSocket, pRespBuf, strlen(pRespBuf) + 1, pNetAddr );
		
		iRetCode = 0;
	}
	
	log_print( "dgram_service session_business<------------------------end" );	
	
	return iRetCode;	
}

int32_t dgram_service_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize )
{
	int32_t iRetCode = -1;
	
	log_print( "dgram_service session_event:------------------------>" );
	
	log_print( "dgram_service session_event<------------------------end" );	
	
	return iRetCode;	
}

int main( int argc, char **argv )
{
	CSession *pUDPSession = NULL;
	CSessionParam stUDPSessionParam;
	const int8_t *pLocalIP = "192.168.1.100";
	const int8_t *pPeerIP = "192.168.1.100";
	const int16u_t iLocalPort = 9002;
	const int16u_t iPeerPort = 9001;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( init_session(  ) < 0 )
		return -1;
	
	log_print( "init_session ok.................." );
		
	memset( &stUDPSessionParam, 0x00, sizeof(stUDPSessionParam) );
	memcpy( stUDPSessionParam.pIP, pPeerIP, strlen(pPeerIP) + 1 );
	memcpy( stUDPSessionParam.pLocalIP, pLocalIP, strlen(pLocalIP) + 1 );
	stUDPSessionParam.iPort = iPeerPort;
	stUDPSessionParam.iLocalPort = iLocalPort;
	
	log_print( "start to create session............" );
	pUDPSession = create_session( SESSION_TYPE_DGRAM_SERVER, &stUDPSessionParam );
	
	if ( pUDPSession )
	{
		log_print( "create udp session ok................" );
		if ( set_session_business( pUDPSession, dgram_service_session_business, NULL ) >= 0 )
		{
			if ( set_session_event( pUDPSession, dgram_service_session_event ) >= 0 )
			{
				const int8_t pWords[1024] = { 0x00, };
				
				while ( fgets( pWords, 1024, stdin ) )
				{
					int32_t iNSent = send_session_data( pUDPSession, pWords, strlen(pWords) + 1, NULL );
					log_print( "have send %d bytes", iNSent );
				}
			}
		}
	}
	else 
		log_print( "create udp session failed????????????????????????" );
		
	while ( 1 )
		os_sleep( 1000 );
		
	destroy_session( pUDPSession );
	pUDPSession = NULL;
		
	release_session(  );	
	
	return 0;	
}
