#include "../inc/http_session.h"

#include "http_parser.h"

#include "../inc/mem_api.h"

static int32_t init_http_socket( struct CSession_t *pThis, const CSessionParam *pSessionParam )
{
	int32_t iRetCode = -1;
	CSocket *pTCPSocket = NULL;
	
	if ( pThis->pSocket )
	{
		iRetCode = 0;
		
		return iRetCode;
	}
		
	pTCPSocket = net_socket( SOCKET_TYPE_STREAM, 0 );
	if ( pTCPSocket )
	{
		CNetAddr addr;
		char tempBuf[1024] = { 0x00, };
		int32_t iLen = 0;
				
		memset( &addr, 0x00, sizeof(addr) );
				
		memcpy( addr.pIP, pSessionParam->pIP, strlen(pSessionParam->pIP) );
		addr.iPort = pSessionParam->iPort;
		
		if ( net_connect( pTCPSocket, &addr ) >= 0 )
		{
			if ( add_reactor_socket( pThis->pOwnerReactor, pTCPSocket, pThis ) >= 0 )
			{
				if ( net_set_socket( pTCPSocket, SOCKET_OPTION_NONE_BLOCK, NULL, 0 ) >= 0 )
				{
					pThis->pSocket = pTCPSocket;
						
					iRetCode = 0;
				}	
				else 
					log_print( "set none blocking socket failed???????????????" );
			}
			else 
				log_print( "add reactor socket failed???????????????????" );
		}
		else 
			log_print( "connect failed????????????????" );
	}
	else 
		log_print( "create tcp socket failed???????????????????????" );
	
	return iRetCode;	
}

	//init.
static int32_t init( struct CSession_t *pThis, const CSessionParam *pSessionParam )
{
	int32_t iRetCode = -1;
	
	if ( !pThis || !pSessionParam )
		return iRetCode;
	
	if ( init_mutex( &( pThis->Locker ) ) < 0 )
		return iRetCode;	
	
	lock( &( pThis->Locker ) );	
	
	iRetCode = init_http_socket( pThis, pSessionParam );
	
	unlock( &( pThis->Locker ) );
	
	return iRetCode;
}

//release.
static void release( struct CSession_t *pThis )
{
	CHttpData *pHttpData = (CHttpData *)pThis->pResultCode;
	
	lock( &( pThis->Locker ) );	
	
	if ( remove_reactor_socket( pThis->pOwnerReactor, pThis->pSocket ) >= 0 )
	{
		pThis->pSocket = NULL;		
	}
	else 
		log_print( "!if ( remove_reactor_socket failed??????????????????????????" );
		
	mem_free( pHttpData->pHttpData );
	pHttpData->pHttpData = NULL;
		
	unlock( &( pThis->Locker ) );
}

//------------------------------------------------------------------------------------------->


static int on_message_begin(http_parser* pParser) {

 // printf("\n***MESSAGE BEGIN***\n\n");

	//log_print( "pParser->content_length-->%d", pParser->content_length );  
  
  return 0;
}

static int on_headers_complete(http_parser* pParser) {

 // printf("\n***HEADERS COMPLETE***\n\n");
  
 // log_print( "pParser->content_length-->%d", pParser->content_length );  
  
  return 0;
}

static int on_message_complete(http_parser* pParser) {
	if ( pParser )
	{
		CHttpData *pHttpData = (CHttpData *)pParser->data;
		
		pHttpData->iEndFlag = 1;
	}

  //printf("\n***MESSAGE COMPLETE***\n\n");
  
 // log_print( "pParser->content_length-->%d", pParser->content_length );  

  
  return 0;
}

static int on_url(http_parser* pParser, const char* at, size_t length) {

  //printf("Url: %.*s\n", (int)length, at);
  
  //log_print( "pParser->content_length-->%d", pParser->content_length );  
  
  return 0;
}

static int on_header_field(http_parser* pParser, const char* at, size_t length) {

  //printf("Header field: %.*s\n", (int)length, at);
  
  // log_print( "pParser->content_length-->%d", pParser->content_length );  
   
  return 0;
}

static int on_header_value(http_parser* pParser, const char* at, size_t length) {

  //printf("Header value: %.*s\n", (int)length, at);
  
 // log_print( "pParser->content_length-->%d", pParser->content_length );  
  
  return 0;
}

static int on_body(http_parser* pParser, const char* at, size_t length) {

	//fwrite( at, 1, length, stdout );
	//fflush( stdout );
	
	CHttpData *pHttpData = (CHttpData *)pParser->data;
	if ( pHttpData )
	{
		if ( pHttpData->iCurrentHttpDataLen + length < pHttpData->iHttpDataBufLen )
		{
			memcpy( pHttpData->pHttpData + pHttpData->iCurrentHttpDataLen, at, length );
			pHttpData->iCurrentHttpDataLen += length;
			
			//log_print( "pHttpData->iCurrentHttpDataLen-->%d", pHttpData->iCurrentHttpDataLen );
		}	
	}

  return 0;
}

static int32_t start_parsing_http_data( CSession *pThis, const int8u_t *pDataBuf, const int32_t iDataLen )
{
	int32_t iRetCode = -1;
	
	if ( pThis && pDataBuf && iDataLen > 0 )
	{
		CHttpData *pHttpData = (CHttpData *)pThis->pResultCode;
		int32_t NParsed = -1;

		if ( !pHttpData )
		{
			pHttpData = pThis->pResultCode = mem_malloc( sizeof(CHttpData) );
			if ( pThis->pResultCode )
			{
				memset( pThis->pResultCode, 0x00, sizeof(CHttpData) );
			
				pHttpData->pSetting = mem_malloc( sizeof( http_parser_settings ) );
				pHttpData->pParser = mem_malloc( sizeof( http_parser ) );
				pHttpData->pHttpData = mem_malloc( 1024 * 20 );
				pHttpData->iHttpDataBufLen = 1024 * 20;
				pHttpData->iCurrentHttpDataLen = 0;
				
				if ( pHttpData->pSetting && pHttpData->pParser )
				{
					memset( pHttpData->pSetting, 0x00, sizeof( http_parser_settings ) );
					
					memset( pHttpData->pParser, 0x00, sizeof( http_parser ) );
				
			  		((http_parser_settings *)pHttpData->pSetting)->on_message_begin = on_message_begin;
  					((http_parser_settings *)pHttpData->pSetting)->on_url = on_url;
  					((http_parser_settings *)pHttpData->pSetting)->on_header_field = on_header_field;
  					((http_parser_settings *)pHttpData->pSetting)->on_header_value = on_header_value;
 					((http_parser_settings *)pHttpData->pSetting)->on_headers_complete = on_headers_complete;
  					((http_parser_settings *)pHttpData->pSetting)->on_body = on_body;
 					((http_parser_settings *)pHttpData->pSetting)->on_message_complete = on_message_complete;
 					
 					((http_parser *)pHttpData->pParser)->data = pHttpData;
 					
 					http_parser_init( pHttpData->pParser, HTTP_RESPONSE );
				}
				else 
				{
					log_print( "malloc memory for http data failed??????????????????" );
					if ( pHttpData->pSetting )
					{
						mem_free( pHttpData->pSetting );
						pHttpData->pSetting = NULL;	
					}
					
					if ( pHttpData->pParser )
					{
						mem_free( pHttpData->pParser );
						pHttpData->pParser = NULL;	
					}
					
					mem_free( pThis->pResultCode );
					pThis->pResultCode = NULL;
					
					return iRetCode;
				}
			}
			else 
			{
				return iRetCode;
			}
		}
		
 	 	NParsed = http_parser_execute( pHttpData->pParser, pHttpData->pSetting, pDataBuf, iDataLen );
 	 	if ( NParsed >= 0 )
 	 	{
 	 		//log_print( "execute end........" );
 	 		if ( pHttpData->iEndFlag )
 	 		{
 	 			
 	 			//fwrite( pHttpData->pHttpData, 1, pHttpData->iCurrentHttpDataLen, stdout );
 	 			//fflush( stdout );
 	 		

 	 			iRetCode = 0;	
 	 		}
 	 		//else 
 	 			//log_print( "pHttpData->iEndFlag-->%d", pHttpData->iEndFlag );
 	 	}
 	 	else 
 	 		log_print( "NParsed-->%d", NParsed );
	}
	else 
		log_print( "parsing http data, param is invalid????????????????????????" );
	
	return iRetCode;	
}
//---------------------------------------------------------------------------------------------->

//input data.
static int32_t handle_input( const struct CSession_t *pThis, 
				 const CSocket *pSocket )
{
	int32_t iRetCode = -1;
	
	if ( pThis && pSocket )
	{
		int8u_t pRecvBuf[1024] = { 0x00, };
		
		lock( &( pThis->Locker ) );
		
		do 
		{
			iRetCode = net_recv( pSocket, pRecvBuf, sizeof(pRecvBuf) );
			//log_print( "read %d bytes.", iRetCode );
			if ( iRetCode > 0 )
			{
				//fwrite( pRecvBuf, 1, iRetCode, stdout );
				//fflush( stdout );
				if ( start_parsing_http_data( pThis, pRecvBuf, iRetCode ) >= 0 )
				{
					if ( pThis->handle_business )
					{
						CHttpData *pHttpData = (CHttpData *)pThis->pResultCode;
 	 			
						//log_print( "http data notify................" );
						pThis->handle_business( pThis, pSocket, pThis->pResultCode, sizeof(CHttpData), NULL, pThis->pUserData );
					
						if ( pHttpData )
						{
							pHttpData->iCurrentHttpDataLen = 0;
 	 						pHttpData->iEndFlag = 0;
 	 						http_parser_init( pHttpData->pParser, HTTP_RESPONSE );
 	 					}
					}
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
					
						//log_print( "http server close connection............" );
						pThis->handle_event( pThis, EVENT_READ_ERROR, &stEventParam, sizeof(stEventParam), pThis->pUserData );
					}	
				}
			}
		}while ( iRetCode > 0 );
		
		unlock( &( pThis->Locker ) );
	}
	
	return iRetCode;
}

static int32_t get_http_send_string( const CHttpData *pHttpData, int8_t *pOutDataBuf, const int32_t iOutBufSize )
{
	int32_t iRetCode = -1;
	
	if ( pHttpData && pOutDataBuf && iOutBufSize > 0 )
	{
		switch ( pHttpData->eHttpAction )
		{
		case SESSION_HTTP_GET:
		{
			snprintf( pOutDataBuf, iOutBufSize, 
						 "GET %s HTTP/1.1\r\n"
         			 "Host: %s\r\n"
         			 "Keep-Alive: 300\r\n"
         			 "Connection: keep-alive\r\n"
         			 "\r\n", 
         			 pHttpData->pURL, 
         			 pHttpData->pHost );
         			 
#if 0
						 "GET %s HTTP/1.1\r\n"
         			 "Host: %s\r\n"
         			 "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"
         			 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
         			 "Accept-Language: en-us,en;q=0.5\r\n"
         			 "Accept-Encoding: gzip,deflate\r\n"
         			 "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
         			 "Keep-Alive: 300\r\n"
         			 "Connection: keep-alive\r\n"
         			 "\r\n"
#endif
			
			iRetCode = strlen( pOutDataBuf );
		}break ;
		default:
		{
			
		}break ;	
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
		lock( &( pThis->Locker ) );
		
		if ( init_http_socket( pThis, &( pThis->stSessionParam ) ) >= 0 )
		{
			CHttpData *pHttpData = (CHttpData *)pOutDatabuf;
			int8_t pSendBuf[1024] = { 0x00, };
			
			int32_t iSendLen = get_http_send_string( pHttpData, pSendBuf, sizeof(pSendBuf) );
			
			if ( iSendLen > 0 )
			{
				//log_print( pSendBuf );
				
				iRetCode = net_send( pThis->pSocket, pSendBuf, iSendLen );
				//log_print( "send: %d bytes", iRetCode );
			}
			else 
				log_print( "Send Len < 0????????????????" );
		}
		else 
			log_print( "inti http socket failed????????????" );
		
		unlock( &( pThis->Locker ) );
	}
	else 
		log_print( "send http data cond failed?????????????" );
	
	return iRetCode;
}

CSession *get_http_session( void )
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
