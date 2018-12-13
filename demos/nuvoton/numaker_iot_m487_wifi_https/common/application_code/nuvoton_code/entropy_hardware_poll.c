/*
 *  Hardware entropy collector for M487 RNG
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "NuMicro.h"

/*
 * Get Random number generator.
 */
 #define PRNG_KEY_SIZE  (0x20UL)
 
static volatile int  g_PRNG_done;
volatile int  g_AES_done;

void CRYPTO_IRQHandler()
{
    if (PRNG_GET_INT_FLAG(CRPT)) {
        g_PRNG_done = 1;
        PRNG_CLR_INT_FLAG(CRPT);
    } else	if (AES_GET_INT_FLAG(CRPT)) {
        g_AES_done = 1;
        AES_CLR_INT_FLAG(CRPT);
    }

} 

static void trng_get(unsigned char *pConversionData)
{
	uint32_t *p32ConversionData;

	p32ConversionData = (uint32_t *)pConversionData;
	
    /* Unlock protected registers */
    SYS_UnlockReg();	
    /* Enable IP clock */
    CLK_EnableModuleClock(CRPT_MODULE);
	
    /* Lock protected registers */
    SYS_LockReg();	
	
    NVIC_EnableIRQ(CRPT_IRQn);
    PRNG_ENABLE_INT(CRPT);
	
//	PRNG_Open(PRNG_KEY_SIZE_64, 0, 0);
	PRNG_Open(CRPT, PRNG_KEY_SIZE_256, 1, xTaskGetTickCount());

    PRNG_Start(CRPT);
    while (!g_PRNG_done);

    PRNG_Read(CRPT, p32ConversionData);

//    printf("    0x%08x  0x%08x  0x%08x  0x%08x\n\r", *p32ConversionData, *(p32ConversionData+1), *(p32ConversionData+2), *(p32ConversionData+3));
//    printf("    0x%08x  0x%08x  0x%08x  0x%08x\n\r", *(p32ConversionData+4), *(p32ConversionData+5), *(p32ConversionData+6), *(p32ConversionData+7));

    PRNG_DISABLE_INT(CRPT);
///    NVIC_DisableIRQ(CRPT_IRQn);
 //    CLK_DisableModuleClock(CRPT_MODULE);
		
}


/*
 * Get len bytes of entropy from the hardware RNG.
 */
 
int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
#if 0
    unsigned long timer = xTaskGetTickCount();
	  ((void) data);
    *olen = 0;
 
    if( len < sizeof(unsigned long) )
        return( 0 );
 
    memcpy( output, &timer, sizeof(unsigned long) );
    *olen = sizeof(unsigned long);
#else
    unsigned char tmpBuff[PRNG_KEY_SIZE];
    size_t cur_length = 0;
    ((void) data);
    
    while (len >= sizeof(tmpBuff)) {
        trng_get(output);
        output += sizeof(tmpBuff);
        cur_length += sizeof(tmpBuff);
        len -= sizeof(tmpBuff);
    }
    if (len > 0) {
        trng_get(tmpBuff);
        memcpy(output, tmpBuff, len);
        cur_length += len;
    }
    *olen = cur_length;
#endif
	
    return( 0 );
}
 
#if 1
uint32_t numaker_ulRand( void )
{
	unsigned char tmpBuff[PRNG_KEY_SIZE];
	trng_get(tmpBuff);
	return *((uint32_t*)tmpBuff);
}
#endif

