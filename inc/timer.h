#ifndef __TIMER_H__
#define __TIMER_H__

#include "config.h"
#include "typedefs.h"


#if defined(__cplusplus)
extern "C"
{
#endif

typedef int32_t (*timer_callback_t)( int32u_t iTimerId, void *pUserData );

//init timer.
int32_t init_timer( void );

//register timer.
int32u_t register_timer( int32_t iTimeInMileSeconds, timer_callback_t callback, void *pUserData );

//unregister timer.
int32_t unregister_timer( int32u_t iTimerId );

//destroy timer.
void release_timer( void );

#if defined(__cplusplus)
}
#endif

#endif/* __TIMER_H__ */

