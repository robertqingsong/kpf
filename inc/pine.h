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
	int32_t m_iSaved;
}CPine;

#define EXTENDS_PINE \
	CPine PineSuper;

#define CHILD_ADDR_OF_PINE(pine) \
	((int8u_t *)( ((int8u_t *)pine) + sizeof(CPine) ))

//init pine.
int32_t pine_init( CPine *pPine );

//copy pine
CPine *pine_den( CPine *pPineSrc );

//release.
int32_t pine_release( CPine *pPine );

//

#if defined(__cplusplus)
}
#endif

#endif/* __PINE_H__ */


