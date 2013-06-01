#include "../inc/lock.h"

/*********************************************************************************
* function name: init_mutex
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to init a mutex.
**********************************************************************************/
int32_t init_mutex( CMutex *pMutex )
{
	int32_t iRetCode = -1;

#if (__OS_LINUX__)
#if (KERNEL_DEV_SUPPORT)
	if ( pMutex )
	{
		spin_lock_init( pMutex );

		iRetCode = 0;
	}
#else
	if ( pMutex )
	{
		memset( pMutex, 0x00, sizeof(*pMutex) );
		if ( 0 == pthread_mutex_init( pMutex, NULL ) )
			iRetCode = 0;
	}
#endif
#endif

	return iRetCode;
}

/*********************************************************************************
* function name: lock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to lcok a mutex.
**********************************************************************************/
int32_t lock( CMutex *pMutex )
{
	int32_t iRetCode = -1;

	if ( pMutex )
	{
#if (__OS_LINUX__)
#if (KERNEL_DEV_SUPPORT)
		spin_lock( pMutex );
		iRetCode = 0;
#else
		while ( 0 != pthread_mutex_lock( pMutex ) )
			;	
		iRetCode = 0;
#endif
#endif
	}

	return iRetCode;
}

/*********************************************************************************
* function name: unlock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to unlock a mutex.
**********************************************************************************/
int32_t unlock( CMutex *pMutex )
{
	int32_t iRetCode = -1;

	if ( pMutex )
	{
#if (__OS_LINUX__)

#if (KERNEL_DEV_SUPPORT)
		spin_unlock( pMutex );
		iRetCode = 0;
#else

	while ( 0 != pthread_mutex_unlock( pMutex ) )
		;

	iRetCode = 0;

#endif

#endif
	}

	return iRetCode;
}

/*********************************************************************************
* function name: try_lock
* input param: CMutex *pMutex //the mutex memory address.
* return value type: int32_t //0 success, -1 failed.
* description: to try lock a mutex.
**********************************************************************************/
int32_t try_lock( CMutex *pMutex )
{
	int32_t iRetCode = -1;

	if ( pMutex )
	{

#if (__OS_LINUX__)

#if (KERNEL_DEV_SUPPORT)
		spin_trylock( pMutex );
		iRetCode = 0;
#else
		while ( 0 != pthread_mutex_trylock( pMutex ) )
			;

		iRetCode = 0;

#endif

#endif

	}

	return iRetCode;
}

