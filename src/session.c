#include "../inc/session.h"

#include "../inc/mem_api.h"

#include "../inc/lock.h"

#include "../inc/dgram_session.h"

#include "../inc/stream_session.h"
#include "../inc/dgram_service_session.h"

#include "../inc/stream_service_session.h"

#include "../inc/http_session.h"

#include "../inc/pine.h"

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
	case SESSION_TYPE_STREAM_CLIENT:
	{
		pRetCode = get_stream_session(  );
	}break ;
	case SESSION_TYPE_DGRAM_SERVER:
	{
		pRetCode = get_dgram_service_session(  );
	}break ;
	case SESSION_TYPE_STREAM_SERVER:
	{
		pRetCode = get_stream_service_session(  );
	}break ;
	case SESSION_TYPE_MULTICAST_LISTENER:
	{
		
	}break ;
	
	case SESSION_TYPE_HTTP_CLIENT:
	{
		pRetCode = get_http_session(  );
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
	{
		log_print( "pReactor-->%u, pSocket-->%u", pReactor, pSocket );
		
		return iRetCode;
	}
		
	pSession = (CSession *)pSocket->pUserData;
	if ( pSession )
	{
		if ( pSession->handle_input )
			iRetCode = pSession->handle_input( pSession, pSocket );
		else 
			log_print( "common session handle_input is NULL." );
	}
	else 
		log_print( "!if ( pSession ) failed?????????????????????" );
	
	return iRetCode;	
}

//init session.
int32_t init_session( void )
{
	int32_t iRetCode = -1;
	
	if( !( fg_SessionManager.iInitFlag ) )
	{
		if ( init_pine_system(  ) >= 0 )
		{
			if ( init_mutex( &( fg_SessionManager.Locker ) ) >= 0 )
			{
				lock( &( fg_SessionManager.Locker ) );
			
				fg_SessionManager.iInitFlag = 1;
				
				iRetCode = 0;
			
				unlock( &( fg_SessionManager.Locker ) );
			}
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
		
		release_pine_system(  );		
		
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
		case SESSION_TYPE_STREAM_CLIENT:
		case SESSION_TYPE_MULTICAST_LISTENER:
		case SESSION_TYPE_DGRAM_SERVER:
		case SESSION_TYPE_STREAM_SERVER:
		case SESSION_TYPE_HTTP_CLIENT:
		{
			CReactor *pReactor = get_session_reactor( eSessionType );
			
			if ( pReactor )
			{
				pNewSession->pOwnerReactor = pReactor;
				
				if ( pNewSession->init )
				{
					pNewSession->stSessionParam = *pSessionParam;
					if ( pNewSession->init( pNewSession, pSessionParam ) >= 0 )
						pRetCode = pNewSession;	
				}
			}
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
		if ( pThis->release )
			pThis->release( pThis );
			
		mem_free( pThis );
		pThis = NULL;
	}
}

//set session business.
int32_t set_session_business( CSession *pThis, session_business_t business, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( pThis && business )
	{
		pThis->handle_business = business;
		pThis->pUserData = pUserData;
		
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
int32_t send_session_data( const CSession *pThis, const int8u_t *pData, const int32_t iDataLen, const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;

	if ( pThis && pData && iDataLen > 0 )
	{
		if ( pThis->handle_output )
			iRetCode = pThis->handle_output( pThis, pData, iDataLen, pNetAddr );
		else 
			log_print( "session handle_output function is NULL??????????????????" );
	}
	else 
		log_print( "send session data cond failed??????????????????" );
	
	return iRetCode;	
}

//add session socket.
int32_t add_session_socket( CSession *pThis, CSocket *pSocket )
{
	int32_t iRetCode = -1;
	
	if ( pThis && pSocket )
	{
		pSocket->pUserData = pThis;
		iRetCode = add_reactor_socket( pThis->pOwnerReactor, pSocket, pThis );
	}
	
	return iRetCode;	
}

//remove session socket.
int32_t remove_session_socket( CSession *pThis, const CSocket *pSocket )
{
	int32_t iRetCode = -1;
	
	if ( pThis && pSocket )
	{
		if ( pThis->pSocket == pSocket )
			pThis->pSocket = NULL;
		iRetCode = remove_reactor_socket( pThis->pOwnerReactor, pSocket );
	}
	
	return iRetCode;	
}

