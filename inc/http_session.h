#ifndef __HTTP_SESSION_H__
#define __HTTP_SESSION_H__

#include "config.h"

#include "session.h"

#if defined(__cplusplus)
extern "C"
{
#endif

CSession *get_http_session( void );

#if defined(__cplusplus)
}
#endif

#endif/* __HTTP_SESSION_H__ */
