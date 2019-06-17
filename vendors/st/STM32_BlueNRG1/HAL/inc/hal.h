/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : hal.h
* Author             : AMS - HEA&RF BU
* Version            : V1.0.0
* Date               : 19-July-2012
* Description        : Header file which defines Hardware abstraction layer APIs
*                       used by the BLE stack. It defines the set of functions
*                       which needs to be ported to the target platform.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef __HAL_H__
#define __HAL_H__

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <hal_types.h>
#include <ble_status.h>
/******************************************************************************
 * Macros
 *****************************************************************************/
/* Little Endian buffer to host endianess conversion */
#define LE_TO_HOST_16(ptr)  (uint16_t) ( ((uint16_t) \
                                           *((uint8_t *)ptr)) | \
                                          ((uint16_t) \
                                           *((uint8_t *)ptr + 1) << 8 ) )

#define LE_TO_HOST_32(ptr)   (uint32_t) ( ((uint32_t) \
                                           *((uint8_t *)ptr)) | \
                                           ((uint32_t) \
                                            *((uint8_t *)ptr + 1) << 8)  | \
                                           ((uint32_t) \
                                            *((uint8_t *)ptr + 2) << 16) | \
                                           ((uint32_t) \
                                            *((uint8_t *)ptr + 3) << 24) )

/* Big Endian buffer to host endianess conversion */
#define BE_TO_HOST_16(ptr)  (uint16_t) ( ((uint16_t) \
                                           *((uint8_t *)ptr)) << 8 | \
                                          ((uint16_t) \
                                           *((uint8_t *)ptr + 1) ) )
											
/* Store Value into a buffer in Little Endian Format */
#define HOST_TO_LE_16(buf, val)    ( ((buf)[0] =  (uint8_t) (val)    ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8) ) )

#define HOST_TO_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8)  ) , \
                                   ((buf)[2] =  (uint8_t) (val>>16) ) , \
                                   ((buf)[3] =  (uint8_t) (val>>24) ) ) 


/* Store Value into a buffer in Big Endian Format */
#define HOST_TO_BE_16(buf, val)    ( ((buf)[1] =  (uint8_t) (val)    ) , \
                                   ((buf)[0] =  (uint8_t) (val>>8) ) )

#define DISABLE_INTERRUPTS() __disable_interrupt()
#define ENABLE_INTERRUPTS() __enable_interrupt()
#define SAVE_PRIMASK() uint32_t uwPRIMASK_Bit = __get_PRIMASK()
#define ATOMIC_SECTION_BEGIN() uint32_t uwPRIMASK_Bit = __get_PRIMASK(); \
                                __disable_interrupt(); \
/* Must be called in the same or in a lower scope of SUSPEND_INTERRUPTS */
#define ATOMIC_SECTION_END() __set_PRIMASK(uwPRIMASK_Bit)
 
/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * Writes data to a serial interface.
 *
 * @param[in]  data1    1st buffer
 * @param[in]  data2    2nd buffer
 * @param[in]  n_bytes1 number of bytes in 1st buffer
 * @param[in]  n_bytes2 number of bytes in 2nd buffer
 */
void Hal_Write_Serial(const void* data1, const void* data2, uint16_t n_bytes1, uint16_t n_bytes2);

/**
 * Enable interrupts from HCI controller.
 */
void Enable_SPI_IRQ(void);

/**
 * Disable interrupts from BLE controller.
 */
void Disable_SPI_IRQ(void);

void Hal_Init_Timer();
uint32_t Hal_Get_Timer_Value();
void Hal_Start_Timer(uint32_t timeout);
void Hal_Stop_Timer();

#endif /* __HAL_H__ */
