#include "../inc/stream_service_session.h"


	//init.
static int32_t init( struct CSession_t *pThis, const CSessionParam *pSessionParam )
{
	int32_t iRetCode = -1;
	
	CSocket *pTCPSocket = NULL;

	
	if ( !pThis || !pSessionParam )
		return iRetCode;
	
	pTCPSocket = net_socket( SOCKET_TYPE_STREAM, 0 );
	if ( pTCPSocket )
	{
		CNetAddr addr;
		char tempBuf[1024] = { 0x00, };
		int32_t iLen = 0;
				
		memset( &addr, 0x00, sizeof(addr) );
				
		memcpy( addr.pIP, pSessionParam->pIP, strlen(pSessionParam->pIP) );
		addr.iPort = pSessionParam->iPort;
	
		if ( net_bind( pTCPSocket, &addr ) >= 0 )
		{
			if ( net_set_socket( pTCPSocket, SOCKET_OPTION_REUSE_ADDRESS, NULL, 0 ) >= 0 )
			{
				if ( add_reactor_socket( pThis->pOwnerReactor, pTCPSocket, pThis ) >= 0 )
				{
					if ( net_set_socket( pTCPSocket, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
					{
						if ( net_listen( pTCPSocket, 1024 * 100 ) >= 0 )
						{
							pThis->pSocket = pTCPSocket;
							
							iRetCode = 0;
						}
					}	
					else 
						log_print( "set none blocking socket failed???????????????" );
				}
				else 
					log_print( "add reactor socket failed???????????????????" );
			}
		}
		else 
			log_print( "connect failed????????????????" );
	}
	
	return iRetCode;
}

//release.
static void release( struct CSession_t *pThis )
{
	if ( remove_reactor_socket( pThis->pOwnerReactor, pThis->pSocket ) >= 0 )
	{
		pThis->pSocket = NULL;		
	}
	else 
		log_print( "!if ( remove_reactor_socket failed??????????????????????????" );
}

//input data.
static int32_t handle_input( const struct CSession_t *pThis, 
				 const CSocket *pSocket )
{
	int32_t iRetCode = -1;
	
	log_print( "handle_input:----------------->" );
	
	if ( pThis && pSocket )
	{
		if ( pSocket == pThis->pSocket )
		{
			int8u_t pRecvBuf[1024] = { 0x00, };
			CNetAddr stClientAddr;
		
			CSocket *pNewSocket = net_accept( pSocket, &stClientAddr );
		
			if ( pNewSocket )
			{
				if ( pThis->handle_event )
				{
					CEventParam stEventParam;
				
					memset( &stEventParam, 0x00, sizeof(stEventParam) );
					stEventParam.pSocket = pNewSocket;
					stEventParam.stNetAddr = stClientAddr;
					iRetCode = pThis->handle_event( pThis, EVENT_ACCEPT_NEW_STREAM_CLIENT_NOTIFY, &stEventParam, sizeof(stEventParam) );
				}
			}
			else 
			{
				if ( SOCKET_ERROR == iRetCode )
				{
					if ( pThis->handle_event )
					{
						pThis->handle_event( pThis, EVENT_ACCEPT_ERROR, NULL, 0 );
					}
				}
			}
		}
		else 
		{
			//read socket data.
			int8u_t pRecvBuf[1024] = { 0x00, };
		
			iRetCode = net_recv( pSocket, pRecvBuf, sizeof(pRecvBuf) );
		
			if ( iRetCode > 0 )
			{
				if ( pThis->handle_business )
				{
					pThis->handle_business( pThis, pSocket, pRecvBuf, iRetCode, NULL );	
				}
			}
			else 
			{
				if ( SOCKET_ERROR == iRetCode )
				{
					if ( pThis->handle_event )
					{
						CEventParam stEventParam;
						
						memset( &stEventParam, 0x00, sizeof(stEventParam) );
						stEventParam.pSocket = pSocket;
						pThis->handle_event( pThis, EVENT_READ_ERROR, &stEventParam, sizeof(stEventParam) );
					}
				}
			}	
		}
	}
	
	return iRetCode;
}

//output data.
static int32_t handle_output( const struct CSession_t *pThis, 
				  const int8u_t *pOutDatabuf, 
				  const int32_t iOutDataLen, 
				  const CNetAddr *pNetAddr )
{
	int32_t iRetCode = -1;
	
	if ( pThis && pOutDatabuf && iOutDataLen > 0 )
	{
		iRetCode = net_send( pThis->pSocket, pOutDatabuf, iOutDataLen );
	}
	
	return iRetCode;
}

CSession *get_stream_service_session( void )
{
	CSession *pRetCode = NULL;

	pRetCode = mem_malloc( sizeof( *pRetCode ) );
	if ( pRetCode )
	{
		memset( pRetCode, 0x00, sizeof( *pRetCode ) );
		
		pRetCode->init = init;
		pRetCode->release = release;
		pRetCode->handle_input = handle_input;
		pRetCode->handle_output = handle_output;
	}
	
	return pRetCode;	
}
