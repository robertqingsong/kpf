#include "../inc/session.h"

#include "../inc/mem_api.h"

#include "../inc/lock.h"

#include "../inc/dgram_session.h"

typedef struct CSessionManager_t
{
	int32_t iInitFlag;
	
	CReactor *pSessionReactor[SESSION_TYPE_END];
	
	CMutex Locker;
}CSessionManager;

static CSessionManager fg_SessionManager = {
	0, 
 	{NULL, }, 
 		
};

static int32_t common_session_reactor_callback( const struct CReactor_t *pReactor, const struct CSocket_t *pSocket, void *pUserData );
static CReactor *get_session_reactor( C_SESSION_TYPE eSessionType )
{
	CReactor *pRetCode = NULL;
	
	lock( &( fg_SessionManager.Locker ) );
	
	pRetCode = fg_SessionManager.pSessionReactor[ eSessionType ];
	if ( !pRetCode )
	{
		pRetCode = net_reactor(  );
		//int32_t register_reactor_callback( CReactor *pReactor, reactor_callback_t callback, void *pUserData );
		if ( pRetCode )
		{
			if ( register_reactor_callback( pRetCode, common_session_reactor_callback, NULL ) < 0 )
			{
				net_close_reactor( pRetCode );
				
				pRetCode = NULL;
			}
			else 
				fg_SessionManager.pSessionReactor[ eSessionType ] = pRetCode;
		}
	}
	
	unlock( &( fg_SessionManager.Locker ) );
	
	return pRetCode;	
}

static CSession *get_session( C_SESSION_TYPE eSessionType )
{
	CSession *pRetCode = NULL;

	switch ( eSessionType )
	{
	case SESSION_TYPE_DGRAM_CLIENT:
	{
		pRetCode = get_dgram_session(  );
	}break ;
	default:
	{
		
	}break ;	
	}
	
	return pRetCode;	
}

static int32_t common_session_reactor_callback( const struct CReactor_t *pReactor, const struct CSocket_t *pSocket, void *pUserData )
{
	int32_t iRetCode = -1;

	CSession *pSession = NULL;
	
	if ( !pReactor || !pSocket )
		return iRetCode;
		
	pSession = (CSession *)pSocket->pUserData;
	if ( pSession )
	{
		if ( pSession->handle_input )
			iRetCode = pSession->handle_input( pSession, pSocket );
	}
	
	return iRetCode;	
}

//init session.
int32_t init_session( void )
{
	int32_t iRetCode = -1;
	
	if( !( fg_SessionManager.iInitFlag ) )
	{
		if ( init_mutex( &( fg_SessionManager.Locker ) ) >= 0 )
		{
			lock( &( fg_SessionManager.Locker ) );
			
			fg_SessionManager.iInitFlag = 1;
			
			unlock( &( fg_SessionManager.Locker ) );
		}
	}
	else 
		iRetCode = 0;
	
	return iRetCode;	
}

//release session.
void release_session( void )
{
	if ( fg_SessionManager.iInitFlag )
	{
		int32_t i = 0;
		
		lock( &( fg_SessionManager.Locker ) );

		for ( i = 0; i < SESSION_TYPE_END; i++ )
		{
			if ( fg_SessionManager.pSessionReactor[i] )
			{
				net_close_reactor( fg_SessionManager.pSessionReactor[i] );
				fg_SessionManager.pSessionReactor[i] = NULL;
			}
		}
		
		fg_SessionManager.iInitFlag = 0;
		
		unlock( &( fg_SessionManager.Locker ) );
	}
}

//create session.
CSession *create_session( C_SESSION_TYPE eSessionType, const CSessionParam *pSessionParam )
{
	CSession *pRetCode = NULL, *pNewSession = NULL;
	
	pNewSession = get_session( eSessionType );
	if ( pNewSession )
	{	
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
				CReactor *pDGRamReactor = get_session_reactor( eSessionType );
			
				if ( pDGRamReactor )
				{
					memset( &addr, 0x00, sizeof(addr) );
					memset( &stLocalAddr, 0x00, sizeof(stLocalAddr) );
				
					memcpy( addr.pIP, pSessionParam->pIP, strlen(pSessionParam->pIP) );
					addr.iPort = pSessionParam->iPort;
			
					memcpy( stLocalAddr.pIP, pSessionParam->pLocalIP, strlen(pSessionParam->pLocalIP) );
					stLocalAddr.iPort = pSessionParam->iLocalPort;

					if ( net_bind( pUDPSocket, &stLocalAddr ) >= 0 )
					{
						if ( net_connect( pUDPSocket, &addr ) >= 0 )
						{
							if ( add_reactor_socket( pDGRamReactor, pUDPSocket, pNewSession ) >= 0 )
							{
								if ( net_set_socket( pUDPSocket, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
								{
									pNewSession->pSocket = pUDPSocket;
									pNewSession->pOwnerReactor = pDGRamReactor;
							
									pRetCode = pNewSession;
								}	
							}
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
	if ( pThis )
	{
		if ( remove_reactor_socket( pThis->pOwnerReactor, pThis->pSocket ) >= 0 )
		{
			pThis->pSocket = NULL;		
		}
		else 
			log_print( "!if ( remove_reactor_socket failed??????????????????????????" );
			
		mem_free( pThis );
		pThis = NULL;
	}
}

//set session business.
int32_t set_session_business( CSession *pThis, session_business_t business )
{
	int32_t iRetCode = -1;
	
	if ( pThis && business )
	{
		pThis->handle_business = business;
		
		iRetCode = 0;	
	}
	
	return iRetCode;	
}

//set session event.
int32_t set_session_event( CSession *pThis, session_event_t event )
{
	int32_t iRetCode = -1;
	
	if ( pThis && event )
	{
		pThis->handle_event = event;
		
		iRetCode = 0;	
	}
	
	return iRetCode;	
}

//send data.
int32_t send_session_data( const CSession *pThis, const int8u_t *pData, const int32_t iDataLen )
{
	int32_t iRetCode = -1;

	if ( pThis && pData && iDataLen > 0 )
	{
		if ( pThis->handle_output )
			iRetCode = pThis->handle_output( pThis, pData, iDataLen );	
	}
	
	return iRetCode;	
}
