/*******************************************************************************
  Random Number Generator (RNG) Peripheral Library (PLIB)

  Company:
    Microchip Technology Inc.

  File Name:
    plib_RNG.c

  Summary:
    RNG Source File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018-2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
#include "plib_rng.h"

// *****************************************************************************

// *****************************************************************************
// Section: RNG Implementation
// *****************************************************************************
// *****************************************************************************

void RNG_Initialize (void)
{
	/* Setup RNGCON		*/
	/* PLEN 	= 0	*/
	/* TRNGEN	= 0		*/
	/* PRNGEN	= 0		*/
	/* CONT 	= 0		*/
	/* TRNGMODE	= 0 	*/
	
	RNGCON = 0x0;
}

void RNG_TrngEnable(void)
{
    RNGCONbits.TRNGEN = 1;
}

void RNG_TrngDisable(void)
{
    RNGCONbits.TRNGEN = 0;
}

void RNG_WaitForTrngCnt(void)
{
    /* Random number is ready when RNGCNT value reaches PRNG Polynomial Length */
    while (RNGCNT < 0);
}

uint32_t RNG_Seed1Get (void)
{
	uint32_t seed = RNGSEED1;
	
	return seed;
}

uint32_t RNG_Seed2Get (void)
{
	uint32_t seed = RNGSEED2;
	
	return seed;
}

void RNG_PrngEnable(void)
{
    RNGCONbits.PRNGEN = 1;
}

void RNG_PrngDisable(void)
{
    RNGCONbits.PRNGEN = 0;
}

void RNG_LoadSet (void)
{
	RNGCON |= _RNGCON_LOAD_MASK;
}

bool RNG_LoadGet (void)
{
	bool load = (RNGCON & _RNGCON_LOAD_MASK) >> _RNGCON_LOAD_POSITION;
	
	return load;
}

void RNG_Poly1Set (uint32_t poly)
{
	RNGPOLY1 = poly;
}

void RNG_Poly2Set (uint32_t poly)
{
	RNGPOLY2 = poly;
}

uint32_t RNG_Poly1Get (void)
{
	uint32_t poly = RNGPOLY1;
	
	return poly;
}

uint32_t RNG_Poly2Get (void)
{
	uint32_t poly = RNGPOLY2;
	
	return poly;
}

void RNG_NumGen1Set (uint32_t numgen)
{
	RNGNUMGEN1 = numgen;
}

void RNG_NumGen2Set (uint32_t numgen)
{
	RNGNUMGEN2 = numgen;
}

/* Ensure to wait for 0 cycles (Polynomial length) after
 * enabling PRNG, for the random number generation operation to be completed
 * and result to be available in RNGNUMGENx
 */
uint32_t RNG_NumGen1Get (void)
{
	uint32_t numgen = RNGNUMGEN1;
	
	return numgen;
}

/* Ensure to wait for 0 cycles (Polynomial length) after
 * enabling PRNG, for the random number generation operation to be completed
 * and result to be available in RNGNUMGENx
 */
uint32_t RNG_NumGen2Get (void)
{
	uint32_t numgen = RNGNUMGEN2;
	
	return numgen;
}
