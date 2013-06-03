#include "../inc/pine.h"

#include "../inc/lock.h"

#include "../inc/mem_api.h"

#include "../inc/log.h"

#include "../inc/oal_api.h"

#include "../inc/list.h"

#include "../inc/btree.h"

//smart type define.
typedef struct CSmart_t
{
	int32_t iRefCount;
	void *pData;
	int32_t (*on_close)( void *pData );
}CSmart;

typedef struct CDestory_t
{
	
	CListNode LNode;
}CDestory;

//pine node for btree.
typedef struct CPineNode_t
{
        int32_t m_iId;//pine id.
        void *pm_Base;//pine ref count.
        int32_t m_iHasChild;//is pine has child.
        int32_t m_iSizeOfChild;//child's size.
        int32_t m_iResolved;//resolved.

	CDestory *pDestroyHead;

	CBTreeNode BTNode;
}CPineNode;

typedef struct CPineManager_t
{
	int32_t iInitFlag;

	CBTree *pPineBTree;

	CMutex Locker;
}CPineManager;

static CPineManager fg_PineManager = {
	0,

};

static CSmart *create_smart( void *pData, int32_t (*close_callback)( void *pSmart ) );

static int32_t retain_smart( CSmart *pSrcSmart );

static int32_t release_smart( CSmart *pSmart );

//file function declaration.
static int32_t on_pine_close( void *pData );

static int32_t on_pine_destory( CPine *pPine );

static int32_t is_pine_ready( void );
static int32_t init( void );

//btree function declaration.
static int32_t pine_btree_comp( const void *pValA, const void *pValB, void *pParam );

static CPineNode *search_pine_node( CPine *pPine );

//create pine btree node.
static int32_t create_pine_node( CPine *pPine );

//destory pine node.
static int32_t destory_pine_node( CPine *pPine );

int32_t pine_init( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( is_pine_ready() < 0 )
		if ( init() < 0 )
			return iRetCode;

	lock( &( fg_PineManager.Locker ) );
	
	if ( pPine )
	{
		memset( pPine, 0x00, sizeof(*pPine) );
		
		if ( create_pine_node( pPine ) )
		{
			iRetCode = 0;
		}
	}

	unlock( &( fg_PineManager.Locker ) );

	return iRetCode;
}

//copy pine
CPine *pine_den( CPine *pPineSrc )
{
	CPine *pRetCode = NULL;

   if ( is_pine_ready() < 0 )
		if ( init() < 0 )
			return pRetCode;

	lock( &( fg_PineManager.Locker ) );

	if ( pPineSrc )
	{
		CPineNode *pPineNode = NULL;
		
		pPineNode = search_pine_node( pPineSrc );
		
		if ( pPineNode )
		{
			if ( retain_smart( pPineNode->pm_Base ) < 0 )
				pRetCode = NULL;
			else
				pRetCode = pPineSrc;
		}
	}

	unlock( &( fg_PineManager.Locker ) );

	return pRetCode;
}

//release.
int32_t pine_release( CPine *pPine )
{
	int32_t iRetCode = -1;

        if ( is_pine_ready() < 0 )
		if ( init() < 0 )
			return iRetCode;

	lock( &( fg_PineManager.Locker ) );

	if ( pPine )
	{
		CPineNode *pPineNode = NULL;
		
		pPineNode = search_pine_node( pPine );
		
		if ( pPineNode )
			iRetCode = release_smart( pPineNode->pm_Base );
	}

	unlock( &( fg_PineManager.Locker ) );

	return iRetCode;
}

static int32_t on_pine_destory( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		CPineNode *pPineNode = NULL;
		int32_t (*on_destory_child)(CPine *pPine ) = NULL;
		
		pPineNode = search_pine_node( pPine );
		//release resource.call destruct.
		
		destory_pine_node( pPine );

#if 0
		if ( pPine->m_iHasChild )
		{
			memcpy( &on_destory_child, CHILD_ADDR_OF_PINE(pPine), sizeof(on_destory_child) );
			if ( on_destory_child )
			{
				on_destory_child( pPine );
			}
		}
#endif

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
	
	if ( fg_PineManager.iInitFlag )
		iRetCode = 0;

	return iRetCode;
}

static int32_t init( void )
{
	int32_t iRetCode = -1;

	if ( !(fg_PineManager.iInitFlag) )
	{
		memset( &( fg_PineManager.Locker ), 0x00, sizeof( fg_PineManager.Locker ) );
		iRetCode = init_mutex( &( fg_PineManager.Locker ) );
		if ( iRetCode >= 0 )
		{
			lock( &( fg_PineManager.Locker ) );

			fg_PineManager.pPineBTree = create_btree( pine_btree_comp );

			if ( fg_PineManager.pPineBTree )
				iRetCode = 0;

			unlock( &( fg_PineManager.Locker ) );
		}
	}
	else
		iRetCode = 0;

	return iRetCode;
}

static int32_t pine_btree_comp( const void *pValA, const void *pValB, void *pParam )
{
	int32_t iRetCode = -1;


	return iRetCode;
}

//create pine btree node.
static int32_t create_pine_node( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		CPineNode *pPineNode = mem_malloc( sizeof( *pPineNode ) );
		if ( pPineNode )
		{
			memset( pPineNode, 0x00, sizeof( *pPineNode ) );
			
			pPineNode->pm_Base = create_smart( pPine, on_pine_close );
			
			if ( pPineNode->pm_Base )
			{
				//init pine btree node.
				pPineNode->BTNode.pData = pPine;

				if ( add_btree_node( fg_PineManager.pPineBTree, &(pPineNode->BTNode) ) >= 0 )
					iRetCode = 0;	
			}

			if ( iRetCode < 0 )
			{
				if ( pPineNode->pm_Base )
				{
					mem_free( pPineNode->pm_Base );
					pPineNode->pm_Base = NULL;					
				}
				
				mem_free( pPineNode );
				pPineNode = NULL;
			}
		}
	}

	return iRetCode;
}

static CPineNode *search_pine_node( CPine *pPine )
{
	CPineNode *pRetCode = NULL;


	if ( pPine )
	{
		CBTreeNode *pBTNode = NULL;
		
		pBTNode = search_btree_node( fg_PineManager.pPineBTree, ( int32u_t )pPine );
		if ( pBTNode )
		{
			pRetCode = CONTAINER_OF_BTNODE( pBTNode, CPineNode );
		}
	}

	return pRetCode;
}

//destory pine node.
static int32_t destory_pine_node( CPine *pPine )
{
	int32_t iRetCode = -1;

	if ( pPine )
	{
		CBTreeNode *pBTNode = NULL;
		CPineNode *pPineNode = NULL;
		
		pBTNode = remove_btree_node( fg_PineManager.pPineBTree, ( int32u_t )pPine );
		if ( pBTNode )
		{
			pPineNode = CONTAINER_OF_BTNODE(pBTNode, CPineNode);
			
			mem_free( pPineNode->pm_Base );
			pPineNode->pm_Base = NULL;
			
			mem_free( pPineNode );
			pPineNode = NULL;

			iRetCode = 0;
		}
	}

	return iRetCode;
}


