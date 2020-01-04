/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the platform abstraction layer APIs for gpio.
*
* \ingroup  grPAL
* @{
*/


/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/
#include "optiga/pal/pal_gpio.h"
#include "driver/gpio.h"

/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * LOCAL DATA
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * LOCAL ROUTINES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * API IMPLEMENTATION
 *********************************************************************************************************************/

/**
* Sets the gpio pin to high state
* 
* <b>API Details:</b>
*      The API sets the pin high, only if the pin is assigned to a valid gpio context.<br>
*      Otherwise the API returns without any faliure status.<br>
*
*\param[in] p_gpio_context Pointer to pal layer gpio context
*
*
*/
void pal_gpio_set_high(const pal_gpio_t* p_gpio_context)
{
	if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))     
    {
		gpio_set_level((gpio_num_t)p_gpio_context->p_gpio_hw, 1);
	}
}

/**
* Sets the gpio pin to low state
*
* <b>API Details:</b>
*      The API set the pin low, only if the pin is assigned to a valid gpio context.<br>
*      Otherwise the API returns without any faliure status.<br>
* 
*\param[in] p_gpio_context Pointer to pal layer gpio context
*
*/
void pal_gpio_set_low(const pal_gpio_t* p_gpio_context)
{
    if ((p_gpio_context != NULL) && (p_gpio_context->p_gpio_hw != NULL))     
    {
        gpio_set_level((gpio_num_t)p_gpio_context->p_gpio_hw, 0);
    }
}

/**
* @}
*/

