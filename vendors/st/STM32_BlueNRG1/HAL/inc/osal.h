/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : osal.h
* Author             : AMS - VMA RF Application team 
* Version            : V1.0.0
* Date               : 28-Sept-2015
* Description        : This header file defines the OS abstraction layer used by
*                      the BLE stack. OSAL defines the set of functions
*                      which needs to be ported to target operating system and
*                      target platform.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __OSAL_H__
#define __OSAL_H__

/******************************************************************************
 * Includes
 *****************************************************************************/

/******************************************************************************
 * Macros
 *****************************************************************************/


/******************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * This function copies size number of bytes from a 
 * memory location pointed by src to a destination 
 * memory location pointed by dest
 * 
 * @param[in] dest Destination address
 * @param[in] src  Source address
 * @param[in] size size in the bytes  
 * 
 * @return  Address of the destination
 */
 
extern void* Osal_MemCpy(void *dest,const void *src, unsigned int size);

/**
 * This function sets first number of bytes, specified
 * by size, to the destination memory pointed by ptr 
 * to the specified value
 * 
 * @param[in] ptr    Destination address
 * @param[in] value  Value to be set
 * @param[in] size   Size in the bytes  
 * 
 * @return  Address of the destination
 */
 
extern void* Osal_MemSet(void *ptr, int value, unsigned int size);

/**
 * This function compares n bytes of two regions of memory
 * 
 * @param[in] s1    First buffer to compare.
 * @param[in] s2    Second buffer to compare.
 * @param[in] size  Number of bytes to compare.   
 * 
 * @return  0 if the two buffers are equal, 1 otherwise
 */
extern int Osal_MemCmp(void *s1,void *s2,unsigned int size);
#endif /* __OSAL_H__ */
