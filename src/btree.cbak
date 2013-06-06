#include "../inc/btree.h"

#include "../inc/mem_api.h"

#include "avl.h"

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
		
	}

	return iRetCode;
}

//add node to btree.
int32_t add_btree_node( CBTree *pBTree, CBTreeNode *pBTreeNode )
{
	int32_t iRetCode = -1;


	return iRetCode;
}

//remove btree node.                    
CBTreeNode *remove_btree_node( CBTree *pBTree, int32u_t iBTNodeId )
{
	CBTreeNode *pRetCode = NULL;

	return pRetCode;
}

//search btree node.
CBTreeNode *search_btree_node( CBTree *pBTree, int32u_t iBTNodeId )
{
	CBTreeNode *pRetCode = NULL;

	return pRetCode;
}
