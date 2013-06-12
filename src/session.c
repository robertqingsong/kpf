#include "../inc/session.h"

#include "../inc/mem_api.h"

//create session.
CSession *create_session( C_SESSION_TYPE eSessionType, const CSessionParam *pSessionParam )
{
	CSession *pRetCode = NULL, *pNewSession = NULL;
	
	pNewSession = mem_malloc( sizeof( *pNewSession ) );
	if ( pNewSession )
	{
		memset( pNewSession, 0x00, sizeof(*pNewSession) );
		
		switch ( eSessionType )
		{
		case SESSION_TYPE_DGRAM_CLIENT:
		{
			CSocket *pUDPSocket = NULL;
			
			pUDPSocket = net_socket( SOCKET_TYPE_DGRAM, 0 );
			if ( pUDPSocket )
			{
				CNetAddr addr, stLocalAddr;
				char tempBuf[1024] = { 0x00, };
				int32_t iLen = 0;
			
				memset( &addr, 0x00, sizeof(addr) );
				memset( &stLocalAddr, 0x00, sizeof(stLocalAddr) );
				
				memcpy( addr.pIP, pSessionParam->pIP, strlen(pSessionParam->pIP) );
				addr.iPort = pSessionParam->iPort;
			
				memcpy( stLocalAddr.pIP, pLocalIP, strlen(pSessionParam->pLocalIP) );
				stLocalAddr.iPort = pSessionParam->iLocalPort;

				if ( net_bind( pUDPSocket, &stLocalAddr ) >= 0 )
				{
					if ( net_connect( pUDPSocket, &addr ) >= 0 )
					{
						if ( net_set_socket( pUDPSocket, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
						{
							pNewSession->pSocket = pUDPSocket;
							
							pRetCode = pNewSession;
						}
					}
				}
			}
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
		
		if ( NULL == pRetCode )
		{
			mem_free( pNewSession );
			pNewSession = NULL;
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
