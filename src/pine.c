#include "../inc/pine.h"

#include "../inc/lock.h"

#include "../inc/mem_api.h"

#include "../inc/log.h"

//smart type define.
typedef struct CSmart_t
{
	int32_t iRefCount;
	void *pData;
	int32_t (*on_close)( void *pData );
}CSmart;

static CSmart *create_smart( void *pData, int32_t (*close_callback)( void *pSmart ) );

static int32_t retain_smart( CSmart *pSrcSmart );

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
CPine *operator_den( CPine *pPineSrc )
{
	CPine *pRetCode = NULL;

	if ( pPineSrc )
	{
		pRetCode = pPineSrc;

		if ( retain_smart( pPineSrc->pm_Base ) < 0 )
			pRetCode = NULL;
	}

	return pRetCode;
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

	//log_print( "on_pine_destory:--------------------->" );
	if ( pPine )
	{
		int32_t (*on_destory_child)( CPine *pPine ) = NULL;
		
		if ( pPine->m_iHasChild )
		{
			memcpy( &on_destory_child, CHILD_ADDR_OF_PINE(pPine), sizeof(on_destory_child) );
			if ( on_destory_child )
			{
	//			log_print( "on_destory_child-->%u", on_destory_child );
				on_destory_child( pPine );
			}
		}

		iRetCode = 0;
	}

	//log_print( "on_pine_destory<-----------------------" );

	return iRetCode;
}

static int32_t on_pine_close( void *pData )
{
	int32_t iRetCode = -1;
	CSmart *pSmart = (CSmart *)pData;

	//log_print( "on_pine_close:----------------->" );

	if ( pSmart )
	{
		CPine *pPine = (CPine *)pSmart->pData;
                
		mem_free( pSmart );
                pSmart = NULL;

		if ( pPine && pPine->on_destory )
			iRetCode = pPine->on_destory( pPine );
	}

	//log_print( "on_pine_close<---------------------" );

	return iRetCode;
}

static CSmart *create_smart( void *pData, int32_t (*close_callback)( void *pData ) )
{
	CSmart *pRetCode = NULL;

	pRetCode = mem_malloc( sizeof(*pRetCode) );
	if ( pRetCode )
	{
		log_print( "pSmart->%u", pRetCode );
		memset( pRetCode, 0x00, sizeof(*pRetCode) );

		pRetCode->pData = pData;
		pRetCode->on_close = close_callback;
		pRetCode->iRefCount = 1;
		
	}

	return pRetCode;
}

static int32_t retain_smart( CSmart *pSmart )
{
	int32_t iRetCode = -1;

	if ( pSmart )
	{
		(pSmart->iRefCount)++;
		iRetCode = 0;

	}

	return iRetCode;
}

static int32_t release_smart( CSmart *pSmart )
{
	int32_t iRetCode = -1;
	
	if ( pSmart )
	{
		(pSmart->iRefCount)--;

		if ( pSmart->iRefCount <= 0 )
		{
			iRetCode = pSmart->on_close( pSmart );
		}
	}

	return iRetCode;
}

