#include "../inc/pine.h"

#include "../inc/lock.h"

#include "../inc/mem_api.h"

//smart type define.
typedef struct CSmart_t
{
	int32_t *pRefCount;
	void *pData;
	int32_t (*on_close)( void *pData );
}CSmart;

static CSmart *create_smart( void *pData, int32_t (*close_callback)( void *pSmart ) );

static int32_t copy_smart( CSmart *pDestSmart, CSmart *pSrcSmart );

static int32_t release_smart( CSmart *pSmart );

//file function declaration.
static int32_t on_pine_close( void *pData );

static int32_t on_pine_destory( CPine *pPine );

int32_t operator_init( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		memset( pPine, 0x00, sizeof(*pPine) );

		pPine->on_destory = on_pine_destory;

		pPine->pm_Base = create_smart( pPine, on_pine_close );
		if ( pPine->pm_Base )
			iRetCode = 0;
	}

	return iRetCode;
}

//copy pine
int32_t operator_den( CPine *pPineDest, CPine *pPineSrc )
{
	int32_t iRetCode = -1;

	if ( pPineSrc )
	{
		pPineDest = pPineSrc;

		if ( copy_smart( pPineDest->pm_Base, pPineSrc->pm_Base ) >= 0 )
			iRetCode = 0;
	}

	return iRetCode;
}

//release.
int32_t operator_release( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		iRetCode = release_smart( pPine->pm_Base );
	}

	return iRetCode;
}

static int32_t on_pine_destory( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		int32_t (*on_destory_child)( CPine *pPine );

		on_destory_child = ( int32_t (*)( CPine *) )((unsigned char *)pPine + sizeof(*pPine));
		if ( on_destory_child )
			on_destory_child( pPine );
		mem_free( pPine );
		pPine = NULL;

		iRetCode = 0;
	}

	return iRetCode;
}

static int32_t on_pine_close( void *pData )
{
	int32_t iRetCode = -1;
	CSmart *pSmart = (CSmart *)pData;

	if ( pSmart )
	{
		CPine *pPine = (CPine *)pSmart->pData;

		if ( pPine && pPine->on_destory )
			iRetCode = pPine->on_destory( pPine );

		if ( pSmart->pRefCount )
		{
			mem_free( pSmart->pRefCount );
			pSmart->pRefCount = NULL;
		}
		
		mem_free( pSmart );
		pSmart = NULL;
	}

	return iRetCode;
}

static CSmart *create_smart( void *pData, int32_t (*close_callback)( void *pData ) )
{
	CSmart *pRetCode = NULL;

	pRetCode = mem_malloc( sizeof(*pRetCode) );
	if ( pRetCode )
	{
		memset( pRetCode, 0x00, sizeof(*pRetCode) );

		pRetCode->pData = pData;
		pRetCode->on_close = close_callback;
		pRetCode->pRefCount = mem_malloc( sizeof( *( pRetCode->pRefCount ) ) );
		if ( !pRetCode->pRefCount )
		{
			mem_free( pRetCode );
			pRetCode = NULL;
		}
		else
		{
			*(pRetCode->pRefCount) = 1;
		}
	}

	return pRetCode;
}

static int32_t copy_smart( CSmart *pDestSmart, CSmart *pSrcSmart )
{
	int32_t iRetCode = -1;

	if ( pSrcSmart )
	{
		pDestSmart = pSrcSmart;

		if ( pDestSmart->pRefCount )
		{
			*(pDestSmart->pRefCount)++;

			iRetCode = 0;
		}
	}

	return iRetCode;
}

static int32_t release_smart( CSmart *pSmart )
{
	int32_t iRetCode = -1;
	
	if ( pSmart )
	{
		if ( pSmart->pRefCount )
		{
			*(pSmart->pRefCount)--;

			if ( *(pSmart->pRefCount) <= 0 )
			{
				iRetCode = pSmart->on_close( pSmart );
			}
		}
	}

	return iRetCode;
}

