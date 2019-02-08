/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : sbrk.c
* Device(s)    : RX
* Description  : Configures the MCU heap memory.  The size of the heap is defined by the macro HEAPSIZE below.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 12.03.2012 1.10     Heap size is now defined in r_bsp_config.h, not sbrk.h.
*         : 19.11.2012 1.20     Updated code to use 'BSP_' and 'BSP_CFG_' prefix for macros.
*         : 26.11.2013 1.21     Replaced stddef.h, stdio.h, and stdint.h includes with include for platform.h.
*         : 15.05.2017 1.30     Deleted unnecessary comments.
*         :(15.05.2017 2.00     Deleted unnecessary comments.)
*                               Added the bsp startup module disable function.
*         :(01.11.2017 2.00     Added the bsp startup module disable function.)
*         : xx.xx.xxxx 2.01     Added support for GNUC and ICCRX.
*                               Deleted unnecessary stddef.h, stdio.h, and stdint.h includes.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_bsp_config.h"

/* Only use this file if heap is enabled in r_bsp_config. */
#if (BSP_CFG_HEAP_BYTES > 0)

/* Used for getting BSP_CFG_HEAP_BYTES macro. */
#include "platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

#if defined(__CCRX__) || defined(__GNUC__)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Memory allocation function prototype declaration (CC-RX and GNURX+NEWLIB) */
int8_t  *sbrk(size_t size);
#if defined(__GNUC__)
/* Memory address function prototype declaration (GNURX+OPTLIB only) */
int8_t  *_top_of_heap(void);
#endif /* defined(__GNUC__) */

//const size_t _sbrk_size=      /* Specifies the minimum unit of */
/* the defined heap area */
extern int8_t *_s1ptr;

union HEAP_TYPE
{
    int32_t  dummy;             /* Dummy for 4-byte boundary */
    int8_t heap[BSP_CFG_HEAP_BYTES];    /* Declaration of the area managed by sbrk*/
};
/* Declare memory heap area */
static union HEAP_TYPE heap_area;
/* End address allocated by sbrk (CC-RX and GNURX+NEWLIB) */
static int8_t *brk=(int8_t *)&heap_area;

#if defined(__GNUC__)
/* Start address of allocated heap area (GNURX+OPTLIB only) */
int8_t *_heap_of_memory=(int8_t *)&heap_area;
/* End address of allocated heap area (GNURX+OPTLIB only) */
int8_t *_last_heap_object=(int8_t *)&heap_area;
#endif /* defined(__GNUC__) */

/***********************************************************************************************************************
* Function name: sbrk
* Description  : This function configures MCU memory area allocation. (CC-RX and GNURX+NEWLIB)
* Arguments    : size - 
*                    assigned area size
* Return value : Start address of allocated area (pass)
*                -1 (failure)
***********************************************************************************************************************/
int8_t  *sbrk(size_t size)
{
    int8_t  *p;

    if ((brk + size) > (heap_area.heap + BSP_CFG_HEAP_BYTES))
    {
        /* Empty area size  */
        p = (int8_t *)-1;
    }
    else
    {
        /* Area assignment */
        p = brk;  

        /* End address update */
        brk += size;
    }

    /* Return result */
    return p;
}

#if defined(__GNUC__)
/***********************************************************************************************************************
* Function name: _top_of_heap
* Description  : This function returns end address of reserved heap area. (GNURX+OPTLIB only)
* Arguments    : none
* Return value : End address of reserved heap area
***********************************************************************************************************************/
int8_t *_top_of_heap(void)
{
    return (int8_t *)(heap_area.heap + BSP_CFG_HEAP_BYTES);
}
#endif /* defined(__GNUC__) */

#endif /* defined(__CCRX__), defined(__GNUC__) */

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

#endif /* BSP_CFG_HEAP_BYTES */
