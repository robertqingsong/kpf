#ifndef __DGRAM_SESSION_H__
#define __DGRAM_SESSION_H__

#include "config.h"
#include "session.h"

#if defined(__cplusplus)
extern "C"
{
#endif

CSession *get_dgram_session( void );

#if defined(__cplusplus)
}
#endif

#endif/* __DGRAM_SESSION_H__ */
