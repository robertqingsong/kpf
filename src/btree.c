#include "../inc/btree.h"

#include "../inc/mem_api.h"

#include "avl.h"

#include "../inc/log.h"

//create btree.
CBTree *create_btree( btree_comp_t comp )
{
	CBTree *pRetCode = NULL;
	int32_t iOpRet = -1;

	if ( comp )
	{
		pRetCode = (CBTree *)mem_malloc( sizeof(*pRetCode) );
		if ( pRetCode )
		{
			memset( pRetCode, 0x00, sizeof(*pRetCode) );

			if ( init_mutex( &( pRetCode->Locker ) ) >= 0 )
			{
				pRetCode->pBTreeTbl = avl_create( comp, NULL, NULL );

				if ( pRetCode->pBTreeTbl )
				{
					iOpRet = 0;
				}
			}

			if ( iOpRet < 0 )
			{
				mem_free( pRetCode );
				pRetCode = NULL;
			}
		}
	}

	return pRetCode;
}

//destory btree.
int32_t destory_btree( CBTree *pBTree )
{
	int32_t iRetCode = -1;

	if ( pBTree )
	{
		lock( &(pBTree->Locker) );	

		mem_free( pBTree->pBTreeTbl );
		pBTree->pBTreeTbl = NULL;

		unlock( &(pBTree->Locker) );

		mem_free( pBTree );
		pBTree = NULL;

	}

	return iRetCode;
}

//init btree.
int32_t init_btree( CBTree *pBTree, btree_comp_t comp )
{
		int32_t iRetCode = -1;
		
		if ( pBTree && comp )
		{
			memset( pBTree, 0x00, sizeof(*pBTree) );

			if ( init_mutex( &( pBTree->Locker ) ) >= 0 )
			{
				pBTree->pBTreeTbl = avl_create( comp, NULL, NULL );

				if ( pBTree->pBTreeTbl )
				{
					iRetCode = 0;
				}
			}
		}
		
		return iRetCode;
}

//release btree.
int32_t release_btree( CBTree *pBTree )
{
		int32_t iRetCode = -1;
		
		if ( pBTree )
		{
			lock( &(pBTree->Locker) );	

			mem_free( pBTree->pBTreeTbl );
			pBTree->pBTreeTbl = NULL;

			unlock( &(pBTree->Locker) );
			
			iRetCode = 0;
		}
		
		return iRetCode;
}

//add node to btree.
int32_t add_btree_node( CBTree *pBTree, CBTreeNode *pBTreeNode )
{
	int32_t iRetCode = -1;

	if ( pBTree && pBTreeNode )
	{
		void *pOpRet = NULL;

		lock( &( pBTree->Locker ) );

		pOpRet = avl_insert( pBTree->pBTreeTbl, (void *)pBTreeNode );
	
		if ( !pOpRet )
		{
			(pBTree->iCurrentWaterLevel)++;
			
			//log_print( "btree water level->%d", pBTree->iCurrentWaterLevel );
			
			iRetCode = 0;			
		}

		unlock( &( pBTree->Locker ) );
	}

	return iRetCode;
}

//remove btree node.                    
CBTreeNode *remove_btree_node( CBTree *pBTree, int64u_t iBTNodeId )
{
	CBTreeNode *pRetCode = NULL;

	if ( pBTree )
	{

		void *pOpRet = NULL;

		lock( &( pBTree->Locker ) );

		pOpRet = avl_delete( pBTree->pBTreeTbl, (const void *)iBTNodeId );
		
		if ( pOpRet )
		{
			pRetCode = (CBTreeNode *)pOpRet;
			
			(pBTree->iCurrentWaterLevel)--;
			
			//log_print( "btree water level->%d", pBTree->iCurrentWaterLevel );
		}

		unlock( &( pBTree->Locker ) );
	}

	return pRetCode;
}

//search btree node.
CBTreeNode *search_btree_node( CBTree *pBTree, int64u_t iBTNodeId )
{
	CBTreeNode *pRetCode = NULL;

	if ( pBTree )
	{
		void *pOpRet = NULL;
		
		lock( &( pBTree->Locker ) );

		//log_print( "btree water level->%d", pBTree->iCurrentWaterLevel );
		pOpRet = avl_find( pBTree->pBTreeTbl, (const void *)iBTNodeId );
		//log_print( "btree water level->%d", pBTree->iCurrentWaterLevel );
		
		if ( pOpRet )
			pRetCode = (CBTreeNode *)pOpRet;
	
	
		unlock( &( pBTree->Locker ) );
	}

	return pRetCode;
}

