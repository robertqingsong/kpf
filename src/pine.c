#include "../inc/pine.h"

#include "../inc/lock.h"

#include "../inc/mem_api.h"

#include "../inc/log.h"

#include "../inc/oal_api.h"

#include "../inc/hash.h"

//smart type define.
typedef struct CSmart_t
{
	int32_t iRefCount;
	void *pData;
	int32_t (*on_close)( void *pData );
}CSmart;

//pine file global locker define.
static CMutex fg_PineMutex;
//flag for initialize.
static int32_t fg_iInitFlag = 0;
//hash table for pine manmagement.


static CSmart *create_smart( void *pData, int32_t (*close_callback)( void *pSmart ) );

static int32_t retain_smart( CSmart *pSrcSmart );

static int32_t release_smart( CSmart *pSmart );

//file function declaration.
static int32_t on_pine_close( void *pData );

static int32_t on_pine_destory( CPine *pPine );

static int32_t is_pine_ready( void );
static int32_t init( void );

int32_t pine_init( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( is_pine_ready() < 0 )
		if ( init() < 0 )
			return iRetCode;

	lock( &fg_PineMutex );
	
	if ( pPine )
	{
		memset( pPine, 0x00, sizeof(*pPine) );

		pPine->pm_Base = create_smart( pPine, on_pine_close );
		if ( pPine->pm_Base )
			iRetCode = 0;
	}

	unlock( &fg_PineMutex );

	return iRetCode;
}

//copy pine
CPine *pine_den( CPine *pPineSrc )
{
	CPine *pRetCode = NULL;

        if ( is_pine_ready() < 0 )
		if ( init() < 0 )
			return pRetCode;

	lock( &fg_PineMutex );

	if ( pPineSrc )
	{
		pRetCode = pPineSrc;

		if ( retain_smart( pPineSrc->pm_Base ) < 0 )
			pRetCode = NULL;
	}

	unlock( &fg_PineMutex );

	return pRetCode;
}

//release.
int32_t pine_release( CPine *pPine )
{
	int32_t iRetCode = -1;

        if ( is_pine_ready() < 0 )
		if ( init() < 0 )
			return iRetCode;

	lock( &fg_PineMutex );

	if ( pPine )
	{
		iRetCode = release_smart( pPine->pm_Base );
	}

	unlock( &fg_PineMutex );

	return iRetCode;
}

static int32_t on_pine_destory( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		int32_t (*on_destory_child)( CPine *pPine ) = NULL;
		
		if ( pPine->m_iHasChild )
		{
			memcpy( &on_destory_child, CHILD_ADDR_OF_PINE(pPine), sizeof(on_destory_child) );
			if ( on_destory_child )
			{
				on_destory_child( pPine );
			}
		}

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
                
		mem_free( pSmart );
                pSmart = NULL;

		iRetCode = on_pine_destory( pPine );
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

static int32_t is_pine_ready( void )
{
	int32_t iRetCode = -1;
	
	if ( fg_iInitFlag )
		iRetCode = 0;

	return iRetCode;
}

static int32_t init( void )
{
	int32_t iRetCode = -1;

	if ( !fg_iInitFlag )
	{
		iRetCode = init_mutex( &fg_PineMutex );	
	}
	else
		iRetCode = 0;

	return iRetCode;
}

