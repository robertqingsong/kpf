#ifndef __PINE_H__
#define __PINE_H__

#include "config.h"
#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//pine define.
typedef struct CPine_t
{
	int32_t (*on_destory)( struct CPine_t *pPine );
	void *pm_Base;
}CPine;

#define EXTENDS_PINE \
	int32_t (*on_destory)( CPine *pPine ); \
	void *pm_Base;

//init pine.
int32_t operator_init( CPine *pPine );

//copy pine
int32_t operator_den( CPine *pPineDest, CPine *pPineSrc );

//release.
int32_t operator_release( CPine *pPine );

//

#if defined(__cplusplus)
}
#endif

#endif/* __PINE_H__ */


