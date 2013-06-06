/********************************************************
 * file name: hash.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: hash for framework kpf
 * *****************************************************/

#ifndef __HASH_H__
#define __HASH_H__

#include "config.h"
#include "typedefs.h"

#include "list.h"

#include "lock.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct CHashElem_t
{
	int64u_t m_iHashId;
	
	CListNode LNode;
}CHashElem;

#define CONTAINER_OF_HASH(addr, type) \
	CONTAINER_OF( addr, HElem, type )

typedef struct CHashTbl_t
{
	CHashElem **ppHashElemArray;

	int32_t m_iMaxHashTblSize;
	int32_t m_iCurrentWaterLevel;

	CMutex Locker;
}CHashTbl;

//init hash table.
int32_t init_hash_tbl( CHashTbl *pHashTbl, int32_t iMaxWaterLevel );

//release hash table.
void release_hash_tbl( CHashTbl *pHashTbl );

//insert hash table.
int32_t insert_hash_tbl( CHashTbl *pHashTbl, CHashElem *pNewHashElem );

//search hash table.
CHashElem *search_hash_tbl( CHashTbl *pHashTbl, int64u_t iHashId );

//delete hash table.
CHashElem *remove_hash_tbl( CHashTbl *pHashTbl, int64u_t iHashId );

#if defined(__cplusplus)
}
#endif

#endif/* __HASH_H__ */

