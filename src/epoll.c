#include "../inc/epoll.h"

#include "../inc/net_api.h"

#include "../inc/mem_api.h"

int32u_t create_engine( void )
{
	int32u_t iRetCode = 0;
	
	if ( callback )
	{
		CNetEngine *pNewEngine = NULL;
		
		pNewEngine = mem_malloc( sizeof( *pNewEngine ) + sizeof( void * ) );
		if ( pNewEngine )
		{
			memset( pNewEngine, 0x00, sizeof( *pNewEngine ) + sizeof( void * ) );
		
			
			iRetCode = (((int8u_t *)pNewEngine) + sizeof( *pNewEngine ));
		}
	}
	
	return iRetCode;	
}

//register reactor callback.
int32_t register_engine_callback( int32u_t iEngineId, engine_callback_t callback )
{
	int32_t iRetCode = -1;
	
	if ( iEngineId && callback )
	{
		CNetEngine *pEngine = NULL;
		
		pEngine = iEngineId - sizeof( *pEngine );
		
		pEngine->pEngineCallback = callback;
		
		iRetCode = 0;
	}
	
	return iRetCode;	
}

void destroy_engine( int32u_t iEngineId )
{
	if ( iEngineId )
	{
		CNetEngine *pEngine = NULL;
		
		pEngine = iEngineId - sizeof( *pEngine );

		//close epoll or kqueue id.
				
		//release memory.		
		mem_free( pEngine );
		pEngine = NULL;
	}
}

//add engine socket.
int32_t add_engine_socket( int32u_t iEngineId, int32_t iSocketId, void *pUserData )
{
	int32_t iRetCode = -1;
	
	if ( iEngineId && iSocketId >= 0 )
	{
		CNetEngine *pEngine = NULL;
		
		pEngine = iEngineId - sizeof( *pEngine );
		
		//add socket to ...
		
		iRetCode = 0;
	}
	
	return iRetCode;	
}

//remove engine socket.
int32_t remove_engine_socket( int32u_t iEngineId, int32_t iSocketId )
{
	int32_t iRetCode = -1;
	
	if ( iEngineId && iSocketId >= 0 )
	{
		CNetEngine *pEngine = NULL;
		
		pEngine = iEngineId - sizeof( *pEngine );
		
		//add socket to ...
		
		iRetCode = 0;
	}
	
	return iRetCode;	
}
