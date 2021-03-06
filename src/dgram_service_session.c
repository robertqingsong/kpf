#include "../inc/dgram_service_session.h"

//init.
static int32_t init( struct CSession_t *pThis, const CSessionParam *pSessionParam )
{
	int32_t iRetCode = -1;
	
	CSocket *pUDPSocket = NULL;
	
	if ( !pThis || !pSessionParam )
		return iRetCode;
	
	pUDPSocket = net_socket( SOCKET_TYPE_DGRAM, 0 );
	if ( pUDPSocket )
	{
		CNetAddr stLocalAddr;
		char tempBuf[1024] = { 0x00, };
		int32_t iLen = 0;
				
		memset( &stLocalAddr, 0x00, sizeof(stLocalAddr) );
			
		memcpy( stLocalAddr.pIP, pSessionParam->pLocalIP, strlen(pSessionParam->pLocalIP) );
		stLocalAddr.iPort = pSessionParam->iLocalPort;

		if ( net_bind( pUDPSocket, &stLocalAddr ) >= 0 )
		{
			if ( add_reactor_socket( pThis->pOwnerReactor, pUDPSocket, pThis ) >= 0 )
			{
				if ( net_set_socket( pUDPSocket, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
				{
					pThis->pSocket = pUDPSocket;
							
					iRetCode = 0;
				}	
			}
		}
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
	
	if ( pThis && pSocket )
	{
		int8u_t pRecvBuf[1024] = { 0x00, };
		CNetAddr stPeerAddr;
		
		iRetCode = net_recvfrom( pSocket, pRecvBuf, sizeof(pRecvBuf), &stPeerAddr );
		
		if ( iRetCode > 0 )
		{
			if ( pThis->handle_business )
			{
				pThis->handle_business( pThis, pSocket, pRecvBuf, iRetCode, &stPeerAddr, pThis->pUserData );
			}
		}
		else 
		{
			if ( SOCKET_ERROR == iRetCode )
			{
				if ( pThis->handle_event )
				{
					pThis->handle_event( pThis, EVENT_READ_ERROR, NULL, 0, pThis->pUserData );
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
		iRetCode = net_sendto( pThis->pSocket, pOutDatabuf, iOutDataLen, NULL );
	}
	
	return iRetCode;
}


CSession *get_dgram_service_session( void )
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
