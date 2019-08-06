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
#include "semphr.h"
#include "task.h"
/*
 * Get Random number generator.
 */

/* The random number generation solution presented in this file is
* for demonstration purposes only. It is not recommended to go into production with
* the logic presented here. The current solution takes entropy from ADC white noise signal and uses it
* as input seed to generate a random number from the RNG HW.
* For production development, it is recommended to use a source which will be
* truly random in nature.
*/

#define PRNG_KEY_SIZE  (0x20UL)
 
static volatile int  g_PRNG_done;
volatile int  g_AES_done;


#define SNUM        32       /* recorded number of lastest samples */


static uint32_t   adc_val[SNUM];
static uint32_t   val_sum;
static int        oldest;

static SemaphoreHandle_t xTrngMutex = NULL;

#ifdef __ICCARM__
#define __inline   inline
#endif

static __inline uint32_t  get_adc_bg_val()
{
    EADC->SWTRG = (1 << 16);      //Trigger Band-gap
    while ((EADC->STATUS1 & 1) != 1);
    return (EADC->DAT[16] & 0xFFF);
}

int  adc_trng_gen_bit()
{
    uint32_t   new_val, average;
    int        ret_val;
    /* Internal full scale voltage 3.3v, internal measure voltage 1.2v  */
    /* ADC 12 bits, it's resolution 0~4095 */
    /* To filter peak, set sampling data range as -50 ~ +50 */    
    int ref_bg_ub = ((1.2/3.3)*4095) + 50;
    int ref_bg_lb = ((1.2/3.3)*4095) - 50;
    
    do{
      new_val = get_adc_bg_val();
    } while( (new_val > ref_bg_ub) || (new_val < ref_bg_lb) );

    average = (val_sum / SNUM);   /* sum divided by 32 */

    if (average >= new_val)
        ret_val = 1;
    else
        ret_val = 0;

    //printf("%d - sum = 0x%x, avg = 0x%x, new = 0x%x\n", oldest, val_sum, average, new_val);

    /* kick-off the oldest one and insert the new one */
    val_sum -= adc_val[oldest];
    val_sum += new_val;
    adc_val[oldest] = new_val;
    oldest = (oldest + 1) % SNUM;

    return ret_val;
}


uint32_t  adc_trng_gen_rnd()
{
    int       i;
    uint32_t  val32;

    val32 = 0;
    for (i = 31; i >= 0; i--)
        val32 |= (adc_trng_gen_bit() << i);
    //printf("### %s: 0x%x \n", __FUNCTION__, val32);

    return val32;
}

static void  adc_init()
{
    static uint8_t init_flag = FALSE;
    int    i;
    
    if( init_flag ) return;
  
    /* ADC refernce external 1.2V */
    /* Unlock protected registers */
    //SYS_UnlockReg();    
    //SYS->VREFCTL = SYS_VREFCTL_VREF_PIN;
    /* Lock protected registers */
    //SYS_LockReg();
  
    /* Enable EADC clock */
    CLK->APBCLK0 |= CLK_APBCLK0_EADCCKEN_Msk;

    /* Set EADC clock divider */
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_EADCDIV_Msk;
    CLK->CLKDIV0 |= (5 << CLK_CLKDIV0_EADCDIV_Pos);

    EADC->CTL = (0x3 << EADC_CTL_RESSEL_Pos) | EADC_CTL_ADCEN_Msk;        /* A/D Converter Enable, select 12-bit ADC result  */

    while (!(EADC->PWRM & EADC_PWRM_PWUPRDY_Msk));

    EADC->SCTL[16] = (0x70 << EADC_SCTL_EXTSMPT_Pos)  /* ADC Sampling Time Extend          */
                     | (0x0 << EADC_SCTL_TRGSEL_Pos);      /* A/D SAMPLE Start of Conversion Trigger Source Selection */

    val_sum = 0;
    for (i = 0; i < SNUM; i++)
    {
        adc_val[i] = get_adc_bg_val();
        val_sum += adc_val[i];
    }
    oldest = 0;
    init_flag = TRUE;
    adc_trng_gen_rnd();    // drop the first 32-bits
}

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

//extern void *pxCurrentTCB;

static void trng_get(unsigned char *pConversionData)
{
	uint32_t *p32ConversionData;
    uint32_t u32val;
  
	p32ConversionData = (uint32_t *)pConversionData;

    if( xSemaphoreTake( xTrngMutex, ( TickType_t ) 10 ) == pdTRUE ) {

        //printf("### SemaphoreTake 0x%x\r\n", pxCurrentTCB);    
        PRNG_ENABLE_INT(CRPT);
	
        u32val = adc_trng_gen_rnd();
        //printf("=== %s: 0x%x \n", __FUNCTION__, u32val);
        PRNG_Open(CRPT, PRNG_KEY_SIZE_256, 1, u32val); //adc_trng_gen_rnd());

        PRNG_Start(CRPT);
        while (!g_PRNG_done);

        PRNG_Read(CRPT, p32ConversionData);

        PRNG_DISABLE_INT(CRPT);
        //printf("### SemaphoreGive 0x%x\r\n", pxCurrentTCB);        
        xSemaphoreGive( xTrngMutex );   
    }
}


static BaseType_t trng_init()
{
    static BaseType_t init_done = pdFALSE;
    
    taskENTER_CRITICAL();
    if( xTrngMutex == NULL ) {
        xTrngMutex = xSemaphoreCreateMutex();
        if( xTrngMutex == NULL) return pdFALSE;
    }
    taskEXIT_CRITICAL();

    if( xSemaphoreTake( xTrngMutex, ( TickType_t ) 10 ) == pdTRUE ) {
        if( init_done != pdTRUE ) {
            adc_init();
            /* Unlock protected registers */
            SYS_UnlockReg();	
            /* Enable IP clock */
            CLK_EnableModuleClock(CRPT_MODULE);
	
            /* Lock protected registers */
            SYS_LockReg();	
	
            NVIC_EnableIRQ(CRPT_IRQn);    
            init_done = pdTRUE;
        }
        xSemaphoreGive( xTrngMutex );
        return pdTRUE;
    } else {
        return pdFALSE;
    }
}

/*
 * Get len bytes of entropy from the hardware RNG.
 */
 
int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    /* Enabling ADC and sampling the internal voltage to come out one random seed,
    * then input this seed to HW PRNG to generate one random number.
    * This random seed is the combination of sequence of ADC band-gap white noise.
    * Current mechanism could pass the NIST certification suit.
    */
    unsigned char tmpBuff[PRNG_KEY_SIZE];
    size_t cur_length = 0;
    *olen = 0;
    ((void) data);

    if( trng_init() == pdFALSE ) return (-1);
    
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
	
    return( 0 );
}
 
#if 1
uint32_t numaker_ulRand( void )
{
	unsigned char tmpBuff[PRNG_KEY_SIZE];
  if( trng_init() == pdFALSE ) return (-1);
	trng_get(tmpBuff);
	return *((uint32_t*)tmpBuff);
}
#endif

