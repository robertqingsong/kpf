#ifndef __SESSION_H__
#define __SESSION_H__

#include "config.h"
#include "net_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum CSessionType_t
{
	SESSION_TYPE_DGRAM_CLIENT, 
	SESSION_TYPE_STREAM_CLIENT,
	SESSION_TYPE_MULTICAST_LISTENER, 
	SESSION_TYPE_DGRAM_SERVER, 
	SESSION_TYPE_STREAM_SERVER, 
	
	SESSION_TYPE_HTTP_CLIENT, 
}C_SESSION_TYPE;

typedef enum CSessionEvent_t
{
	EVENT_READ_ERROR, 
	EVENT_WRITE_ERROR
}C_SESSION_EVENT;

typedef struct CEventParam_t
{
	int i;
}CEventParam;

typedef struct CSessionParam_t
{
	int8_t pIP[32];
	int16u_t iPort;

	
}CSessionParam;

typedef int32_t (*session_business_t)( const struct CSession_t *pThis, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen );

typedef int32_t (*session_event_t)( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize );

typedef struct CSession_t
{
	//init.
	int32_t (*init)( struct CSession_t *pThis, const CSessionParam *pParam );
	//release.
	void (*release)( struct CSession_t *pThis );

	//input data.
	int32_t (*handle_input)( const struct CSession_t *pThis, 
				 const CSocket *pSocket );

	//output data.
	int32_t (*handle_output)( const struct CSession_t *pThis, 
				  const CSocket *pSocket, 
				  const int8u_t *pOutDatabuf, 
				  const int32_t iOutDataLen );

	//business.
	int32_t (*handle_business)( const struct CSession_t *pThis, 
				    const int8u_t *pInData, 
				    const int32_t iInDataLen );

	//event.
	int32_t (*handle_event)( const struct CSession_t *pThis, 
				 const C_SESSION_EVENT eEvent, 
				 const CEventParam *pEventParam, 
				 const int32_t iEventParamSize );
				 
	//session data define.
	CReactor *pOwnerReactor;
	CSocket *pSocket;
}CSession;

//create session.
CSession *create_session( C_SESSION_TYPE eSessionType, const CSessionParam *pSessionParam );

//destroy session.
void destroy_session( CSession *pThis );

//set session business.
int32_t set_session_business( CSession *pThis, session_business_t business );

//set session event.
int32_t set_session_event( CSession *pThis, session_event_t event );

//set session reactor.
int32_t set_session_reactor( CSession *pThis, const CReactor *pReactor );

#if defined(__cplusplus)
}
#endif

#endif/* __SESSION_H__ */

