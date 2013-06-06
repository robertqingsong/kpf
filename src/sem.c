#include "../inc/sem.h"

//init sem.
int32_t init_sem( CSem *pSem, int32_t iMaxValue )
{
	int32_t iRetCode = -1;

	if ( pSem )
	{
#if (__OS_LINUX__)
		if ( 0 == sem_init( pSem, 0, iMaxValue ) )
			iRetCode = 0;
#endif
	}

	return iRetCode;
}

//release sem.
void release_sem( CSem *pSem )
{
	if ( pSem )
	{
#if (__OS_LINUX__)

		sem_destroy( pSem );
		pSem = NULL;

#endif
	}
}

//wait sem.
int32_t wait_sem( CSem *pSem )
{
	int32_t iRetCode = -1;
	
	if ( pSem )
	{

#if (__OS_LINUX__)

		if ( sem_wait( pSem ) >= 0 )
			iRetCode = 0;

#endif

	}

	return iRetCode;
}

//post sem.
int32_t post_sem( CSem *pSem )
{
	int32_t iRetCode = -1;

	if ( pSem )
	{

#if (__OS_LINUX__)

		if ( sem_post( pSem ) >= 0 )
			iRetCode = 0;

#endif


	}

	return iRetCode;
}


