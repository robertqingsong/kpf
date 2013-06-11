#include "log.h"

#include "pine.h"

#include "oal_api.h"
#include "net_api.h"

#include "mem_api.h"

#define LOCAL_PORT  (9001)

CSocket *iUDPServiceSocketId = NULL;

int32_t udp_reactor_callback( CReactor *pReactor, CSocket *pSocket, void *pUserData )
{
	char recvBuf[1024] = { 0x00, };
		CNetAddr stPeerAddr;
		
		int32_t iRetCode = net_recvfrom( pSocket, recvBuf, sizeof(recvBuf), &stPeerAddr );
		if ( iRetCode > 0 )
		{
			char respbuf[1024 * 2] = { 0x00, };
			
			log_print( recvBuf );
			
			sprintf( respbuf, "%s:%d hi, %s", stPeerAddr.pIP, stPeerAddr.iPort, recvBuf );
			
			log_print( respbuf );
			
			iRetCode = net_sendto( pSocket, respbuf, strlen(respbuf) + 1, &stPeerAddr );
			if ( SOCKET_ERROR == iRetCode )
			{
				perror( "send response msg failed:" );
				log_print( "close socket, pSocket->iSocketId-->%d....................", pSocket->iSocketId );
				remove_reactor_socket( pReactor, pSocket );
				pSocket = NULL;
			}
			else 
				log_print( "send udp response ok................" );
		}
		else 
		{
			log_print( "recv from failed?????????????????" );
			if ( SOCKET_ERROR == iRetCode )
			{
				log_print( "close socket, pSocket->iSocketId-->%d....................", pSocket->iSocketId );
				remove_reactor_socket( pReactor, pSocket );
				pSocket = NULL;
			}	
		}
		
	return 0;	
}

int main( int argc, char **argv )
{
	int32_t iRetCode = -1;
	CReactor *iUDPReactorId = NULL;
	int8_t pLocalIP[32] = "192.168.1.100";
	CThread *acceptTid = NULL;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( strlen( pLocalIP ) <= 0 )
		if ( net_get_local_ip( pLocalIP, sizeof( pLocalIP ) ) < 0 )
			return iRetCode;
	
	log_print( "%s %s:%d local ip-->%s\r\n", __FILE__, __FUNCTION__, __LINE__, pLocalIP );

	if ( init_pine_system(  ) < 0 )
		return iRetCode;
	
	iUDPReactorId = net_reactor(  );
	if ( iUDPReactorId )
	{
		if ( register_reactor_callback( iUDPReactorId, udp_reactor_callback, NULL ) < 0 )
		{	
			log_print( "%s %s:%d register reactor callback failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
			return iRetCode;
		}
		
		iUDPServiceSocketId = net_socket( SOCKET_TYPE_DGRAM, 0 );
		if ( iUDPServiceSocketId )
		{
			CNetAddr addr;
			
			memset( &addr, 0x00, sizeof(addr) );
			memcpy( addr.pIP, pLocalIP, strlen(pLocalIP) );
			addr.iPort = LOCAL_PORT;
			
			if ( net_bind( iUDPServiceSocketId, &addr ) < 0 )
			{
				log_print( "%s %s:%d socket bind failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;	
			}
			else 
				log_print( "bind ok.............." );
			
			if ( net_set_socket( iUDPServiceSocketId, SOCKET_OPTION_REUSE_ADDRESS, NULL, 0 ) < 0 )
			{
				log_print( "%s %s:%d set socket reuse address failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;
			}
			else 
				log_print( "set reuse address ok..............." );
			
			if ( add_reactor_socket( iUDPReactorId, iUDPServiceSocketId, NULL ) < 0 )
			{
				log_print( "%s %s:%d set add reactor socket failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;	
			}
			else 
				log_print( "add reactor socket ok................." );
			
			if ( net_set_socket( iUDPServiceSocketId, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) < 0 )
			{
				log_print( "%s %s:%d set socket none block failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;
			}
			else 
				log_print( "set socket none blocking ok....................." );
			
			
			log_print( "udp server is running.............." );
		}
		
		while ( 1 )
			os_sleep( 1000 );
			
		net_close_reactor( iUDPReactorId );
	}

	return 0;	
}
