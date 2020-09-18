/* Include Unity header */
#include <unity.h>

/* Include standard libraries */
#include <stdlib.h>
#include <string.h>

/* Include header file(s) which have declaration
 * of functions under test */
#include "FreeRTOS_IP.h"
#include "mock_FreeRTOS_ARP.h"
#include "FreeRTOS_IP_Private.h"

#include "FreeRTOSIPConfig.h"

#include "FreeRTOS_ARP_stubs.c"

#define ARPCacheEntryToCheck  2

#if ARPCacheEntryToCheck >= ipconfigARP_CACHE_ENTRIES
	#error "ARPCacheEntryToCheck cannot be greater than ipconfigARP_CACHE_ENTRIES"
#endif

extern ARPCacheRow_t xARPCache[ ipconfigARP_CACHE_ENTRIES ];

void FillARPCache( void )
{
    int i,j;
    for( i =0; i < ipconfigARP_CACHE_ENTRIES; i++ )
    {
	for( j = 0; j < ipMAC_ADDRESS_LENGTH_BYTES; j++ )
	{
            xARPCache[ i ].xMACAddress.ucBytes[ j ] = i*10 + j;
	}

        xARPCache[ i ].ulIPAddress = i;
    }
}

void test_ulARPRemoveCacheEntryByMac_RemoveNormalEntry( void )
{
    int32_t lResult;
    uint8_t offset = ARPCacheEntryToCheck * 10;
    const MACAddress_t pxMACAddress = { offset + 0, offset + 1, offset + 2, offset + 3, offset + 4, offset + 5 };

    FillARPCache();

    lResult = ulARPRemoveCacheEntryByMac( &pxMACAddress );

    TEST_ASSERT_EQUAL( lResult, ARPCacheEntryToCheck );
}

void test_ulARPRemoveCacheEntryByMac_RemoveAbsentEntry( void )
{
    int32_t lResult;
    uint8_t offset = ARPCacheEntryToCheck * 10;
    const MACAddress_t pxMACAddress = { offset + 6, offset + 7, offset + 8, offset + 9, offset + 10, offset + 11 };

    FillARPCache();

    lResult = ulARPRemoveCacheEntryByMac( &pxMACAddress );

    TEST_ASSERT_EQUAL( lResult, 0 );
}

void test_ulARPRemoveCacheEntryByMac_UseNULLPointer( void )
{
    /* We expect this test to call ASSERT. */
    ulARPRemoveCacheEntryByMac( NULL );
}

void test_eARPGetCacheEntryByMac_GetNormalEntry( void )
{
    uint32_t ulIPPointer = 12345;
    eARPLookupResult_t xResult;
    uint8_t offset = ARPCacheEntryToCheck * 10;
    MACAddress_t xMACAddress = { offset + 0, offset + 1, offset + 2, offset + 3, offset + 4, offset + 5 };
    MACAddress_t * const pxMACAddress = &xMACAddress;

    FillARPCache();

    xResult = eARPGetCacheEntryByMac( pxMACAddress, &ulIPPointer );

    TEST_ASSERT_EQUAL( xResult, eARPCacheHit );
    TEST_ASSERT_EQUAL( ulIPPointer, ARPCacheEntryToCheck );
}

void test_eARPGetCacheEntryByMac_GetAbsentEntry( void )
{
    uint32_t ulIPPointer = 12345;
    eARPLookupResult_t xResult;
    uint8_t offset = ARPCacheEntryToCheck * 10;
    MACAddress_t xMACAddress = { offset + 5, offset + 4, offset + 3, offset + 2, offset + 1, offset + 0 };
    MACAddress_t * const pxMACAddress = &xMACAddress;

    FillARPCache();

    xResult = eARPGetCacheEntryByMac( pxMACAddress, &ulIPPointer );

    TEST_ASSERT_EQUAL( xResult, eARPCacheMiss );
    TEST_ASSERT_EQUAL( ulIPPointer, 12345 );
}

void test_eARPGetCacheEntryByMac_UseNULLPointer( void )
{
    uint32_t * ulIPPointer = NULL;
    MACAddress_t * const pxMACAddress = NULL;

    /* Expect this test to his an ASSERT. */
    eARPGetCacheEntryByMac( pxMACAddress, ulIPPointer );
}
