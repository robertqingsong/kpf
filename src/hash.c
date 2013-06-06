#include "../inc/hash.h"

#include "../inc/mem_api.h"

#include "../inc/lock.h"

#if 0
typedef struct CHashElem_t
{

        CListNode LNode;
	}CHashElem;

	typedef struct CHashTbl_t
	{
	        CHashElem **ppHashElemArray;

		        int32_t m_iMaxHashTblSize;
			        int32_t m_iCurrentWaterLevel;

				        CMutex Locker;
					}CHashTbl;

#endif

//init hash table.
int32_t init_hash_tbl( CHashTbl *pHashTbl, int32_t iMaxWaterLevel )
{
	int32_t iRetCode = -1;

	if ( pHashTbl && iMaxWaterLevel > 0 )
	{
		if ( init_mutex( &( pHashTbl->Locker ) ) >= 0 )
		{
			lock( &( pHashTbl->Locker ) );

			pHashTbl->ppHashElemArray = mem_malloc( sizeof(void *) * iMaxWaterLevel );
			
			if ( pHashTbl->ppHashElemArray )
			{
				int64u_t i = 0;

				for ( i = 0; i < iMaxWaterLevel; i++ )
					pHashTbl->ppHashElemArray[i] = NULL;

				pHashTbl->m_iMaxHashTblSize = iMaxWaterLevel;
				pHashTbl->m_iCurrentWaterLevel;

				iRetCode = 0;
			}

			unlock( &( pHashTbl->Locker ) );
		} 
		
	}

	return iRetCode;
}

//release hash table.
void release_hash_tbl( CHashTbl *pHashTbl )
{
	if ( pHashTbl )
	{
		lock( &(pHashTbl->Locker) );

		mem_free( pHashTbl->ppHashElemArray );
		pHashTbl->ppHashElemArray = NULL;

		pHashTbl->m_iMaxHashTblSize = 0;
		pHashTbl->m_iCurrentWaterLevel = 0;
		
		unlock( &( pHashTbl->Locker ) );
	}
}

//insert hash table.
int32_t insert_hash_tbl( CHashTbl *pHashTbl, CHashElem *pNewHashElem )
{
	int32_t iRetCode = -1;

	if ( pHashTbl && pNewHashElem )
	{
		int64u_t iHashId = pNewHashElem->m_iHashId;
		int64u_t iHashKey = iHashId % pHashTbl->m_iMaxHashTblSize;
		CHashElem *pHashHeadElem = NULL;

		lock( &( pHashTbl->Locker ) );
	
		//init list node.
		memset( &( pNewHashElem->LNode ), 0x00, sizeof( pNewHashElem->LNode ) );
		pHashHeadElem = pHashTbl->ppHashElemArray[iHashKey];
		if ( pHashHeadElem )
		{
			CListNode *pHListHead = &( pHashHeadElem->LNode );
			
			//add new hash node to current list.
			iRetCode = insert_list_head_rear( &pHListHead, &( pNewHashElem->LNode ) );
		}
		else
		{
			//add first hash elem of hash key.
			pHashTbl->ppHashElemArray[iHashKey] = pNewHashElem;

			iRetCode = 0;
		}

		unlock( &( pHashTbl->Locker ) );
	}

	return iRetCode;
}

//search hash table.
CHashElem *search_hash_tbl( CHashTbl *pHashTbl, int64u_t iHashId )
{
	CHashElem *pRetCode = NULL;

	if ( pHashTbl )
	{
		int64u_t iHashKey = iHashId % pHashTbl->m_iMaxHashTblSize;
		CHashElem *pHashHeadElem = NULL;

		lock( &( pHashTbl->Locker ) );

		pHashHeadElem = pHashTbl->ppHashElemArray[iHashKey];
		if ( pHashHeadElem )
		{
			CHashElem *pTempHashElem = NULL;
			CListNode *pHLHead = &( pHashHeadElem->LNode ), *pTempHLNode = NULL;

			pTempHLNode = pHLHead;
			while ( pTempHLNode )
			{
				pTempHashElem = CONTAINER_OF_LIST( pTempHLNode, CHashElem );
				if ( iHashId == pTempHashElem->m_iHashId )
				{
					pRetCode = pTempHashElem;
					
					break ;
				}
				
				pTempHLNode = pTempHLNode->Next;
			}
		}

		unlock( &( pHashTbl->Locker ) );
	}

	return pRetCode;
}

//delete hash table.
CHashElem *remove_hash_tbl( CHashTbl *pHashTbl, int64u_t iHashId )
{
	CHashElem *pRetCode = NULL;

	if ( pHashTbl )
	{
		int64u_t iHashKey = iHashId % pHashTbl->m_iMaxHashTblSize;
		CHashElem *pHashHeadElem = NULL;

		lock( &( pHashTbl->Locker ) );

		pHashHeadElem = pHashTbl->ppHashElemArray[iHashKey];
		if ( pHashHeadElem )
		{
			CHashElem *pTempHashElem = NULL;
			CListNode *pHLHead = &( pHashHeadElem->LNode ), *pTempHLNode = NULL;
			int32_t iFoundFlag = 0;

			pTempHLNode = pHLHead;
			while ( pTempHLNode )
			{
				pTempHashElem = CONTAINER_OF_LIST( pTempHLNode, CHashElem );
				if ( iHashId == pTempHashElem->m_iHashId )
				{
					iFoundFlag = 1;
					break ;
				}

				pTempHLNode = pTempHLNode->Next;
			}

			if ( iFoundFlag )
			{
				CListNode *pRemoveLNode = remove_list_head_node( &pHLHead, pTempHLNode );
				
				if ( pRemoveLNode )
				{

					if ( pTempHashElem == pHashHeadElem )
					{
						pHashTbl->ppHashElemArray[iHashKey] = CONTAINER_OF_LIST( pHLHead, CHashElem );
					}

					pRetCode = pTempHashElem;
				}
			}
		}

		unlock( &( pHashTbl->Locker ) );

	}

	return pRetCode;
}

