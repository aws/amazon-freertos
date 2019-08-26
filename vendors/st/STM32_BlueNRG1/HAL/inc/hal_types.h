/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : hal_types.h
* Author             : AMS - HEA&RF BU
* Version            : V1.0.0
* Date               : 19-July-2012
* Description        : This header file defines the basic data types used by the
*                       BLE stack.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef __HAL_TYPES_H__
#define __HAL_TYPES_H__

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 0
#define __BIG_ENDIAN    1
#endif


/* Byte order conversions */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htob(d,n)  (d)
#define btoh(d,n)  (d)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define htob(d,n)  (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#define btoh(d,n)  (d<<24|((d<<8)&0x00ff0000)|((d>>8)&0x0000ff00)|((d>>24)&0x000000ff))
#warning "to be fixed htob btoh"
#else
#error "Unknown byte order"
#endif

typedef uint8_t BOOL;

#ifndef TRUE 
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif



#endif /* __HAL_TYPES_H__ */ 

