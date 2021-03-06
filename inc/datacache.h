#ifndef __DATACACHE_H__
#define __DATACACHE_H__

#include "config.h"

#include "typedefs.h"

#include "lock.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//data cache define.
typedef struct CDataCache_t
{
	int8u_t *pDataBuf;
	int32_t iRear;
	int32_t iFront;	
	
	int32_t iMaxDataBufSize;
	int32_t iCurrentDataBufLen;
	
	CMutex Locker;
}CDataCache;

//data cache init.
int32_t init_datacache( CDataCache *pDataCache, const int32_t iMaxWaterLevel );

//reset datacache.
int32_t reset_datacache( CDataCache *pDataCache );

//write datacache.
int32_t write_datacache( CDataCache *pDataCache, const int8u_t *pData, const int32_t iDataLen );

//read datacache.
int32_t read_datacache( CDataCache *pDataCache, int8u_t *pData, const int32_t iDataBufLen );

//get data space size.
int32_t data_of_datacache( const CDataCache *pDataCache );

//free space .
int32_t free_of_datacache( const CDataCache *pDataCache );

#if defined(__cplusplus)
}
#endif

#endif/* __DATACACHE_H__ */
