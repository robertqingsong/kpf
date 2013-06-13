#include "log.h"

#include "oal_api.h"
#include "net_api.h"

#include "mem_api.h"

#include "session.h"

int32_t stream_session_business( const struct CSession_t *pThis, 
					 const CSocket *pSocket, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen, 
				    const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;

	log_print( "stream_service_session_business:------------------------>" );	
	
	if ( pInData )
	{
		int8_t pRespBuf[1024] = { 0x00, };
		
		log_print( pInData );
		
		sprintf( pRespBuf, "hi, %s", pInData );
		net_send( pSocket, pRespBuf, strlen(pRespBuf) + 1 );
		
		iRetCode = 0;
	}
	
	log_print( "stream_service_session_business<------------------------end" );	
	
	return iRetCode;	
}

int32_t stream_session_event( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize )
{
	int32_t iRetCode = -1;
	
	log_print( "stream_servie_session_event:------------------------>" );
	
	if ( !pEventParam )
		return iRetCode;
	
	if ( EVENT_ACCEPT_NEW_STREAM_CLIENT_NOTIFY == eEvent )
	{
		if ( net_set_socket( pEventParam->pSocket, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
		{
				
			log_print( "accept_task: start to add reactor socket................" );
			if ( add_session_socket( pThis, pEventParam->pSocket ) >= 0 )
			{
				log_print( "client:%s:%d", pEventParam->stNetAddr.pIP, pEventParam->stNetAddr.iPort );
				iRetCode = net_send( pEventParam->pSocket, "welcome\r\n", 9 );
				if ( iRetCode > 0 )
					log_print( "accept_task: have send %d bytes.", iRetCode );		
			}	
			else 
				log_print( "accept_task: add reactor socket failed??????????????????????" );		
		}
		else 
			log_print( "accept_task: set none blocking socket failed????????????????????????" );	
	}	
	
	log_print( "stream_service_session_event<------------------------end" );	
	
	return iRetCode;	
}

int main( int argc, char **argv )
{
	CSession *pSession = NULL;
	CSessionParam stSessionParam;
	const int8_t *pPeerIP = "192.168.1.100";
	const int16u_t iPeerPort = 9001;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( init_session(  ) < 0 )
		return -1;
	
	log_print( "init_session ok.................." );
		
	memset( &stSessionParam, 0x00, sizeof(stSessionParam) );
	memcpy( stSessionParam.pIP, pPeerIP, strlen(pPeerIP) + 1 );
	stSessionParam.iPort = iPeerPort;
	
	log_print( "start to create session............" );
	pSession = create_session( SESSION_TYPE_STREAM_SERVER, &stSessionParam );
	
	if ( pSession )
	{
		log_print( "create tcp session ok................" );
		if ( set_session_business( pSession, stream_session_business ) >= 0 )
		{
			if ( set_session_event( pSession, stream_session_event ) >= 0 )
			{
				log_print( "stream server is running..............." );
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
