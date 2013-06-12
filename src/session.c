#include "../inc/session.h"

#include "../inc/mem_api.h"

//create session.
CSession *create_session( C_SESSION_TYPE eSessionType, const CSessionParam *pSessionParam )
{
	CSession *pRetCode = NULL;
	pRetCode = mem_malloc( sizeof( *pRetCode ) );
	if ( pRetCode )
	{
		memset( pRetCode, 0x00, sizeof(*pRetCode) );
		
		switch ( eSessionType )
		{
		case SESSION_TYPE_DGRAM_CLIENT:
		{
			CSocket *pUDPSocket = NULL;
			
			pUDPSocket = net_socket(  );
		}break ; 
		case SESSION_TYPE_STREAM_CLIENT:
		{
			
		}break ;
		case SESSION_TYPE_MULTICAST_LISTENER:
		{
			
		}break ;
		case SESSION_TYPE_DGRAM_SERVER:
		{
			
		}break ;
		case SESSION_TYPE_STREAM_SERVER:
		{
			
		}break ;
		case SESSION_TYPE_HTTP_CLIENT:
		{
			
		}break ;
		default:
		{
			
		}break ;	
		}
	}
	
	return pRetCode;	
}

//destroy session.
void destroy_session( CSession *pThis )
{
	
}

//set session business.
int32_t set_session_business( CSession *pThis, session_business_t business )
{
	int32_t iRetCode = -1;
	
	return iRetCode;	
}

//set session event.
int32_t set_session_event( CSession *pThis, session_event_t event )
{
	int32_t iRetCode = -1;
	
	return iRetCode;	
}

//set session reactor.
int32_t set_session_reactor( CSession *pThis, const CReactor *pReactor )
{
	int32_t iRetCode = -1;
	
	return iRetCode;
}
