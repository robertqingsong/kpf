#include "../inc/pine.h"

#include "../inc/lock.h"

#include "../inc/mem_api.h"

#include "../inc/log.h"

#include "../inc/oal_api.h"

#include "../inc/list.h"

#include "../inc/btree.h"

#include "../inc/timer.h"

#include "../inc/block.h"

#include "../inc/net_api.h"

//smart type define.
typedef struct CSmart_t
{
	int32_t iRefCount;
	void *pData;
	int32_t (*on_close)( void *pData );
}CSmart;

//pine node for btree.
typedef struct CPineNode_t
{
        int32_t m_iId;//pine id.
        void *pm_Base;//pine ref count.
        int32_t m_iHasChild;//is pine has child.
        int32_t m_iSizeOfChild;//child's size.
        int32_t m_iResolved;//resolved.

	CPineMethod *pPineMethod;

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
static void release( void );

//btree function declaration.
static int32_t pine_btree_comp( const void *pValA, const void *pValB, void *pParam );

static CPineNode *search_pine_node( CPine *pPine );

//create pine btree node.
static int32_t create_pine_node( CPine *pPine, CPineMethod *pMethod );

//destory pine node.
static int32_t destory_pine_node( CPine *pPine );

//---------------------------------------------------------------------------------------------->

//init pine system.
int32_t init_pine_system( void )
{
	int32_t iRetCode = -1;

	
	if ( is_pine_ready() < 0 )
	{
		if ( init() >= 0 )
		{
			init_timer(  );
			
			init_block(  );
			
			init_reactor(  );
			
			iRetCode = 0;
		}
	}
	else 
		iRetCode = 0;
	
	return iRetCode;
}

//release pine system.
void release_pine_system( void )
{
	//log_print( "release_pine_system:------------------>" );

	release_reactor(  );

	release_block(  );

	release_timer(  );	
	
	release(  );
	
	//log_print( "release_pine_system end<------------------" );
}

int8u_t *create_pine( int32_t iSizeOfPine )
{
	int8u_t *pRetCode = NULL;

	//log_print( "create_pine:------------------>" );

	if ( iSizeOfPine > 0 )
	{
			pRetCode = mem_malloc( iSizeOfPine + 8 );
			if ( pRetCode )
			{
				memset( pRetCode, 0x00, iSizeOfPine + 8 );
				
				pRetCode = pRetCode + 8;
			}
			else 
				log_print( "%s %s-%d: !if ( pRetCode ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s-%d: !if ( iSizeOfPine > 0 ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
	//log_print( "create_pine end<------------------" );
	
	return pRetCode;	
}

//destroy pine memory.
static void destroy_pine( int8u_t *pPineMem )
{
	//log_print( "destroy_pine:------------------>" );
	
	if ( pPineMem )
	{
		
		mem_free( pPineMem - 8 );
		
		pPineMem = NULL;
	}
	else 
		log_print( "%s %s-%d: !if ( pPineMem ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
	//log_print( "destroy_pine end<------------------" );
}

//add pine methods.
int32_t add_pine_method( CPineMethod **ppHeadMethod, CPineMethod *pNewMethod )
{
	int32_t iRetCode = -1;

	//log_print( "add_pine_method:------------------>" );
	
	if ( ppHeadMethod && pNewMethod )
	{
		pNewMethod->LNode.Prev = pNewMethod->LNode.Next = NULL;
		if ( *ppHeadMethod )
		{
			CListNode *pMethodHeadLNode = &( ( *(ppHeadMethod) )->LNode );
			
			iRetCode = insert_list_head_front( &pMethodHeadLNode, &( pNewMethod->LNode ) );
			if ( iRetCode >= 0 )
			{
				*ppHeadMethod = CONTAINER_OF_LIST( pMethodHeadLNode, CPineMethod );
			}
			else 
				log_print( "%s %s-%d: !if ( iRetCode >= 0 ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		}
		else
		{
			*ppHeadMethod = pNewMethod;
			
			iRetCode = 0;
		}
	}
	else 
		log_print( "%s %s-%d: !if ( ppHeadMethod && pNewMethod ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
	//log_print( "add_pine_method end<------------------" );

	return iRetCode;
}

int32_t pine_init( CPine *pPine, CPineMethod *pMethod )
{
	int32_t iRetCode = -1;

	//log_print( "pine_init:------------------>" );
	
	if ( is_pine_ready() < 0 )
	{
		log_print( "%s %s-%d: if ( init() < 0 ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		return iRetCode;
	}

	lock( &( fg_PineManager.Locker ) );
	
	if ( pPine )
	{
		if ( create_pine_node( pPine, pMethod) >= 0 )
		{
			//log_print( "pine_init ok, pPine->%u", pPine );
			iRetCode = 0;
		}
		else 
			log_print( "%s %s-%d: !if ( create_pine_node( failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s-%d: !if ( pPine ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

	unlock( &( fg_PineManager.Locker ) );
	
	//log_print( "pine_init end<------------------" );

	return iRetCode;
}

//copy pine
CPine *pine_den( CPine *pPineSrc )
{
	CPine *pRetCode = NULL;
	
	//log_print( "pine_den:------------------>" );

   if ( is_pine_ready() < 0 )
	{
		log_print( "if ( init() < 0 ) failed?????????????????????????" );
		return pRetCode;
	}

	lock( &( fg_PineManager.Locker ) );

	if ( pPineSrc )
	{
		CPineNode *pPineNode = NULL;
		
		pPineNode = search_pine_node( pPineSrc );
		
		if ( pPineNode )
		{
			if ( retain_smart( pPineNode->pm_Base ) < 0 )
			{
				log_print( "!if ( retain_smart( pPineNode->pm_Base ) < 0 ) failed????????????????????????????" );
				pRetCode = NULL;
			}
			else
				pRetCode = pPineSrc;
		}
		else 
			log_print( "!if ( pPineNode ) failed????????????????????????" );
	}
	else 
		log_print( "%s %s-%d: !if ( pPineSrc ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

	unlock( &( fg_PineManager.Locker ) );

	//log_print( "pine_den end<------------------" );

	return pRetCode;
}

//release.
int32_t pine_release( CPine *pPine )
{
	int32_t iRetCode = -1;
	
	//log_print( "pine_release:------------------>" );

   if ( is_pine_ready() < 0 )
	{
		log_print( "if ( init() < 0 )????????????????\r\n" );
		return iRetCode;
	}

	lock( &( fg_PineManager.Locker ) );

	if ( pPine )
	{
		CPineNode *pPineNode = NULL;
		
		//log_print( "%s %s-%d: pine_release:------------------------->\r\n", __FILE__, __FUNCTION__, __LINE__ );

		//log_print( "pine_release: pPine->%u", pPine );		
		
		pPineNode = search_pine_node( pPine );
		
		if ( pPineNode )
		{
			iRetCode = release_smart( pPineNode->pm_Base );
		}
		else 
			log_print( "!if ( pPineNode ) failed???????????????????????" );
	}
	else 
		log_print( "%s %s-%d: !if ( pPine ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

	unlock( &( fg_PineManager.Locker ) );
	
	//log_print( "pine_release end<------------------" );

	return iRetCode;
}

static int32_t on_pine_destory( CPine *pPine )
{
	int32_t iRetCode = -1;

	//log_print( "on_pine_destory:------------------>" );
	
	if ( pPine )
	{
		CPineNode *pPineNode = NULL;
		
		pPineNode = search_pine_node( pPine );
		//release resource.call destruct.
		if ( pPineNode && pPineNode->pPineMethod )
		{
			CPineMethod *pTempMethod = NULL;
			CListNode *pTempMethodLNode = &( pPineNode->pPineMethod->LNode );
			CListNode *pMethodHeadLNode = NULL;
			
			pTempMethodLNode = pTempMethodLNode->Prev;
			pMethodHeadLNode = pTempMethodLNode;
			while ( pTempMethodLNode )
			{
				pTempMethod = CONTAINER_OF_LIST( pTempMethodLNode, CPineMethod );
				
				if ( pTempMethod && pTempMethod->release )
					pTempMethod->release( pPine );
						
				if ( pTempMethodLNode->Prev != pMethodHeadLNode )
							pTempMethodLNode = pTempMethodLNode->Prev;
				else
				{
					pTempMethodLNode = NULL;								
				}
						
				mem_free( pTempMethod );
				pTempMethod = NULL;
			}
		}
		else 
			log_print( "%s %s-%d: !if ( pPineNode && pPineNode->p failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
		destory_pine_node( pPine );
		
		destroy_pine( (int8u_t *)pPine );

		iRetCode = 0;
	}
	else 
		log_print( "%s %s-%d: !if ( pPine ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
	//log_print( "on_pine_destory end<------------------" );

	return iRetCode;
}

static int32_t on_pine_close( void *pData )
{
	int32_t iRetCode = -1;
	CSmart *pSmart = (CSmart *)pData;

	//log_print( "on_pine_close:------------------>" );
	
	if ( pSmart )
	{
		CPine *pPine = (CPine *)pSmart->pData;
                
		mem_free( pSmart );
      pSmart = NULL;

		iRetCode = on_pine_destory( pPine );
	}
	else 
		log_print( "%s %s-%d: !if ( pSmart ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
	//log_print( "on_pine_close end<------------------" );

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
	else 
		log_print( "%s %s-%d: !if ( pRetCode ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

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
	else 
		log_print( "%s %s-%d: !if ( pSmart ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

	return iRetCode;
}

static int32_t release_smart( CSmart *pSmart )
{
	int32_t iRetCode = -1;
	
	if ( pSmart )
	{
		(pSmart->iRefCount)--;

		//log_print( "%s %s-%d: smart ref count-->%d\r\n", __FILE__, __FUNCTION__, __LINE__, pSmart->iRefCount );

		if ( pSmart->iRefCount <= 0 )
		{
			iRetCode = pSmart->on_close( pSmart );
		}
	}
	else 
		log_print( "%s %s-%d: !if ( pSmart ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

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
		
		if ( init_mutex( &( fg_PineManager.Locker ) ) >= 0 )
		{
			lock( &( fg_PineManager.Locker ) );

			fg_PineManager.pPineBTree = create_btree( pine_btree_comp );

			if ( fg_PineManager.pPineBTree )
			{
				fg_PineManager.iInitFlag = 1;
				iRetCode = 0;
				
				//log_print( "init pine ok........................." );
			}

			unlock( &( fg_PineManager.Locker ) );
		}
	}
	else
		iRetCode = 0;

	return iRetCode;
}

static void release( void )
{
		if ( fg_PineManager.iInitFlag )
		{
				lock( &( fg_PineManager.Locker ) );
				
				if ( fg_PineManager.pPineBTree )
				{
					destory_btree( fg_PineManager.pPineBTree );
					fg_PineManager.pPineBTree = NULL;
				}
				
				fg_PineManager.iInitFlag = 0;
				
				unlock( &( fg_PineManager.Locker ) );
		}
}

static int32_t pine_btree_comp( const void *pValA, const void *pValB, void *pParam )
{
	int32_t iRetCode = -1;

	if ( pValA && pValB )
	{
		int64u_t A = (int64u_t)pValA, B = (int64u_t)pValB;
		
		if ( A > B )
			iRetCode = 1;
		else if ( A < B )
			iRetCode = -1;
		else 
			iRetCode = 0;
	}
	else 
		log_print( "%s %s-%d: !if ( pValA && pValB ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

	return iRetCode;
}

//create pine btree node.
static int32_t create_pine_node( CPine *pPine, CPineMethod *pMethod )
{
	int32_t iRetCode = -1;
	
	//log_print( "create_pine_node:------------------>" );
	if ( pPine && pMethod )
	{
		CPineNode *pPineNode = mem_malloc( sizeof( *pPineNode ) );
		if ( pPineNode )
		{
			CPineMethod *pTempMethod = NULL, *pNewMethod = NULL;
			CListNode *pMethodHeadLNode = NULL, *pTailMethodLNode = NULL, *pTempMethodLNode = NULL;
			
			//log_print( "param ok, allow pine node memory ok" );
				
			memset( pPineNode, 0x00, sizeof( *pPineNode ) );

			pPineNode->pm_Base = create_smart( pPine, on_pine_close );
			
			if ( pPineNode->pm_Base )
			{
				//log_print( "create pine node smart ok.............start init pine method.........." );
				pTempMethodLNode = &( pMethod->LNode );
				while ( pTempMethodLNode )
				{
					pTempMethod = CONTAINER_OF_LIST( pTempMethodLNode, CPineMethod );
					pNewMethod = mem_malloc( sizeof( *pTempMethod ) );
					if ( pNewMethod )
					{
						memset( pNewMethod, 0x00, sizeof(*pNewMethod) );
						
						pNewMethod->init = pTempMethod->init;
						pNewMethod->release = pTempMethod->release;
						
						if ( !(pPineNode->pPineMethod) )
						{
							pPineNode->pPineMethod = pNewMethod;
							pTailMethodLNode = &(pNewMethod->LNode);
						}
						else
						{
								CListNode *pHeadLNode = &( pPineNode->pPineMethod->LNode );
								
								if ( insert_list_head_front( &pHeadLNode, &( pNewMethod->LNode ) ) >= 0 )
								{
									pPineNode->pPineMethod = pNewMethod;
								}
								else 
								{
									log_print( "%s %s-%d: !if ( insert_list_head_front failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
									
									mem_free( pNewMethod );
									pNewMethod = NULL;log_print( "on_pine_close end<------------------" );
									
									break ;
								}												
						}
					}
					else
						break ;

					if ( pTempMethodLNode->Next )
						pTempMethodLNode = pTempMethodLNode->Next;
					else 
					{
						pMethodHeadLNode = &(pNewMethod->LNode);
						pTempMethodLNode = NULL;
					}
				}
				
				if ( !pTempMethodLNode )
				{
					//log_print( "init pine method ok................." );
					if ( pMethodHeadLNode && pTailMethodLNode )
					{
						//ring list.
						pMethodHeadLNode->Prev = pTailMethodLNode;
					
						pTailMethodLNode->Next = pMethodHeadLNode;
					}
					
					memcpy( ((( int8u_t * )pPine) - 8), &pPineNode, sizeof(pPineNode) );
					//init pine btree node.
					if ( add_btree_node( fg_PineManager.pPineBTree, &(pPine->BTNode) ) >= 0 )
					{
						//call pine method to initialize pine object.
						if ( pPineNode->pPineMethod )
						{
							pTempMethodLNode = &( pPineNode->pPineMethod->LNode );
							pMethodHeadLNode = pTempMethodLNode;
					
							while ( pTempMethodLNode )
							{
								pTempMethod = CONTAINER_OF_LIST( pTempMethodLNode, CPineMethod );
								
								if ( pTempMethod->init )
									pTempMethod->init( pPine );
						
								if ( pTempMethodLNode->Next != pMethodHeadLNode )
									pTempMethodLNode = pTempMethodLNode->Next;
								else
								{
									pTempMethodLNode = NULL;								
								}
							}
						}
						
						iRetCode = 0;		
					}
				}
			}

			if ( iRetCode < 0 )
			{
				if ( pPineNode->pPineMethod )
				{
					pTempMethodLNode = &( pPineNode->pPineMethod->LNode );
					pMethodHeadLNode = pTempMethodLNode;
					
					while ( pTempMethodLNode )
					{
						pTempMethod = CONTAINER_OF_LIST( pTempMethodLNode, CPineMethod );
						
						if ( pTempMethodLNode->Next != pMethodHeadLNode )
							pTempMethodLNode = pTempMethodLNode->Next;
						else
						{
							pTempMethodLNode = NULL;								
						}
						
						mem_free( pTempMethod );
						pTempMethod = NULL;
					}
				}	
				
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
	else 
		log_print( "%s %s-%d: !if ( pPine && pMethod ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		
	//log_print( "create_pine_node end<------------------" );

	return iRetCode;
}

static CPineNode *search_pine_node( CPine *pPine )
{
	CPineNode *pRetCode = NULL;

	if ( pPine )
	{	
		
		memcpy( &pRetCode, (((int8u_t *)pPine) - 8), sizeof( pRetCode ) );
		if ( !pRetCode )
			log_print( "%s %s-%d: if ( !pRetCode ) ?????????????????failed\r\n", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s-%d: !if ( pPine ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

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
		
		//log_print( "destory_pine_node: btree water level: %d, btree->%u", 
				//		fg_PineManager.pPineBTree->iCurrentWaterLevel, fg_PineManager.pPineBTree );
		pBTNode = remove_btree_node( fg_PineManager.pPineBTree, &(pPine->BTNode ) );
		if ( pBTNode )
		{
			memcpy( &pPineNode, (((int8u_t *)pPine) - 8), sizeof( pPineNode ) );
			
			if ( pPineNode )
			{
				//log_print( "%s %s-%d: destroy pine node...........\r\n", __FILE__, __FUNCTION__, __LINE__ );
			
			
				mem_free( pPineNode );
				pPineNode = NULL;

				iRetCode = 0;
			}
			else
				log_print( "%s %s-%d: !if ( pPineNode ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
		}
		else 
			log_print( "%s %s-%d: !if ( pBTNode ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );
	}
	else 
		log_print( "%s %s-%d: !if ( pPine ) failed ????????????????????\r\n", __FILE__, __FUNCTION__, __LINE__ );

	return iRetCode;
}


