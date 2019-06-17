#ifdef __GNUC__
/******************************************************************************
*
* Copyright (C) 2012 - 2014 Xilinx, Inc.  All rights reserved.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal 
* in the Software without restriction, including without limitation the rights 
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications: 
* (a) running on a Xilinx device, or 
* (b) that interact with a Xilinx device through a bus or interconnect.  
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in 
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file handoff.S
*
* Contains the code that does the handoff to the loaded application. This
* code lives high in the ROM. 
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date.word	Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	03/01/10 Initial release
* 7.00a kc	10/23/13 Added support for armcc compiler
* </pre>
*
* @note
* Assumes that the starting address of the FSBL is provided by the calling routine
* in R0.
*
******************************************************************************/

.globl FsblHandoffJtagExit

.globl FsblHandoffExit

.section .handoff,"axS"

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

FsblHandoffJtagExit:
		mcr	 15,0,r0,cr7,cr5,0		/* Invalidate Instruction cache */
		mcr	 15,0,r0,cr7,cr5,6		/* Invalidate branch predictor array */

		dsb
		isb					/* make sure it completes */

	ldr	r4, =0
		mcr	 15,0,r4,cr1,cr0,0		/* disable the ICache and MMU */

		isb					/* make sure it completes */
Loop:
	wfe
	b Loop

FsblHandoffExit:
		mov	 lr, r0	/* move the destination address into link register */

		mcr	 15,0,r0,cr7,cr5,0		/* Invalidate Instruction cache */
		mcr	 15,0,r0,cr7,cr5,6		/* Invalidate branch predictor array */

		dsb
		isb					/* make sure it completes */

	ldr	r4, =0
		mcr	 15,0,r4,cr1,cr0,0		/* disable the ICache and MMU */

		isb					/* make sure it completes */


		bx		lr	/* force the switch, destination should have been in r0 */

.Ldone: b		.Ldone					/* Paranoia: we should never get here */
.end

#elif defined (__IASMARM__)
	
	PUBLIC FsblHandoffJtagExit
	
	PUBLIC FsblHandoffExit
	
	SECTION .handoff:CODE:NOROOT(2)

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

FsblHandoffJtagExit
		mcr	 p15,0,r0,c7,c5,0		;/* Invalidate Instruction cache */
		mcr	 p15,0,r0,c7,c5,6		;/* Invalidate branch predictor array */

		dsb
		isb							;/* make sure it completes */

	ldr	r4, =0
		mcr	 p15,0,r4,c1,c0,0		;/* disable the ICache and MMU */

		isb							;/* make sure it completes */
Loop
	wfe
	b Loop

FsblHandoffExit
		mov	 lr, r0					;/* move the destination address into link register */

		mcr	 p15,0,r0,c7,c5,0		;/* Invalidate Instruction cache */
		mcr	 p15,0,r0,c7,c5,6		;/* Invalidate branch predictor array */

		dsb
		isb							;/* make sure it completes */

	ldr	r4, =0
		mcr	 p15,0,r4,c1,c0,0		;/* disable the ICache and MMU */

		isb							;/* make sure it completes */


		bx		lr					;/* force the switch, destination should have been in r0 */

.Ldone 
	b		.Ldone				;/* Paranoia: we should never get here */

	END


#else
	EXPORT FsblHandoffJtagExit

	EXPORT FsblHandoffExit

	AREA |.handoff|,CODE

;/***************************** Include Files *********************************/

;/************************** Constant Definitions *****************************/

;/**************************** Type Definitions *******************************/

;/***************** Macros (Inline Functions) Definitions *********************/

;/************************** Function Prototypes ******************************/

;/************************** Variable Definitions *****************************/


FsblHandoffJtagExit
		mcr	 p15,0,r0,c7,c5,0		;/* Invalidate Instruction cache */
		mcr	 p15,0,r0,c7,c5,6		;/* Invalidate branch predictor array */

		dsb
		isb							;/* make sure it completes */

	ldr	r4, =0
		mcr	 p15,0,r4,c1,c0,0		;/* disable the ICache and MMU */

		isb							;/* make sure it completes */
Loop
	wfe
	b Loop

FsblHandoffExit
		mov	 lr, r0	;/* move the destination address into link register */

		mcr	 p15,0,r0,c7,c5,0		;/* Invalidate Instruction cache */
		mcr	 p15,0,r0,c7,c5,6		;/* Invalidate branch predictor array */

		dsb
		isb							;/* make sure it completes */

	ldr	r4, =0
		mcr	 p15,0,r4,c1,c0,0		;/* disable the ICache and MMU */

		isb							;/* make sure it completes */


		bx		lr	;/* force the switch, destination should have been in r0 */

Ldone b		Ldone					;/* Paranoia: we should never get here */
	END
#endif
