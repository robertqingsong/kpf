#include "../inc/datacache.h"

#include "../inc/mem_api.h"


//data cache init.
int32_t init_datacache( CDataCache *pDataCache, const int32_t iMaxWaterLevel )
{
	int32_t iRetCode = -1;
	
	if ( pDataCache && iMaxWaterLevel > 0 )
	{
		memset( pDataCache, 0x00, sizeof( *pDataCache ) );
		
		if ( init_mutex( &(pDataCache->Locker) ) >= 0 )
		{
			lock( &(pDataCache->Locker) );
			
			pDataCache->pDataBuf = mem_malloc( iMaxWaterLevel );
			
			if ( pDataCache->pDataBuf )
			{
				pDataCache->iRear = pDataCache->iFront = 0;
				pDataCache->iMaxDataBufSize = iMaxWaterLevel;
				pDataCache->iCurrentDataBufLen = 0;
				
				iRetCode = 0;
			}
			
			unlock( &(pDataCache->Locker) );
		}	
	}
	
	return iRetCode;
}

//reset datacache.
int32_t reset_datacache( CDataCache *pDataCache )
{
	int32_t iRetCode = -1;
	
	if ( pDataCache )
	{
		lock( &(pDataCache->Locker) );		
		
		if ( pDataCache->pDataBuf )
		{
			mem_free( pDataCache->pDataBuf );
			pDataCache->pDataBuf = NULL;	
		}
		
		memset( pDataCache, 0x00, sizeof(*pDataCache) );
		
		iRetCode = 0;
		
		unlock( &(pDataCache->Locker) );
	}
	
	return iRetCode;	
}

//get data space size.
int32_t data_of_datacache( const CDataCache *pDataCache )
{
	int32_t iRetCode = -1;
	
	if ( pDataCache )
	{
		lock( &(pDataCache->Locker) );

		iRetCode = pDataCache->iCurrentDataBufLen;
		
		unlock( &(pDataCache->Locker) );
	}
	
	return iRetCode;
}

//free space .
int32_t free_of_datacache( const CDataCache *pDataCache )
{
	int32_t iRetCode = -1;
	
	if ( pDataCache )
	{
		lock( &(pDataCache->Locker) );

		iRetCode = pDataCache->iMaxDataBufSize - pDataCache->iCurrentDataBufLen - 1;
		
		unlock( &(pDataCache->Locker) );
	}
	
	return iRetCode;
}

//write datacache.
int32_t write_datacache( CDataCache *pDataCache, const int8u_t *pData, const int32_t iDataLen )
{
	int32_t iRetCode = -1;
	
	if ( pDataCache && pData && iDataLen > 0 )
	{
		lock( &(pDataCache->Locker) );

		if (( (pDataCache->iRear + 1) % pDataCache->iMaxDataBufSize) == pDataCache->iFront )
		{
			//cache is full.

		}
		else 
		{
			if ( pDataCache->iRear >= pDataCache->iFront )
			{
				//data cache is available.
				//0                                                                   99
				//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
				//          front(21)
				//          rear(21)
				int32_t iLeftFreeBytes = pDataCache->iFront, 
							iRightLeftFreeBytes = pDataCache->iMaxDataBufSize - pDataCache->iRear - 1;
				
				int32_t iNLeft = iDataLen, iNWrite = 0;
				int8u_t *pTempData = pData;
				
				iRetCode = 0;
				
				if ( iRightLeftFreeBytes > 0 )
				{
					//right write.
					if ( iRightLeftFreeBytes >= iNLeft )
					{
						iNWrite = iNLeft;
					
					}
					else 
						iNWrite = iRightLeftFreeBytes;
					
					memcpy( pDataCache->pDataBuf + (pDataCache->iRear + 1) % pDataCache->iMaxDataBufSize, pTempData, iNWrite );
					pDataCache->iRear += (iNWrite);
					pDataCache->iRear %= pDataCache->iMaxDataBufSize;
					
					iNLeft -= iNWrite;
					iRetCode += iNWrite;
					pTempData += iNWrite;
					
					pDataCache->iCurrentDataBufLen += iNWrite;
					
					log_print( "write1:iCurrentDataBufLen-->%d, Front-->%d, Rear-->%d", 
									pDataCache->iCurrentDataBufLen, pDataCache->iFront, pDataCache->iRear );
					
					iNWrite = 0;
				}
				
				//left write.
				if ( iNLeft > 0 && iLeftFreeBytes > 0 )
				{
					if ( iNLeft <= iLeftFreeBytes )
					{
						iNWrite = iNLeft;
					}
					else 
					{
						iNWrite = iLeftFreeBytes;
					}
					
					memcpy( pDataCache->pDataBuf + (pDataCache->iRear + 1) % pDataCache->iMaxDataBufSize, pTempData, iNWrite );
					pDataCache->iRear += (iNWrite);
					pDataCache->iRear %= pDataCache->iMaxDataBufSize;
					
					iNLeft -= iNWrite;
					iRetCode += iNWrite;
					pTempData += iNWrite;
					
					pDataCache->iCurrentDataBufLen += iNWrite;
					
					log_print( "write2:iCurrentDataBufLen-->%d, Front-->%d, Rear-->%d", 
									pDataCache->iCurrentDataBufLen, pDataCache->iFront, pDataCache->iRear );
					
					iNWrite = 0;
				}
			}
			else 
			{
				//0                                                                   100
				//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
				//                                 front(50)
				//             rear(20)
				int32_t iLeftFreeBytes = pDataCache->iFront - pDataCache->iRear;
				int32_t iNLeft = iDataLen, iNWrite = 0;
				int8u_t *pTempData = pData;
				
				iRetCode = 0;
				if ( iLeftFreeBytes > 0 )
				{
					if ( iLeftFreeBytes >= iNLeft )
						iNWrite = iNLeft;
					else 
						iNWrite = iLeftFreeBytes;
					
					memcpy( pDataCache->pDataBuf + (pDataCache->iRear + 1) % pDataCache->iMaxDataBufSize, pTempData, iNWrite );
					pDataCache->iRear += (iNWrite);
					pDataCache->iRear %= pDataCache->iMaxDataBufSize;
					
					iNLeft -= iNWrite;
					iRetCode += iNWrite;
					pTempData += iNWrite;
					
					pDataCache->iCurrentDataBufLen += iNWrite;
					
					log_print( "write3:iCurrentDataBufLen-->%d, Front-->%d, Rear-->%d", 
									pDataCache->iCurrentDataBufLen, pDataCache->iFront, pDataCache->iRear );
					
					iNWrite = 0;
				}
			}
		}
		
		unlock( &(pDataCache->Locker) );	
	}
	
	return iRetCode;
}

//read datacache.
int32_t read_datacache( CDataCache *pDataCache, int8u_t *pData, const int32_t iDataBufLen )
{
	int32_t iRetCode = -1;
	
	if ( pDataCache && pData && iDataBufLen > 0 )
	{
		lock( &(pDataCache->Locker) );

		if ( pDataCache->iRear == pDataCache->iFront )
		{
			//data cache is empty.	
			
			log_print( "read datacache: cache is empty????????????????????????" );
			
			
		}
		else 
		{
			//has data in cache.
			if ( pDataCache->iRear < pDataCache->iFront )
			{
				//0                                                                   100
				//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
				//                       front(32)
				//          rear(21)
				int32_t iRightAvailableBytes = pDataCache->iMaxDataBufSize - pDataCache->iFront - 1, 
							iLeftAvailableBytes = pDataCache->iRear + 1;
				int32_t iNLeft = iDataBufLen, iNRead = 0;
				int8u_t *pTempData = pData;
				
				iRetCode = 0;
				
				if ( iRightAvailableBytes > 0 )
				{
					if ( iRightAvailableBytes >= iNLeft )
						iNRead = iNLeft;
					else 
						iNRead = iRightAvailableBytes;
						
					memcpy( pDataCache->pDataBuf + (pDataCache->iFront + 1) % pDataCache->iMaxDataBufSize, pTempData, iNRead );
					pDataCache->iFront += (iNRead);
					pDataCache->iFront %= pDataCache->iMaxDataBufSize;
					
					iNLeft -= iNRead;
					iRetCode += iNRead;
					pTempData += iNRead;
					
					pDataCache->iCurrentDataBufLen -= iNRead;
					
					log_print( "read1:iCurrentDataBufLen-->%d, Front-->%d, Rear-->%d", 
									pDataCache->iCurrentDataBufLen, pDataCache->iFront, pDataCache->iRear );
					
					iNRead = 0;
				}
				
				if ( iLeftAvailableBytes > 0 && iNLeft > 0 )
				{
					if ( iLeftAvailableBytes >= iNLeft )
						iNRead = iNLeft;
					else 
						iNRead = iLeftAvailableBytes;
					
					memcpy( pDataCache->pDataBuf + (pDataCache->iFront + 1) % pDataCache->iMaxDataBufSize, pTempData, iNRead );
					pDataCache->iFront += (iNRead);
					pDataCache->iFront %= pDataCache->iMaxDataBufSize;
					
					iNLeft -= iNRead;
					iRetCode += iNRead;
					pTempData += iNRead;
					
					pDataCache->iCurrentDataBufLen -= iNRead;
					
					log_print( "read2:iCurrentDataBufLen-->%d, Front-->%d, Rear-->%d", 
									pDataCache->iCurrentDataBufLen, pDataCache->iFront, pDataCache->iRear );
					
					iNRead = 0;
				}
			}/* if ( pDataCache->iRear < pDataCache->iFront ) */
			else 
			{
				//0                                                                   100
				//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
				//              front(12)
				//                            rear(21)
				int32_t iAvailableBytes = 	pDataCache->iRear - pDataCache->iFront;
				int32_t iNLeft = iDataBufLen, iNRead = 0;
				int8u_t *pTempData = pData;
				
				iRetCode = 0;
				
				if ( iAvailableBytes > 0 )
				{
					if ( iAvailableBytes >= iNLeft )
						iNRead = iNLeft;
					else 
						iNRead = iAvailableBytes;
						
					memcpy( pDataCache->pDataBuf + (pDataCache->iFront + 1) % pDataCache->iMaxDataBufSize, pTempData, iNRead );
					pDataCache->iFront += (iNRead);
					pDataCache->iFront %= pDataCache->iMaxDataBufSize;
					
					iNLeft -= iNRead;
					iRetCode += iNRead;
					pTempData += iNRead;
					
					pDataCache->iCurrentDataBufLen -= iNRead;
					
					log_print( "read3:iCurrentDataBufLen-->%d, Front-->%d, Rear-->%d", 
									pDataCache->iCurrentDataBufLen, pDataCache->iFront, pDataCache->iRear );
					
					iNRead = 0;
				}
				
			}
		}
		
		unlock( &(pDataCache->Locker) );	
	}
	else 
		log_print( "read datacache:param is invalid??????????????????????" );
	
	return iRetCode;
}
