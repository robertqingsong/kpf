/********************************************************
 * file name: list.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: list for framework kpf
 * *****************************************************/

#ifndef __LIST_H__
#define __LIST_H__

#include "typedefs.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//list node type define.
typedef struct CListNode_t
{
	void *pData;
	struct CListNode_t *Prev;
	struct CListNode_t *Next;	
}CListNode;

#define CONTAINER_OF_LIST(addr, type) \
	CONTAINER_OF( addr, LNode, type )

//front insert node to list.
int32_t insert_list_head_front( CListNode **ppListHead, CListNode *pNewNode );
//rear insert node to list.
int32_t insert_list_head_rear( CListNode **ppListHead, CListNode *pNewNode );
//remove node from list.
CListNode *remove_list_head_node( CListNode **ppListHead, CListNode *pRemoveNode );

#if defined(__cplusplus)
}
#endif

#endif/* __LIST_H__ */
