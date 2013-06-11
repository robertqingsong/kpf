#include "log.h"

#include "pine.h"

#include "oal_api.h"
#include "net_api.h"

#include "mem_api.h"

#include "queue.h"

static CQueue fg_AcceptQ;

typedef struct CAcceptInfo_t
{
	int32u_t iReactorId;
	
	int32u_t iListenSocketId;

	
	CQueueNode QNode;
}CAcceptInfo;

CAcceptInfo fg_AcceptInfo[100];
int counter = 0;

int32_t tcp_reactor_callback( int32u_t iReactorId, int32u_t iSocketId, void *pUserData )
{
			CNetAddr stClientAddr;
			CSocket *iNewSocketId = NULL;
			int32_t iRetCode = -1;

			memset( &stClientAddr, 0x00, sizeof(stClientAddr) );
			
			log_print( "tcp_reactor_callback:------------------------------>" );
			iNewSocketId = net_accept( iSocketId, &stClientAddr );

			//log_print( "\r\n\r\nListen Socket Id-->\r\n" );
			//net_test_socket( iSocketId );		
			//log_print( "\r\n\r\nClient Socket Id-->\r\n" );
			//net_test_socket( iNewSocketId );			
			
			if ( !iNewSocketId )
				return -1;
			
			log_print( "accept new client return..............." );
			log_print( "iNewSocketId->iSocketId-->%d", iNewSocketId->iSocketId );
			if ( net_set_socket( iNewSocketId, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
			{
				
				log_print( "accept_task: start to add reactor socket................" );
				if ( add_reactor_socket( iReactorId, iNewSocketId, NULL ) >= 0 )
				{
					log_print( "accept_task: add reactor socket ok...................." );

		
					iRetCode = net_send( iNewSocketId, "welcome\r\n", 9 );
					if ( iRetCode > 0 )
						log_print( "accept_task: have send %d bytes.", iRetCode );		
				}	
				else 
					log_print( "accept_task: add reactor socket failed??????????????????????" );		
			}
			else 
				log_print( "accept_task: set none blocking socket failed????????????????????????" );
			
			log_print( "tcp_reactor_callback:<------------------------------" );
	
	return 0;
}

int32_t main( int32_t argc, int8_t **argv )
{
	int32_t iRetCode = -1;
	CReactor *iTCPReactorId = NULL;
	int8_t pLocalIP[32] = "192.168.1.100";
	const int32_t LOCAL_PORT = 9000;
	CThread *acceptTid = NULL;
	
	enable_log( 1 );
	set_log( LOG_TYPE_CONSOLE, NULL, 0 );
	
	if ( strlen( pLocalIP ) <= 0 )
		if ( net_get_local_ip( pLocalIP, sizeof( pLocalIP ) ) < 0 )
			return iRetCode;
	
	log_print( "%s %s:%d local ip-->%s\r\n", __FILE__, __FUNCTION__, __LINE__, pLocalIP );

	if ( init_pine_system(  ) < 0 )
		return iRetCode;
	
	iTCPReactorId = net_reactor(  );
	if ( iTCPReactorId )
	{
		CSocket *iTCPServiceSocketId = NULL;
		
		if ( register_reactor_callback( iTCPReactorId, tcp_reactor_callback, NULL ) < 0 )
		{	
			log_print( "%s %s:%d register reactor callback failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
			return iRetCode;
		}
			
		iTCPServiceSocketId = net_socket( SOCKET_TYPE_STREAM, 0 );
		if ( iTCPServiceSocketId )
		{	
			CNetAddr addr;
			
			memset( &addr, 0x00, sizeof(addr) );
			memcpy( addr.pIP, pLocalIP, strlen(pLocalIP) );
			addr.iPort = LOCAL_PORT;
			
			if ( net_bind( iTCPServiceSocketId, &addr ) < 0 )
			{
				log_print( "%s %s:%d socket bind failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;	
			}
			else 
				log_print( "bind ok.............." );
			
			if ( net_set_socket( iTCPServiceSocketId, SOCKET_OPTION_REUSE_ADDRESS, NULL, 0 ) < 0 )
			{
				log_print( "%s %s:%d set socket reuse address failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;
			}
			else 
				log_print( "set reuse address ok..............." );
			
			if ( add_reactor_socket( iTCPReactorId, iTCPServiceSocketId, NULL ) < 0 )
			{
				log_print( "%s %s:%d set add reactor socket failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;	
			}
			else 
				log_print( "add reactor socket ok................." );
			
			if ( net_set_socket( iTCPServiceSocketId, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) < 0 )
			{
				log_print( "%s %s:%d set socket none block failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;
			}
			else 
				log_print( "set socket none blocking ok....................." );
			
			if ( net_listen( iTCPServiceSocketId, 1000 ) < 0 )
			{
				log_print( "%s %s:%d listen socket failed????????????????????", __FILE__, __FUNCTION__, __LINE__ );
				
				return iRetCode;	
			}
			else 
				log_print( "listen socket ok...................." );
			
			log_print( "tcp server is running.............." );
		}
		else 
			log_print( "%s %s:%d socket create failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
		log_print( "start server wait.................." );
		while ( 1 )
			os_sleep( 1000 );
		
		net_close_reactor( iTCPReactorId );
	}
	else 
		log_print( "%s %s:%d create reactor failed???????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
	
	release_pine_system(  );
		
	return iRetCode;
}
