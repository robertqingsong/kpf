#if 0
#include <stdio.h>

typedef struct CAnimal_t
{
	short s;
}CAnimal;

typedef struct CPerson_t
{
	CAnimal animal;

	char j;
}CPerson;

int main( int argc, char **argv )
{
	CPerson person;


	printf( "&person->%u\r\n", &person );

	printf( "&(person.animal)->%u\r\n", &( person.animal ) );

	return 0;
}

#else


#include "btree.h"

int32_t btree_comp( const void *pValA, const void *pValB, void *pParam )
{
	int32_t iRetCode = -1;
	
	int64u_t A = (int64u_t)pValA;
	int64u_t B = (int64u_t)pValB;

	printf( "btree_comp:--------------------->\r\n" );

	printf( "A-->%u\r\n", A );
	printf( "B-->%u\r\n", B );

	if ( A > B )
		iRetCode = 1;
	else if ( A < B )
		iRetCode = -1;
	else
		iRetCode = 0;


	printf( "btree_comp<-------------------------\r\n" );

	return iRetCode;
}

typedef struct CTestNode_t
{
	int i;

	int j;


	CBTreeNode BTNode;
}CTestNode;

#if 0

//add node to btree.
int32_t add_btree_node( CBTree *pBTree, CBTreeNode *pBTreeNode );
//remove btree node.
CBTreeNode *remove_btree_node( CBTree *pBTree, int64u_t iBTNodeId );
//search btree node.
CBTreeNode *search_btree_node( CBTree *pBTree, int64u_t iBTNodeId );
#endif

int main( int argc, char **argv )
{
	CBTree *pBTree = create_btree( btree_comp );

	if ( pBTree )
	{
		CTestNode testnode1;

		memset( &testnode1, 0x00, sizeof(testnode1) );
		testnode1.i = 100;
		testnode1.j = 200;
		printf( "testnode1->%u\r\n", &testnode1 );
		if ( add_btree_node( pBTree, &(testnode1.BTNode) ) >= 0 )
		{

			CTestNode testnode2;
			
			testnode2.i = 200;
			testnode2.j = 300;
			printf( "testnode2->%u\r\n", &testnode2 );

			printf( "add btree node 1 ok................\r\n" );
			if ( add_btree_node( pBTree, &( testnode2.BTNode ) ) >= 0 )
			{
				CBTreeNode *pNode1 = NULL, *pNode2 = NULL;

				printf( "add btree 2 node ok...............\r\n" );
				
				pNode1 = search_btree_node( pBTree, (int64u_t)&(testnode1.BTNode) );
				if ( pNode1 )
				{
					printf( "search node 1 ok.......................\r\n" );

					printf( "node 1-->%u\r\n", CONTAINER_OF_BTNODE(pNode1, CTestNode) );
				}

				pNode2 = search_btree_node( pBTree, (int64u_t)&(testnode2.BTNode) );
				if ( pNode2 )
				{
					printf( "search node 2 ok........................\r\n" );
				
					printf( "node 2->%u\r\n", CONTAINER_OF_BTNODE(pNode2, CTestNode) );
				}	

				pNode1 = remove_btree_node( pBTree, (int64u_t)&(testnode1.BTNode) );
				if ( pNode1 )
					printf( "remove btree node 1 ok................\r\n" );

				pNode2 = remove_btree_node( pBTree, (int64u_t)&(testnode2.BTNode) );
				if ( pNode2 )
					printf( "remove btree node 2 ok................\r\n" );
			}
		}
		else
			printf( "add btree node failed????????????????????\r\n" );

		destory_btree( pBTree );
	}

	return 0;
}


#endif

