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
	int32_t m_iHasChild;
}CPine;

#define EXTENDS_PINE \
	int32_t (*on_destory)( CPine *pPine ); \
	void *pm_Base;	\
	int32_t m_iHasChild;

#define CHILD_ADDR_OF_PINE(pine) \
	((int8u_t *)( ((int8u_t *)pine) + sizeof(CPine) ))

//init pine.
int32_t operator_init( CPine *pPine );

//copy pine
CPine *operator_den( CPine *pPineSrc );

//release.
int32_t operator_release( CPine *pPine );

//

#if defined(__cplusplus)
}
#endif

#endif/* __PINE_H__ */


