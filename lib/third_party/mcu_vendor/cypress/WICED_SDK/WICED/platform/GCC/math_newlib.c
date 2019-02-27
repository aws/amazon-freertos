/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */


#ifndef WICED_DISABLE_MATH_NEWLIB
/**
 *  Sine function
 *
 *  Simple Taylor series approximation of sine function
 *  Since Newlib doesn't contain one
 *  see http://www.wolframalpha.com/input/?i=taylor+series+sin+x
 *
 * @param x : the value for which Sine will be computed
 *
 * @return  the Sine of x
 */
#define PI (3.1415926)

double sin( double x )
{
    int term = 1;
    double val = 0;

    x -= ( (int) ( x / ( 2 * PI ) ) ) * 2 * PI;

    if ( x > PI )
    {
        x -= 2 * PI;
    }
    if ( x < -PI )
    {
        x += 2 * PI;
    }

    for ( term = 0; term < 6; term++ )
    {
        double multval = x;
        double denval = 1;
        int ex;
        for ( ex = 0; ex < ( term * 2 ); ex++ )
        {
            multval *= x;
            denval *= ( ex + 2 );
        }
        val += ( ( term % 2 == 1 ) ? -1 : 1 ) * multval / denval;
    }
    return val;
}

/* Simple Taylor series approximation of natural logarithm function
 * see http://www.efunda.com/math/taylor_series/logarithmic.cfm
 */

double log( double x )
{
    int term = 1;
    double val = 0;

    for ( term = 1; term < 5; term++ )
    {
        double multval = ( x - 1.0 ) / ( x + 1.0 );
        int ex;
        for ( ex = 0; ex < ( term * 2 - 2 ); ex++ )
        {
            multval *= multval;
        }
        val += multval / ( term * 2 - 1 );
    }
    return 2 * val;
}
#endif
