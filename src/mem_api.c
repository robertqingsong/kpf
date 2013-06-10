#include "../inc/mem_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../inc/log.h"

//malloc memory.
void *mem_malloc( int32_t iMemSize )
{
	void *pRetCode = NULL;
	
	log_print( "mem_malloc:-------------------->" );
	
	if ( iMemSize > 0 )
		pRetCode = malloc( iMemSize );
		
	log_print( "mem_malloc:-------------------->end" );
	
	return pRetCode;	
}

//free memory.
int32_t mem_free( void *pMem )
{
	int32_t iRetCode = -1;
	
	log_print( "mem_free:-------------------->" );

	if ( pMem )
	{
		free( pMem );
		pMem = NULL;

		log_print( "pMem------------------------------------------------------------------------>%u", pMem );
		
		iRetCode = 0;
	}

	
	log_print( "mem_free:-------------------->end" );
	
	return iRetCode;
}


