/**
 * \brief Peripheral I/O description for SAME70Q21B
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software and any derivatives
 * exclusively with Microchip products. It is your responsibility to comply with third party license
 * terms applicable to your use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 * APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
 * EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* file generated from device description version 2019-07-24T15:04:36Z */
#ifndef _SAME70Q21B_GPIO_H_
#define _SAME70Q21B_GPIO_H_

/* ========== Peripheral I/O pin numbers ========== */
#define PIN_PA0                     (  0  )  /**< Pin Number for PA0 */
#define PIN_PA1                     (  1  )  /**< Pin Number for PA1 */
#define PIN_PA2                     (  2  )  /**< Pin Number for PA2 */
#define PIN_PA3                     (  3  )  /**< Pin Number for PA3 */
#define PIN_PA4                     (  4  )  /**< Pin Number for PA4 */
#define PIN_PA5                     (  5  )  /**< Pin Number for PA5 */
#define PIN_PA6                     (  6  )  /**< Pin Number for PA6 */
#define PIN_PA7                     (  7  )  /**< Pin Number for PA7 */
#define PIN_PA8                     (  8  )  /**< Pin Number for PA8 */
#define PIN_PA9                     (  9  )  /**< Pin Number for PA9 */
#define PIN_PA10                    ( 10  )  /**< Pin Number for PA10 */
#define PIN_PA11                    ( 11  )  /**< Pin Number for PA11 */
#define PIN_PA12                    ( 12  )  /**< Pin Number for PA12 */
#define PIN_PA13                    ( 13  )  /**< Pin Number for PA13 */
#define PIN_PA14                    ( 14  )  /**< Pin Number for PA14 */
#define PIN_PA15                    ( 15  )  /**< Pin Number for PA15 */
#define PIN_PA16                    ( 16  )  /**< Pin Number for PA16 */
#define PIN_PA17                    ( 17  )  /**< Pin Number for PA17 */
#define PIN_PA18                    ( 18  )  /**< Pin Number for PA18 */
#define PIN_PA19                    ( 19  )  /**< Pin Number for PA19 */
#define PIN_PA20                    ( 20  )  /**< Pin Number for PA20 */
#define PIN_PA21                    ( 21  )  /**< Pin Number for PA21 */
#define PIN_PA22                    ( 22  )  /**< Pin Number for PA22 */
#define PIN_PA23                    ( 23  )  /**< Pin Number for PA23 */
#define PIN_PA24                    ( 24  )  /**< Pin Number for PA24 */
#define PIN_PA25                    ( 25  )  /**< Pin Number for PA25 */
#define PIN_PA26                    ( 26  )  /**< Pin Number for PA26 */
#define PIN_PA27                    ( 27  )  /**< Pin Number for PA27 */
#define PIN_PA28                    ( 28  )  /**< Pin Number for PA28 */
#define PIN_PA29                    ( 29  )  /**< Pin Number for PA29 */
#define PIN_PA30                    ( 30  )  /**< Pin Number for PA30 */
#define PIN_PA31                    ( 31  )  /**< Pin Number for PA31 */
#define PIN_PB0                     ( 32  )  /**< Pin Number for PB0 */
#define PIN_PB1                     ( 33  )  /**< Pin Number for PB1 */
#define PIN_PB2                     ( 34  )  /**< Pin Number for PB2 */
#define PIN_PB3                     ( 35  )  /**< Pin Number for PB3 */
#define PIN_PB4                     ( 36  )  /**< Pin Number for PB4 */
#define PIN_PB5                     ( 37  )  /**< Pin Number for PB5 */
#define PIN_PB6                     ( 38  )  /**< Pin Number for PB6 */
#define PIN_PB7                     ( 39  )  /**< Pin Number for PB7 */
#define PIN_PB8                     ( 40  )  /**< Pin Number for PB8 */
#define PIN_PB9                     ( 41  )  /**< Pin Number for PB9 */
#define PIN_PB12                    ( 44  )  /**< Pin Number for PB12 */
#define PIN_PB13                    ( 45  )  /**< Pin Number for PB13 */
#define PIN_PC0                     ( 64  )  /**< Pin Number for PC0 */
#define PIN_PC1                     ( 65  )  /**< Pin Number for PC1 */
#define PIN_PC2                     ( 66  )  /**< Pin Number for PC2 */
#define PIN_PC3                     ( 67  )  /**< Pin Number for PC3 */
#define PIN_PC4                     ( 68  )  /**< Pin Number for PC4 */
#define PIN_PC5                     ( 69  )  /**< Pin Number for PC5 */
#define PIN_PC6                     ( 70  )  /**< Pin Number for PC6 */
#define PIN_PC7                     ( 71  )  /**< Pin Number for PC7 */
#define PIN_PC8                     ( 72  )  /**< Pin Number for PC8 */
#define PIN_PC9                     ( 73  )  /**< Pin Number for PC9 */
#define PIN_PC10                    ( 74  )  /**< Pin Number for PC10 */
#define PIN_PC11                    ( 75  )  /**< Pin Number for PC11 */
#define PIN_PC12                    ( 76  )  /**< Pin Number for PC12 */
#define PIN_PC13                    ( 77  )  /**< Pin Number for PC13 */
#define PIN_PC14                    ( 78  )  /**< Pin Number for PC14 */
#define PIN_PC15                    ( 79  )  /**< Pin Number for PC15 */
#define PIN_PC16                    ( 80  )  /**< Pin Number for PC16 */
#define PIN_PC17                    ( 81  )  /**< Pin Number for PC17 */
#define PIN_PC18                    ( 82  )  /**< Pin Number for PC18 */
#define PIN_PC19                    ( 83  )  /**< Pin Number for PC19 */
#define PIN_PC20                    ( 84  )  /**< Pin Number for PC20 */
#define PIN_PC21                    ( 85  )  /**< Pin Number for PC21 */
#define PIN_PC22                    ( 86  )  /**< Pin Number for PC22 */
#define PIN_PC23                    ( 87  )  /**< Pin Number for PC23 */
#define PIN_PC24                    ( 88  )  /**< Pin Number for PC24 */
#define PIN_PC25                    ( 89  )  /**< Pin Number for PC25 */
#define PIN_PC26                    ( 90  )  /**< Pin Number for PC26 */
#define PIN_PC27                    ( 91  )  /**< Pin Number for PC27 */
#define PIN_PC28                    ( 92  )  /**< Pin Number for PC28 */
#define PIN_PC29                    ( 93  )  /**< Pin Number for PC29 */
#define PIN_PC30                    ( 94  )  /**< Pin Number for PC30 */
#define PIN_PC31                    ( 95  )  /**< Pin Number for PC31 */
#define PIN_PD0                     ( 96  )  /**< Pin Number for PD0 */
#define PIN_PD1                     ( 97  )  /**< Pin Number for PD1 */
#define PIN_PD2                     ( 98  )  /**< Pin Number for PD2 */
#define PIN_PD3                     ( 99  )  /**< Pin Number for PD3 */
#define PIN_PD4                     (100  )  /**< Pin Number for PD4 */
#define PIN_PD5                     (101  )  /**< Pin Number for PD5 */
#define PIN_PD6                     (102  )  /**< Pin Number for PD6 */
#define PIN_PD7                     (103  )  /**< Pin Number for PD7 */
#define PIN_PD8                     (104  )  /**< Pin Number for PD8 */
#define PIN_PD9                     (105  )  /**< Pin Number for PD9 */
#define PIN_PD10                    (106  )  /**< Pin Number for PD10 */
#define PIN_PD11                    (107  )  /**< Pin Number for PD11 */
#define PIN_PD12                    (108  )  /**< Pin Number for PD12 */
#define PIN_PD13                    (109  )  /**< Pin Number for PD13 */
#define PIN_PD14                    (110  )  /**< Pin Number for PD14 */
#define PIN_PD15                    (111  )  /**< Pin Number for PD15 */
#define PIN_PD16                    (112  )  /**< Pin Number for PD16 */
#define PIN_PD17                    (113  )  /**< Pin Number for PD17 */
#define PIN_PD18                    (114  )  /**< Pin Number for PD18 */
#define PIN_PD19                    (115  )  /**< Pin Number for PD19 */
#define PIN_PD20                    (116  )  /**< Pin Number for PD20 */
#define PIN_PD21                    (117  )  /**< Pin Number for PD21 */
#define PIN_PD22                    (118  )  /**< Pin Number for PD22 */
#define PIN_PD23                    (119  )  /**< Pin Number for PD23 */
#define PIN_PD24                    (120  )  /**< Pin Number for PD24 */
#define PIN_PD25                    (121  )  /**< Pin Number for PD25 */
#define PIN_PD26                    (122  )  /**< Pin Number for PD26 */
#define PIN_PD27                    (123  )  /**< Pin Number for PD27 */
#define PIN_PD28                    (124  )  /**< Pin Number for PD28 */
#define PIN_PD29                    (125  )  /**< Pin Number for PD29 */
#define PIN_PD30                    (126  )  /**< Pin Number for PD30 */
#define PIN_PD31                    (127  )  /**< Pin Number for PD31 */
#define PIN_PE0                     (128  )  /**< Pin Number for PE0 */
#define PIN_PE1                     (129  )  /**< Pin Number for PE1 */
#define PIN_PE2                     (130  )  /**< Pin Number for PE2 */
#define PIN_PE3                     (131  )  /**< Pin Number for PE3 */
#define PIN_PE4                     (132  )  /**< Pin Number for PE4 */
#define PIN_PE5                     (133  )  /**< Pin Number for PE5 */

/* ========== Peripheral I/O masks ========== */
#define PIO_PA0                     _UL_(1 << 0)    /**< PIO mask for PA0 */
#define PIO_PA1                     _UL_(1 << 1)    /**< PIO mask for PA1 */
#define PIO_PA2                     _UL_(1 << 2)    /**< PIO mask for PA2 */
#define PIO_PA3                     _UL_(1 << 3)    /**< PIO mask for PA3 */
#define PIO_PA4                     _UL_(1 << 4)    /**< PIO mask for PA4 */
#define PIO_PA5                     _UL_(1 << 5)    /**< PIO mask for PA5 */
#define PIO_PA6                     _UL_(1 << 6)    /**< PIO mask for PA6 */
#define PIO_PA7                     _UL_(1 << 7)    /**< PIO mask for PA7 */
#define PIO_PA8                     _UL_(1 << 8)    /**< PIO mask for PA8 */
#define PIO_PA9                     _UL_(1 << 9)    /**< PIO mask for PA9 */
#define PIO_PA10                    _UL_(1 << 10)   /**< PIO mask for PA10 */
#define PIO_PA11                    _UL_(1 << 11)   /**< PIO mask for PA11 */
#define PIO_PA12                    _UL_(1 << 12)   /**< PIO mask for PA12 */
#define PIO_PA13                    _UL_(1 << 13)   /**< PIO mask for PA13 */
#define PIO_PA14                    _UL_(1 << 14)   /**< PIO mask for PA14 */
#define PIO_PA15                    _UL_(1 << 15)   /**< PIO mask for PA15 */
#define PIO_PA16                    _UL_(1 << 16)   /**< PIO mask for PA16 */
#define PIO_PA17                    _UL_(1 << 17)   /**< PIO mask for PA17 */
#define PIO_PA18                    _UL_(1 << 18)   /**< PIO mask for PA18 */
#define PIO_PA19                    _UL_(1 << 19)   /**< PIO mask for PA19 */
#define PIO_PA20                    _UL_(1 << 20)   /**< PIO mask for PA20 */
#define PIO_PA21                    _UL_(1 << 21)   /**< PIO mask for PA21 */
#define PIO_PA22                    _UL_(1 << 22)   /**< PIO mask for PA22 */
#define PIO_PA23                    _UL_(1 << 23)   /**< PIO mask for PA23 */
#define PIO_PA24                    _UL_(1 << 24)   /**< PIO mask for PA24 */
#define PIO_PA25                    _UL_(1 << 25)   /**< PIO mask for PA25 */
#define PIO_PA26                    _UL_(1 << 26)   /**< PIO mask for PA26 */
#define PIO_PA27                    _UL_(1 << 27)   /**< PIO mask for PA27 */
#define PIO_PA28                    _UL_(1 << 28)   /**< PIO mask for PA28 */
#define PIO_PA29                    _UL_(1 << 29)   /**< PIO mask for PA29 */
#define PIO_PA30                    _UL_(1 << 30)   /**< PIO mask for PA30 */
#define PIO_PA31                    _UL_(1 << 31)   /**< PIO mask for PA31 */
#define PIO_PB0                     _UL_(1 << 0)    /**< PIO mask for PB0 */
#define PIO_PB1                     _UL_(1 << 1)    /**< PIO mask for PB1 */
#define PIO_PB2                     _UL_(1 << 2)    /**< PIO mask for PB2 */
#define PIO_PB3                     _UL_(1 << 3)    /**< PIO mask for PB3 */
#define PIO_PB4                     _UL_(1 << 4)    /**< PIO mask for PB4 */
#define PIO_PB5                     _UL_(1 << 5)    /**< PIO mask for PB5 */
#define PIO_PB6                     _UL_(1 << 6)    /**< PIO mask for PB6 */
#define PIO_PB7                     _UL_(1 << 7)    /**< PIO mask for PB7 */
#define PIO_PB8                     _UL_(1 << 8)    /**< PIO mask for PB8 */
#define PIO_PB9                     _UL_(1 << 9)    /**< PIO mask for PB9 */
#define PIO_PB12                    _UL_(1 << 12)   /**< PIO mask for PB12 */
#define PIO_PB13                    _UL_(1 << 13)   /**< PIO mask for PB13 */
#define PIO_PC0                     _UL_(1 << 0)    /**< PIO mask for PC0 */
#define PIO_PC1                     _UL_(1 << 1)    /**< PIO mask for PC1 */
#define PIO_PC2                     _UL_(1 << 2)    /**< PIO mask for PC2 */
#define PIO_PC3                     _UL_(1 << 3)    /**< PIO mask for PC3 */
#define PIO_PC4                     _UL_(1 << 4)    /**< PIO mask for PC4 */
#define PIO_PC5                     _UL_(1 << 5)    /**< PIO mask for PC5 */
#define PIO_PC6                     _UL_(1 << 6)    /**< PIO mask for PC6 */
#define PIO_PC7                     _UL_(1 << 7)    /**< PIO mask for PC7 */
#define PIO_PC8                     _UL_(1 << 8)    /**< PIO mask for PC8 */
#define PIO_PC9                     _UL_(1 << 9)    /**< PIO mask for PC9 */
#define PIO_PC10                    _UL_(1 << 10)   /**< PIO mask for PC10 */
#define PIO_PC11                    _UL_(1 << 11)   /**< PIO mask for PC11 */
#define PIO_PC12                    _UL_(1 << 12)   /**< PIO mask for PC12 */
#define PIO_PC13                    _UL_(1 << 13)   /**< PIO mask for PC13 */
#define PIO_PC14                    _UL_(1 << 14)   /**< PIO mask for PC14 */
#define PIO_PC15                    _UL_(1 << 15)   /**< PIO mask for PC15 */
#define PIO_PC16                    _UL_(1 << 16)   /**< PIO mask for PC16 */
#define PIO_PC17                    _UL_(1 << 17)   /**< PIO mask for PC17 */
#define PIO_PC18                    _UL_(1 << 18)   /**< PIO mask for PC18 */
#define PIO_PC19                    _UL_(1 << 19)   /**< PIO mask for PC19 */
#define PIO_PC20                    _UL_(1 << 20)   /**< PIO mask for PC20 */
#define PIO_PC21                    _UL_(1 << 21)   /**< PIO mask for PC21 */
#define PIO_PC22                    _UL_(1 << 22)   /**< PIO mask for PC22 */
#define PIO_PC23                    _UL_(1 << 23)   /**< PIO mask for PC23 */
#define PIO_PC24                    _UL_(1 << 24)   /**< PIO mask for PC24 */
#define PIO_PC25                    _UL_(1 << 25)   /**< PIO mask for PC25 */
#define PIO_PC26                    _UL_(1 << 26)   /**< PIO mask for PC26 */
#define PIO_PC27                    _UL_(1 << 27)   /**< PIO mask for PC27 */
#define PIO_PC28                    _UL_(1 << 28)   /**< PIO mask for PC28 */
#define PIO_PC29                    _UL_(1 << 29)   /**< PIO mask for PC29 */
#define PIO_PC30                    _UL_(1 << 30)   /**< PIO mask for PC30 */
#define PIO_PC31                    _UL_(1 << 31)   /**< PIO mask for PC31 */
#define PIO_PD0                     _UL_(1 << 0)    /**< PIO mask for PD0 */
#define PIO_PD1                     _UL_(1 << 1)    /**< PIO mask for PD1 */
#define PIO_PD2                     _UL_(1 << 2)    /**< PIO mask for PD2 */
#define PIO_PD3                     _UL_(1 << 3)    /**< PIO mask for PD3 */
#define PIO_PD4                     _UL_(1 << 4)    /**< PIO mask for PD4 */
#define PIO_PD5                     _UL_(1 << 5)    /**< PIO mask for PD5 */
#define PIO_PD6                     _UL_(1 << 6)    /**< PIO mask for PD6 */
#define PIO_PD7                     _UL_(1 << 7)    /**< PIO mask for PD7 */
#define PIO_PD8                     _UL_(1 << 8)    /**< PIO mask for PD8 */
#define PIO_PD9                     _UL_(1 << 9)    /**< PIO mask for PD9 */
#define PIO_PD10                    _UL_(1 << 10)   /**< PIO mask for PD10 */
#define PIO_PD11                    _UL_(1 << 11)   /**< PIO mask for PD11 */
#define PIO_PD12                    _UL_(1 << 12)   /**< PIO mask for PD12 */
#define PIO_PD13                    _UL_(1 << 13)   /**< PIO mask for PD13 */
#define PIO_PD14                    _UL_(1 << 14)   /**< PIO mask for PD14 */
#define PIO_PD15                    _UL_(1 << 15)   /**< PIO mask for PD15 */
#define PIO_PD16                    _UL_(1 << 16)   /**< PIO mask for PD16 */
#define PIO_PD17                    _UL_(1 << 17)   /**< PIO mask for PD17 */
#define PIO_PD18                    _UL_(1 << 18)   /**< PIO mask for PD18 */
#define PIO_PD19                    _UL_(1 << 19)   /**< PIO mask for PD19 */
#define PIO_PD20                    _UL_(1 << 20)   /**< PIO mask for PD20 */
#define PIO_PD21                    _UL_(1 << 21)   /**< PIO mask for PD21 */
#define PIO_PD22                    _UL_(1 << 22)   /**< PIO mask for PD22 */
#define PIO_PD23                    _UL_(1 << 23)   /**< PIO mask for PD23 */
#define PIO_PD24                    _UL_(1 << 24)   /**< PIO mask for PD24 */
#define PIO_PD25                    _UL_(1 << 25)   /**< PIO mask for PD25 */
#define PIO_PD26                    _UL_(1 << 26)   /**< PIO mask for PD26 */
#define PIO_PD27                    _UL_(1 << 27)   /**< PIO mask for PD27 */
#define PIO_PD28                    _UL_(1 << 28)   /**< PIO mask for PD28 */
#define PIO_PD29                    _UL_(1 << 29)   /**< PIO mask for PD29 */
#define PIO_PD30                    _UL_(1 << 30)   /**< PIO mask for PD30 */
#define PIO_PD31                    _UL_(1 << 31)   /**< PIO mask for PD31 */
#define PIO_PE0                     _UL_(1 << 0)    /**< PIO mask for PE0 */
#define PIO_PE1                     _UL_(1 << 1)    /**< PIO mask for PE1 */
#define PIO_PE2                     _UL_(1 << 2)    /**< PIO mask for PE2 */
#define PIO_PE3                     _UL_(1 << 3)    /**< PIO mask for PE3 */
#define PIO_PE4                     _UL_(1 << 4)    /**< PIO mask for PE4 */
#define PIO_PE5                     _UL_(1 << 5)    /**< PIO mask for PE5 */

/* ========== PIO definition for AFEC0 peripheral ========== */
#define PIN_PA8B_AFEC0_ADTRG                       _L_(8)       /**< AFEC0 signal: AFEC0_ADTRG on PA8 mux B */
#define MUX_PA8B_AFEC0_ADTRG                       _L_(1)       /**< AFEC0 signal line function value: AFEC0_ADTRG */
#define PIO_PA8B_AFEC0_ADTRG                       _UL_(1 << 8) /**< AFEC0 signal: AFEC0_ADTRG */
#define PIN_PD30X1_AFEC0_AD0                       _L_(126)     /**< AFEC0 signal: AFEC0_AD0 on PD30 mux X1 */
#define PIO_PD30X1_AFEC0_AD0                       _UL_(1 << 30) /**< AFEC0 signal: AFEC0_AD0 */
#define PIN_PA21X1_AFEC0_AD1                       _L_(21)      /**< AFEC0 signal: AFEC0_AD1 on PA21 mux X1 */
#define PIO_PA21X1_AFEC0_AD1                       _UL_(1 << 21) /**< AFEC0 signal: AFEC0_AD1 */
#define PIN_PB3X1_AFEC0_AD2                        _L_(35)      /**< AFEC0 signal: AFEC0_AD2 on PB3 mux X1 */
#define PIO_PB3X1_AFEC0_AD2                        _UL_(1 << 3) /**< AFEC0 signal: AFEC0_AD2 */
#define PIN_PE5X1_AFEC0_AD3                        _L_(133)     /**< AFEC0 signal: AFEC0_AD3 on PE5 mux X1 */
#define PIO_PE5X1_AFEC0_AD3                        _UL_(1 << 5) /**< AFEC0 signal: AFEC0_AD3 */
#define PIN_PE4X1_AFEC0_AD4                        _L_(132)     /**< AFEC0 signal: AFEC0_AD4 on PE4 mux X1 */
#define PIO_PE4X1_AFEC0_AD4                        _UL_(1 << 4) /**< AFEC0 signal: AFEC0_AD4 */
#define PIN_PB2X1_AFEC0_AD5                        _L_(34)      /**< AFEC0 signal: AFEC0_AD5 on PB2 mux X1 */
#define PIO_PB2X1_AFEC0_AD5                        _UL_(1 << 2) /**< AFEC0 signal: AFEC0_AD5 */
#define PIN_PA17X1_AFEC0_AD6                       _L_(17)      /**< AFEC0 signal: AFEC0_AD6 on PA17 mux X1 */
#define PIO_PA17X1_AFEC0_AD6                       _UL_(1 << 17) /**< AFEC0 signal: AFEC0_AD6 */
#define PIN_PA18X1_AFEC0_AD7                       _L_(18)      /**< AFEC0 signal: AFEC0_AD7 on PA18 mux X1 */
#define PIO_PA18X1_AFEC0_AD7                       _UL_(1 << 18) /**< AFEC0 signal: AFEC0_AD7 */
#define PIN_PA19X1_AFEC0_AD8                       _L_(19)      /**< AFEC0 signal: AFEC0_AD8 on PA19 mux X1 */
#define PIO_PA19X1_AFEC0_AD8                       _UL_(1 << 19) /**< AFEC0 signal: AFEC0_AD8 */
#define PIN_PA20X1_AFEC0_AD9                       _L_(20)      /**< AFEC0 signal: AFEC0_AD9 on PA20 mux X1 */
#define PIO_PA20X1_AFEC0_AD9                       _UL_(1 << 20) /**< AFEC0 signal: AFEC0_AD9 */
#define PIN_PB0X1_AFEC0_AD10                       _L_(32)      /**< AFEC0 signal: AFEC0_AD10 on PB0 mux X1 */
#define PIO_PB0X1_AFEC0_AD10                       _UL_(1 << 0) /**< AFEC0 signal: AFEC0_AD10 */
/* ========== PIO definition for AFEC1 peripheral ========== */
#define PIN_PD9C_AFEC1_ADTRG                       _L_(105)     /**< AFEC1 signal: AFEC1_ADTRG on PD9 mux C */
#define MUX_PD9C_AFEC1_ADTRG                       _L_(2)       /**< AFEC1 signal line function value: AFEC1_ADTRG */
#define PIO_PD9C_AFEC1_ADTRG                       _UL_(1 << 9) /**< AFEC1 signal: AFEC1_ADTRG */
#define PIN_PB1X1_AFEC1_AD0                        _L_(33)      /**< AFEC1 signal: AFEC1_AD0 on PB1 mux X1 */
#define PIO_PB1X1_AFEC1_AD0                        _UL_(1 << 1) /**< AFEC1 signal: AFEC1_AD0 */
#define PIN_PC13X1_AFEC1_AD1                       _L_(77)      /**< AFEC1 signal: AFEC1_AD1 on PC13 mux X1 */
#define PIO_PC13X1_AFEC1_AD1                       _UL_(1 << 13) /**< AFEC1 signal: AFEC1_AD1 */
#define PIN_PC15X1_AFEC1_AD2                       _L_(79)      /**< AFEC1 signal: AFEC1_AD2 on PC15 mux X1 */
#define PIO_PC15X1_AFEC1_AD2                       _UL_(1 << 15) /**< AFEC1 signal: AFEC1_AD2 */
#define PIN_PC12X1_AFEC1_AD3                       _L_(76)      /**< AFEC1 signal: AFEC1_AD3 on PC12 mux X1 */
#define PIO_PC12X1_AFEC1_AD3                       _UL_(1 << 12) /**< AFEC1 signal: AFEC1_AD3 */
#define PIN_PC29X1_AFEC1_AD4                       _L_(93)      /**< AFEC1 signal: AFEC1_AD4 on PC29 mux X1 */
#define PIO_PC29X1_AFEC1_AD4                       _UL_(1 << 29) /**< AFEC1 signal: AFEC1_AD4 */
#define PIN_PC30X1_AFEC1_AD5                       _L_(94)      /**< AFEC1 signal: AFEC1_AD5 on PC30 mux X1 */
#define PIO_PC30X1_AFEC1_AD5                       _UL_(1 << 30) /**< AFEC1 signal: AFEC1_AD5 */
#define PIN_PC31X1_AFEC1_AD6                       _L_(95)      /**< AFEC1 signal: AFEC1_AD6 on PC31 mux X1 */
#define PIO_PC31X1_AFEC1_AD6                       _UL_(1 << 31) /**< AFEC1 signal: AFEC1_AD6 */
#define PIN_PC26X1_AFEC1_AD7                       _L_(90)      /**< AFEC1 signal: AFEC1_AD7 on PC26 mux X1 */
#define PIO_PC26X1_AFEC1_AD7                       _UL_(1 << 26) /**< AFEC1 signal: AFEC1_AD7 */
#define PIN_PC27X1_AFEC1_AD8                       _L_(91)      /**< AFEC1 signal: AFEC1_AD8 on PC27 mux X1 */
#define PIO_PC27X1_AFEC1_AD8                       _UL_(1 << 27) /**< AFEC1 signal: AFEC1_AD8 */
#define PIN_PC0X1_AFEC1_AD9                        _L_(64)      /**< AFEC1 signal: AFEC1_AD9 on PC0 mux X1 */
#define PIO_PC0X1_AFEC1_AD9                        _UL_(1 << 0) /**< AFEC1 signal: AFEC1_AD9 */
#define PIN_PE3X1_AFEC1_AD10                       _L_(131)     /**< AFEC1 signal: AFEC1_AD10 on PE3 mux X1 */
#define PIO_PE3X1_AFEC1_AD10                       _UL_(1 << 3) /**< AFEC1 signal: AFEC1_AD10 */
#define PIN_PE0X1_AFEC1_AD11                       _L_(128)     /**< AFEC1 signal: AFEC1_AD11 on PE0 mux X1 */
#define PIO_PE0X1_AFEC1_AD11                       _UL_(1 << 0) /**< AFEC1 signal: AFEC1_AD11 */
/* ========== PIO definition for DACC peripheral ========== */
#define PIN_PB13X1_DACC_DAC0                       _L_(45)      /**< DACC signal: DACC_DAC0 on PB13 mux X1 */
#define PIO_PB13X1_DACC_DAC0                       _UL_(1 << 13) /**< DACC signal: DACC_DAC0 */
#define PIN_PD0X1_DACC_DAC1                        _L_(96)      /**< DACC signal: DACC_DAC1 on PD0 mux X1 */
#define PIO_PD0X1_DACC_DAC1                        _UL_(1 << 0) /**< DACC signal: DACC_DAC1 */
#define PIN_PA2C_DACC_DATRG                        _L_(2)       /**< DACC signal: DACC_DATRG on PA2 mux C */
#define MUX_PA2C_DACC_DATRG                        _L_(2)       /**< DACC signal line function value: DACC_DATRG */
#define PIO_PA2C_DACC_DATRG                        _UL_(1 << 2) /**< DACC signal: DACC_DATRG */
/* ========== PIO definition for EBI peripheral ========== */
#define PIN_PC18A_EBI_A0                           _L_(82)      /**< EBI signal: EBI_A0 on PC18 mux A */
#define MUX_PC18A_EBI_A0                           _L_(0)       /**< EBI signal line function value: EBI_A0 */
#define PIO_PC18A_EBI_A0                           _UL_(1 << 18) /**< EBI signal: EBI_A0 */
#define PIN_PC19A_EBI_A1                           _L_(83)      /**< EBI signal: EBI_A1 on PC19 mux A */
#define MUX_PC19A_EBI_A1                           _L_(0)       /**< EBI signal line function value: EBI_A1 */
#define PIO_PC19A_EBI_A1                           _UL_(1 << 19) /**< EBI signal: EBI_A1 */
#define PIN_PC20A_EBI_A2                           _L_(84)      /**< EBI signal: EBI_A2 on PC20 mux A */
#define MUX_PC20A_EBI_A2                           _L_(0)       /**< EBI signal line function value: EBI_A2 */
#define PIO_PC20A_EBI_A2                           _UL_(1 << 20) /**< EBI signal: EBI_A2 */
#define PIN_PC21A_EBI_A3                           _L_(85)      /**< EBI signal: EBI_A3 on PC21 mux A */
#define MUX_PC21A_EBI_A3                           _L_(0)       /**< EBI signal line function value: EBI_A3 */
#define PIO_PC21A_EBI_A3                           _UL_(1 << 21) /**< EBI signal: EBI_A3 */
#define PIN_PC22A_EBI_A4                           _L_(86)      /**< EBI signal: EBI_A4 on PC22 mux A */
#define MUX_PC22A_EBI_A4                           _L_(0)       /**< EBI signal line function value: EBI_A4 */
#define PIO_PC22A_EBI_A4                           _UL_(1 << 22) /**< EBI signal: EBI_A4 */
#define PIN_PC23A_EBI_A5                           _L_(87)      /**< EBI signal: EBI_A5 on PC23 mux A */
#define MUX_PC23A_EBI_A5                           _L_(0)       /**< EBI signal line function value: EBI_A5 */
#define PIO_PC23A_EBI_A5                           _UL_(1 << 23) /**< EBI signal: EBI_A5 */
#define PIN_PC24A_EBI_A6                           _L_(88)      /**< EBI signal: EBI_A6 on PC24 mux A */
#define MUX_PC24A_EBI_A6                           _L_(0)       /**< EBI signal line function value: EBI_A6 */
#define PIO_PC24A_EBI_A6                           _UL_(1 << 24) /**< EBI signal: EBI_A6 */
#define PIN_PC25A_EBI_A7                           _L_(89)      /**< EBI signal: EBI_A7 on PC25 mux A */
#define MUX_PC25A_EBI_A7                           _L_(0)       /**< EBI signal line function value: EBI_A7 */
#define PIO_PC25A_EBI_A7                           _UL_(1 << 25) /**< EBI signal: EBI_A7 */
#define PIN_PC26A_EBI_A8                           _L_(90)      /**< EBI signal: EBI_A8 on PC26 mux A */
#define MUX_PC26A_EBI_A8                           _L_(0)       /**< EBI signal line function value: EBI_A8 */
#define PIO_PC26A_EBI_A8                           _UL_(1 << 26) /**< EBI signal: EBI_A8 */
#define PIN_PC27A_EBI_A9                           _L_(91)      /**< EBI signal: EBI_A9 on PC27 mux A */
#define MUX_PC27A_EBI_A9                           _L_(0)       /**< EBI signal line function value: EBI_A9 */
#define PIO_PC27A_EBI_A9                           _UL_(1 << 27) /**< EBI signal: EBI_A9 */
#define PIN_PC28A_EBI_A10                          _L_(92)      /**< EBI signal: EBI_A10 on PC28 mux A */
#define MUX_PC28A_EBI_A10                          _L_(0)       /**< EBI signal line function value: EBI_A10 */
#define PIO_PC28A_EBI_A10                          _UL_(1 << 28) /**< EBI signal: EBI_A10 */
#define PIN_PC29A_EBI_A11                          _L_(93)      /**< EBI signal: EBI_A11 on PC29 mux A */
#define MUX_PC29A_EBI_A11                          _L_(0)       /**< EBI signal line function value: EBI_A11 */
#define PIO_PC29A_EBI_A11                          _UL_(1 << 29) /**< EBI signal: EBI_A11 */
#define PIN_PC30A_EBI_A12                          _L_(94)      /**< EBI signal: EBI_A12 on PC30 mux A */
#define MUX_PC30A_EBI_A12                          _L_(0)       /**< EBI signal line function value: EBI_A12 */
#define PIO_PC30A_EBI_A12                          _UL_(1 << 30) /**< EBI signal: EBI_A12 */
#define PIN_PC31A_EBI_A13                          _L_(95)      /**< EBI signal: EBI_A13 on PC31 mux A */
#define MUX_PC31A_EBI_A13                          _L_(0)       /**< EBI signal line function value: EBI_A13 */
#define PIO_PC31A_EBI_A13                          _UL_(1 << 31) /**< EBI signal: EBI_A13 */
#define PIN_PA18C_EBI_A14                          _L_(18)      /**< EBI signal: EBI_A14 on PA18 mux C */
#define MUX_PA18C_EBI_A14                          _L_(2)       /**< EBI signal line function value: EBI_A14 */
#define PIO_PA18C_EBI_A14                          _UL_(1 << 18) /**< EBI signal: EBI_A14 */
#define PIN_PA19C_EBI_A15                          _L_(19)      /**< EBI signal: EBI_A15 on PA19 mux C */
#define MUX_PA19C_EBI_A15                          _L_(2)       /**< EBI signal line function value: EBI_A15 */
#define PIO_PA19C_EBI_A15                          _UL_(1 << 19) /**< EBI signal: EBI_A15 */
#define PIN_PA20C_EBI_A16                          _L_(20)      /**< EBI signal: EBI_A16 on PA20 mux C */
#define MUX_PA20C_EBI_A16                          _L_(2)       /**< EBI signal line function value: EBI_A16 */
#define PIO_PA20C_EBI_A16                          _UL_(1 << 20) /**< EBI signal: EBI_A16 */
#define PIN_PA0C_EBI_A17                           _L_(0)       /**< EBI signal: EBI_A17 on PA0 mux C */
#define MUX_PA0C_EBI_A17                           _L_(2)       /**< EBI signal line function value: EBI_A17 */
#define PIO_PA0C_EBI_A17                           _UL_(1 << 0) /**< EBI signal: EBI_A17 */
#define PIN_PA1C_EBI_A18                           _L_(1)       /**< EBI signal: EBI_A18 on PA1 mux C */
#define MUX_PA1C_EBI_A18                           _L_(2)       /**< EBI signal line function value: EBI_A18 */
#define PIO_PA1C_EBI_A18                           _UL_(1 << 1) /**< EBI signal: EBI_A18 */
#define PIN_PA23C_EBI_A19                          _L_(23)      /**< EBI signal: EBI_A19 on PA23 mux C */
#define MUX_PA23C_EBI_A19                          _L_(2)       /**< EBI signal line function value: EBI_A19 */
#define PIO_PA23C_EBI_A19                          _UL_(1 << 23) /**< EBI signal: EBI_A19 */
#define PIN_PA24C_EBI_A20                          _L_(24)      /**< EBI signal: EBI_A20 on PA24 mux C */
#define MUX_PA24C_EBI_A20                          _L_(2)       /**< EBI signal line function value: EBI_A20 */
#define PIO_PA24C_EBI_A20                          _UL_(1 << 24) /**< EBI signal: EBI_A20 */
#define PIN_PC16A_EBI_A21                          _L_(80)      /**< EBI signal: EBI_A21 on PC16 mux A */
#define MUX_PC16A_EBI_A21                          _L_(0)       /**< EBI signal line function value: EBI_A21 */
#define PIO_PC16A_EBI_A21                          _UL_(1 << 16) /**< EBI signal: EBI_A21 */
#define PIN_PC17A_EBI_A22                          _L_(81)      /**< EBI signal: EBI_A22 on PC17 mux A */
#define MUX_PC17A_EBI_A22                          _L_(0)       /**< EBI signal line function value: EBI_A22 */
#define PIO_PC17A_EBI_A22                          _UL_(1 << 17) /**< EBI signal: EBI_A22 */
#define PIN_PA25C_EBI_A23                          _L_(25)      /**< EBI signal: EBI_A23 on PA25 mux C */
#define MUX_PA25C_EBI_A23                          _L_(2)       /**< EBI signal line function value: EBI_A23 */
#define PIO_PA25C_EBI_A23                          _UL_(1 << 25) /**< EBI signal: EBI_A23 */
#define PIN_PC0A_EBI_D0                            _L_(64)      /**< EBI signal: EBI_D0 on PC0 mux A */
#define MUX_PC0A_EBI_D0                            _L_(0)       /**< EBI signal line function value: EBI_D0 */
#define PIO_PC0A_EBI_D0                            _UL_(1 << 0) /**< EBI signal: EBI_D0 */
#define PIN_PC1A_EBI_D1                            _L_(65)      /**< EBI signal: EBI_D1 on PC1 mux A */
#define MUX_PC1A_EBI_D1                            _L_(0)       /**< EBI signal line function value: EBI_D1 */
#define PIO_PC1A_EBI_D1                            _UL_(1 << 1) /**< EBI signal: EBI_D1 */
#define PIN_PC2A_EBI_D2                            _L_(66)      /**< EBI signal: EBI_D2 on PC2 mux A */
#define MUX_PC2A_EBI_D2                            _L_(0)       /**< EBI signal line function value: EBI_D2 */
#define PIO_PC2A_EBI_D2                            _UL_(1 << 2) /**< EBI signal: EBI_D2 */
#define PIN_PC3A_EBI_D3                            _L_(67)      /**< EBI signal: EBI_D3 on PC3 mux A */
#define MUX_PC3A_EBI_D3                            _L_(0)       /**< EBI signal line function value: EBI_D3 */
#define PIO_PC3A_EBI_D3                            _UL_(1 << 3) /**< EBI signal: EBI_D3 */
#define PIN_PC4A_EBI_D4                            _L_(68)      /**< EBI signal: EBI_D4 on PC4 mux A */
#define MUX_PC4A_EBI_D4                            _L_(0)       /**< EBI signal line function value: EBI_D4 */
#define PIO_PC4A_EBI_D4                            _UL_(1 << 4) /**< EBI signal: EBI_D4 */
#define PIN_PC5A_EBI_D5                            _L_(69)      /**< EBI signal: EBI_D5 on PC5 mux A */
#define MUX_PC5A_EBI_D5                            _L_(0)       /**< EBI signal line function value: EBI_D5 */
#define PIO_PC5A_EBI_D5                            _UL_(1 << 5) /**< EBI signal: EBI_D5 */
#define PIN_PC6A_EBI_D6                            _L_(70)      /**< EBI signal: EBI_D6 on PC6 mux A */
#define MUX_PC6A_EBI_D6                            _L_(0)       /**< EBI signal line function value: EBI_D6 */
#define PIO_PC6A_EBI_D6                            _UL_(1 << 6) /**< EBI signal: EBI_D6 */
#define PIN_PC7A_EBI_D7                            _L_(71)      /**< EBI signal: EBI_D7 on PC7 mux A */
#define MUX_PC7A_EBI_D7                            _L_(0)       /**< EBI signal line function value: EBI_D7 */
#define PIO_PC7A_EBI_D7                            _UL_(1 << 7) /**< EBI signal: EBI_D7 */
#define PIN_PE0A_EBI_D8                            _L_(128)     /**< EBI signal: EBI_D8 on PE0 mux A */
#define MUX_PE0A_EBI_D8                            _L_(0)       /**< EBI signal line function value: EBI_D8 */
#define PIO_PE0A_EBI_D8                            _UL_(1 << 0) /**< EBI signal: EBI_D8 */
#define PIN_PE1A_EBI_D9                            _L_(129)     /**< EBI signal: EBI_D9 on PE1 mux A */
#define MUX_PE1A_EBI_D9                            _L_(0)       /**< EBI signal line function value: EBI_D9 */
#define PIO_PE1A_EBI_D9                            _UL_(1 << 1) /**< EBI signal: EBI_D9 */
#define PIN_PE2A_EBI_D10                           _L_(130)     /**< EBI signal: EBI_D10 on PE2 mux A */
#define MUX_PE2A_EBI_D10                           _L_(0)       /**< EBI signal line function value: EBI_D10 */
#define PIO_PE2A_EBI_D10                           _UL_(1 << 2) /**< EBI signal: EBI_D10 */
#define PIN_PE3A_EBI_D11                           _L_(131)     /**< EBI signal: EBI_D11 on PE3 mux A */
#define MUX_PE3A_EBI_D11                           _L_(0)       /**< EBI signal line function value: EBI_D11 */
#define PIO_PE3A_EBI_D11                           _UL_(1 << 3) /**< EBI signal: EBI_D11 */
#define PIN_PE4A_EBI_D12                           _L_(132)     /**< EBI signal: EBI_D12 on PE4 mux A */
#define MUX_PE4A_EBI_D12                           _L_(0)       /**< EBI signal line function value: EBI_D12 */
#define PIO_PE4A_EBI_D12                           _UL_(1 << 4) /**< EBI signal: EBI_D12 */
#define PIN_PE5A_EBI_D13                           _L_(133)     /**< EBI signal: EBI_D13 on PE5 mux A */
#define MUX_PE5A_EBI_D13                           _L_(0)       /**< EBI signal line function value: EBI_D13 */
#define PIO_PE5A_EBI_D13                           _UL_(1 << 5) /**< EBI signal: EBI_D13 */
#define PIN_PA15A_EBI_D14                          _L_(15)      /**< EBI signal: EBI_D14 on PA15 mux A */
#define MUX_PA15A_EBI_D14                          _L_(0)       /**< EBI signal line function value: EBI_D14 */
#define PIO_PA15A_EBI_D14                          _UL_(1 << 15) /**< EBI signal: EBI_D14 */
#define PIN_PA16A_EBI_D15                          _L_(16)      /**< EBI signal: EBI_D15 on PA16 mux A */
#define MUX_PA16A_EBI_D15                          _L_(0)       /**< EBI signal line function value: EBI_D15 */
#define PIO_PA16A_EBI_D15                          _UL_(1 << 16) /**< EBI signal: EBI_D15 */
#define PIN_PC13A_EBI_NWAIT                        _L_(77)      /**< EBI signal: EBI_NWAIT on PC13 mux A */
#define MUX_PC13A_EBI_NWAIT                        _L_(0)       /**< EBI signal line function value: EBI_NWAIT */
#define PIO_PC13A_EBI_NWAIT                        _UL_(1 << 13) /**< EBI signal: EBI_NWAIT */
#define PIN_PC14A_EBI_NCS0                         _L_(78)      /**< EBI signal: EBI_NCS0 on PC14 mux A */
#define MUX_PC14A_EBI_NCS0                         _L_(0)       /**< EBI signal line function value: EBI_NCS0 */
#define PIO_PC14A_EBI_NCS0                         _UL_(1 << 14) /**< EBI signal: EBI_NCS0 */
#define PIN_PC15A_EBI_NCS1                         _L_(79)      /**< EBI signal: EBI_NCS1 on PC15 mux A */
#define MUX_PC15A_EBI_NCS1                         _L_(0)       /**< EBI signal line function value: EBI_NCS1 */
#define PIO_PC15A_EBI_NCS1                         _UL_(1 << 15) /**< EBI signal: EBI_NCS1 */
#define PIN_PD18A_EBI_NCS1                         _L_(114)     /**< EBI signal: EBI_NCS1 on PD18 mux A */
#define MUX_PD18A_EBI_NCS1                         _L_(0)       /**< EBI signal line function value: EBI_NCS1 */
#define PIO_PD18A_EBI_NCS1                         _UL_(1 << 18) /**< EBI signal: EBI_NCS1 */
#define PIN_PA22C_EBI_NCS2                         _L_(22)      /**< EBI signal: EBI_NCS2 on PA22 mux C */
#define MUX_PA22C_EBI_NCS2                         _L_(2)       /**< EBI signal line function value: EBI_NCS2 */
#define PIO_PA22C_EBI_NCS2                         _UL_(1 << 22) /**< EBI signal: EBI_NCS2 */
#define PIN_PC12A_EBI_NCS3                         _L_(76)      /**< EBI signal: EBI_NCS3 on PC12 mux A */
#define MUX_PC12A_EBI_NCS3                         _L_(0)       /**< EBI signal line function value: EBI_NCS3 */
#define PIO_PC12A_EBI_NCS3                         _UL_(1 << 12) /**< EBI signal: EBI_NCS3 */
#define PIN_PD19A_EBI_NCS3                         _L_(115)     /**< EBI signal: EBI_NCS3 on PD19 mux A */
#define MUX_PD19A_EBI_NCS3                         _L_(0)       /**< EBI signal line function value: EBI_NCS3 */
#define PIO_PD19A_EBI_NCS3                         _UL_(1 << 19) /**< EBI signal: EBI_NCS3 */
#define PIN_PC8A_EBI_NWR0                          _L_(72)      /**< EBI signal: EBI_NWR0 on PC8 mux A */
#define MUX_PC8A_EBI_NWR0                          _L_(0)       /**< EBI signal line function value: EBI_NWR0 */
#define PIO_PC8A_EBI_NWR0                          _UL_(1 << 8) /**< EBI signal: EBI_NWR0 */
#define PIN_PD15C_EBI_NWR1                         _L_(111)     /**< EBI signal: EBI_NWR1 on PD15 mux C */
#define MUX_PD15C_EBI_NWR1                         _L_(2)       /**< EBI signal line function value: EBI_NWR1 */
#define PIO_PD15C_EBI_NWR1                         _UL_(1 << 15) /**< EBI signal: EBI_NWR1 */
#define PIN_PC11A_EBI_NRD                          _L_(75)      /**< EBI signal: EBI_NRD on PC11 mux A */
#define MUX_PC11A_EBI_NRD                          _L_(0)       /**< EBI signal line function value: EBI_NRD */
#define PIO_PC11A_EBI_NRD                          _UL_(1 << 11) /**< EBI signal: EBI_NRD */
#define PIN_PC8A_EBI_NWE                           _L_(72)      /**< EBI signal: EBI_NWE on PC8 mux A */
#define MUX_PC8A_EBI_NWE                           _L_(0)       /**< EBI signal line function value: EBI_NWE */
#define PIO_PC8A_EBI_NWE                           _UL_(1 << 8) /**< EBI signal: EBI_NWE */
#define PIN_PC18A_EBI_NBS0                         _L_(82)      /**< EBI signal: EBI_NBS0 on PC18 mux A */
#define MUX_PC18A_EBI_NBS0                         _L_(0)       /**< EBI signal line function value: EBI_NBS0 */
#define PIO_PC18A_EBI_NBS0                         _UL_(1 << 18) /**< EBI signal: EBI_NBS0 */
#define PIN_PD15C_EBI_NBS1                         _L_(111)     /**< EBI signal: EBI_NBS1 on PD15 mux C */
#define MUX_PD15C_EBI_NBS1                         _L_(2)       /**< EBI signal line function value: EBI_NBS1 */
#define PIO_PD15C_EBI_NBS1                         _UL_(1 << 15) /**< EBI signal: EBI_NBS1 */
#define PIN_PC16A_EBI_NANDALE                      _L_(80)      /**< EBI signal: EBI_NANDALE on PC16 mux A */
#define MUX_PC16A_EBI_NANDALE                      _L_(0)       /**< EBI signal line function value: EBI_NANDALE */
#define PIO_PC16A_EBI_NANDALE                      _UL_(1 << 16) /**< EBI signal: EBI_NANDALE */
#define PIN_PC17A_EBI_NANDCLE                      _L_(81)      /**< EBI signal: EBI_NANDCLE on PC17 mux A */
#define MUX_PC17A_EBI_NANDCLE                      _L_(0)       /**< EBI signal line function value: EBI_NANDCLE */
#define PIO_PC17A_EBI_NANDCLE                      _UL_(1 << 17) /**< EBI signal: EBI_NANDCLE */
#define PIN_PC9A_EBI_NANDOE                        _L_(73)      /**< EBI signal: EBI_NANDOE on PC9 mux A */
#define MUX_PC9A_EBI_NANDOE                        _L_(0)       /**< EBI signal line function value: EBI_NANDOE */
#define PIO_PC9A_EBI_NANDOE                        _UL_(1 << 9) /**< EBI signal: EBI_NANDOE */
#define PIN_PC10A_EBI_NANDWE                       _L_(74)      /**< EBI signal: EBI_NANDWE on PC10 mux A */
#define MUX_PC10A_EBI_NANDWE                       _L_(0)       /**< EBI signal line function value: EBI_NANDWE */
#define PIO_PC10A_EBI_NANDWE                       _UL_(1 << 10) /**< EBI signal: EBI_NANDWE */
#define PIN_PD23C_EBI_SDCK                         _L_(119)     /**< EBI signal: EBI_SDCK on PD23 mux C */
#define MUX_PD23C_EBI_SDCK                         _L_(2)       /**< EBI signal line function value: EBI_SDCK */
#define PIO_PD23C_EBI_SDCK                         _UL_(1 << 23) /**< EBI signal: EBI_SDCK */
#define PIN_PD14C_EBI_SDCKE                        _L_(110)     /**< EBI signal: EBI_SDCKE on PD14 mux C */
#define MUX_PD14C_EBI_SDCKE                        _L_(2)       /**< EBI signal line function value: EBI_SDCKE */
#define PIO_PD14C_EBI_SDCKE                        _UL_(1 << 14) /**< EBI signal: EBI_SDCKE */
#define PIN_PC15A_EBI_SDCS                         _L_(79)      /**< EBI signal: EBI_SDCS on PC15 mux A */
#define MUX_PC15A_EBI_SDCS                         _L_(0)       /**< EBI signal line function value: EBI_SDCS */
#define PIO_PC15A_EBI_SDCS                         _UL_(1 << 15) /**< EBI signal: EBI_SDCS */
#define PIN_PD18A_EBI_SDCS                         _L_(114)     /**< EBI signal: EBI_SDCS on PD18 mux A */
#define MUX_PD18A_EBI_SDCS                         _L_(0)       /**< EBI signal line function value: EBI_SDCS */
#define PIO_PD18A_EBI_SDCS                         _UL_(1 << 18) /**< EBI signal: EBI_SDCS */
#define PIN_PA20C_EBI_BA0                          _L_(20)      /**< EBI signal: EBI_BA0 on PA20 mux C */
#define MUX_PA20C_EBI_BA0                          _L_(2)       /**< EBI signal line function value: EBI_BA0 */
#define PIO_PA20C_EBI_BA0                          _UL_(1 << 20) /**< EBI signal: EBI_BA0 */
#define PIN_PA0C_EBI_BA1                           _L_(0)       /**< EBI signal: EBI_BA1 on PA0 mux C */
#define MUX_PA0C_EBI_BA1                           _L_(2)       /**< EBI signal line function value: EBI_BA1 */
#define PIO_PA0C_EBI_BA1                           _UL_(1 << 0) /**< EBI signal: EBI_BA1 */
#define PIN_PD29C_EBI_SDWE                         _L_(125)     /**< EBI signal: EBI_SDWE on PD29 mux C */
#define MUX_PD29C_EBI_SDWE                         _L_(2)       /**< EBI signal line function value: EBI_SDWE */
#define PIO_PD29C_EBI_SDWE                         _UL_(1 << 29) /**< EBI signal: EBI_SDWE */
#define PIN_PD16C_EBI_RAS                          _L_(112)     /**< EBI signal: EBI_RAS on PD16 mux C */
#define MUX_PD16C_EBI_RAS                          _L_(2)       /**< EBI signal line function value: EBI_RAS */
#define PIO_PD16C_EBI_RAS                          _UL_(1 << 16) /**< EBI signal: EBI_RAS */
#define PIN_PD17C_EBI_CAS                          _L_(113)     /**< EBI signal: EBI_CAS on PD17 mux C */
#define MUX_PD17C_EBI_CAS                          _L_(2)       /**< EBI signal line function value: EBI_CAS */
#define PIO_PD17C_EBI_CAS                          _UL_(1 << 17) /**< EBI signal: EBI_CAS */
#define PIN_PC13C_EBI_SDA10                        _L_(77)      /**< EBI signal: EBI_SDA10 on PC13 mux C */
#define MUX_PC13C_EBI_SDA10                        _L_(2)       /**< EBI signal line function value: EBI_SDA10 */
#define PIO_PC13C_EBI_SDA10                        _UL_(1 << 13) /**< EBI signal: EBI_SDA10 */
#define PIN_PD13C_EBI_SDA10                        _L_(109)     /**< EBI signal: EBI_SDA10 on PD13 mux C */
#define MUX_PD13C_EBI_SDA10                        _L_(2)       /**< EBI signal line function value: EBI_SDA10 */
#define PIO_PD13C_EBI_SDA10                        _UL_(1 << 13) /**< EBI signal: EBI_SDA10 */
#define PIN_PC20A_EBI_SDA0                         _L_(84)      /**< EBI signal: EBI_SDA0 on PC20 mux A */
#define MUX_PC20A_EBI_SDA0                         _L_(0)       /**< EBI signal line function value: EBI_SDA0 */
#define PIO_PC20A_EBI_SDA0                         _UL_(1 << 20) /**< EBI signal: EBI_SDA0 */
#define PIN_PC21A_EBI_SDA1                         _L_(85)      /**< EBI signal: EBI_SDA1 on PC21 mux A */
#define MUX_PC21A_EBI_SDA1                         _L_(0)       /**< EBI signal line function value: EBI_SDA1 */
#define PIO_PC21A_EBI_SDA1                         _UL_(1 << 21) /**< EBI signal: EBI_SDA1 */
#define PIN_PC22A_EBI_SDA2                         _L_(86)      /**< EBI signal: EBI_SDA2 on PC22 mux A */
#define MUX_PC22A_EBI_SDA2                         _L_(0)       /**< EBI signal line function value: EBI_SDA2 */
#define PIO_PC22A_EBI_SDA2                         _UL_(1 << 22) /**< EBI signal: EBI_SDA2 */
#define PIN_PC23A_EBI_SDA3                         _L_(87)      /**< EBI signal: EBI_SDA3 on PC23 mux A */
#define MUX_PC23A_EBI_SDA3                         _L_(0)       /**< EBI signal line function value: EBI_SDA3 */
#define PIO_PC23A_EBI_SDA3                         _UL_(1 << 23) /**< EBI signal: EBI_SDA3 */
#define PIN_PC24A_EBI_SDA4                         _L_(88)      /**< EBI signal: EBI_SDA4 on PC24 mux A */
#define MUX_PC24A_EBI_SDA4                         _L_(0)       /**< EBI signal line function value: EBI_SDA4 */
#define PIO_PC24A_EBI_SDA4                         _UL_(1 << 24) /**< EBI signal: EBI_SDA4 */
#define PIN_PC25A_EBI_SDA5                         _L_(89)      /**< EBI signal: EBI_SDA5 on PC25 mux A */
#define MUX_PC25A_EBI_SDA5                         _L_(0)       /**< EBI signal line function value: EBI_SDA5 */
#define PIO_PC25A_EBI_SDA5                         _UL_(1 << 25) /**< EBI signal: EBI_SDA5 */
#define PIN_PC26A_EBI_SDA6                         _L_(90)      /**< EBI signal: EBI_SDA6 on PC26 mux A */
#define MUX_PC26A_EBI_SDA6                         _L_(0)       /**< EBI signal line function value: EBI_SDA6 */
#define PIO_PC26A_EBI_SDA6                         _UL_(1 << 26) /**< EBI signal: EBI_SDA6 */
#define PIN_PC27A_EBI_SDA7                         _L_(91)      /**< EBI signal: EBI_SDA7 on PC27 mux A */
#define MUX_PC27A_EBI_SDA7                         _L_(0)       /**< EBI signal line function value: EBI_SDA7 */
#define PIO_PC27A_EBI_SDA7                         _UL_(1 << 27) /**< EBI signal: EBI_SDA7 */
#define PIN_PC28A_EBI_SDA8                         _L_(92)      /**< EBI signal: EBI_SDA8 on PC28 mux A */
#define MUX_PC28A_EBI_SDA8                         _L_(0)       /**< EBI signal line function value: EBI_SDA8 */
#define PIO_PC28A_EBI_SDA8                         _UL_(1 << 28) /**< EBI signal: EBI_SDA8 */
#define PIN_PC29A_EBI_SDA9                         _L_(93)      /**< EBI signal: EBI_SDA9 on PC29 mux A */
#define MUX_PC29A_EBI_SDA9                         _L_(0)       /**< EBI signal line function value: EBI_SDA9 */
#define PIO_PC29A_EBI_SDA9                         _UL_(1 << 29) /**< EBI signal: EBI_SDA9 */
#define PIN_PC31A_EBI_SDA11                        _L_(95)      /**< EBI signal: EBI_SDA11 on PC31 mux A */
#define MUX_PC31A_EBI_SDA11                        _L_(0)       /**< EBI signal line function value: EBI_SDA11 */
#define PIO_PC31A_EBI_SDA11                        _UL_(1 << 31) /**< EBI signal: EBI_SDA11 */
#define PIN_PA18C_EBI_SDA12                        _L_(18)      /**< EBI signal: EBI_SDA12 on PA18 mux C */
#define MUX_PA18C_EBI_SDA12                        _L_(2)       /**< EBI signal line function value: EBI_SDA12 */
#define PIO_PA18C_EBI_SDA12                        _UL_(1 << 18) /**< EBI signal: EBI_SDA12 */
#define PIN_PA19C_EBI_SDA13                        _L_(19)      /**< EBI signal: EBI_SDA13 on PA19 mux C */
#define MUX_PA19C_EBI_SDA13                        _L_(2)       /**< EBI signal line function value: EBI_SDA13 */
#define PIO_PA19C_EBI_SDA13                        _UL_(1 << 19) /**< EBI signal: EBI_SDA13 */
#define PIN_PC18A_EBI_DQM0                         _L_(82)      /**< EBI signal: EBI_DQM0 on PC18 mux A */
#define MUX_PC18A_EBI_DQM0                         _L_(0)       /**< EBI signal line function value: EBI_DQM0 */
#define PIO_PC18A_EBI_DQM0                         _UL_(1 << 18) /**< EBI signal: EBI_DQM0 */
#define PIN_PD15C_EBI_DQM1                         _L_(111)     /**< EBI signal: EBI_DQM1 on PD15 mux C */
#define MUX_PD15C_EBI_DQM1                         _L_(2)       /**< EBI signal line function value: EBI_DQM1 */
#define PIO_PD15C_EBI_DQM1                         _UL_(1 << 15) /**< EBI signal: EBI_DQM1 */
/* ========== PIO definition for EFC peripheral ========== */
#define PIN_PB12X1_EFC_ERASE                       _L_(44)      /**< EFC signal: EFC_ERASE on PB12 mux X1 */
#define PIO_PB12X1_EFC_ERASE                       _UL_(1 << 12) /**< EFC signal: EFC_ERASE */
/* ========== PIO definition for GMAC peripheral ========== */
#define PIN_PD13A_GMAC_GCOL                        _L_(109)     /**< GMAC signal: GMAC_GCOL on PD13 mux A */
#define MUX_PD13A_GMAC_GCOL                        _L_(0)       /**< GMAC signal line function value: GMAC_GCOL */
#define PIO_PD13A_GMAC_GCOL                        _UL_(1 << 13) /**< GMAC signal: GMAC_GCOL */
#define PIN_PD10A_GMAC_GCRS                        _L_(106)     /**< GMAC signal: GMAC_GCRS on PD10 mux A */
#define MUX_PD10A_GMAC_GCRS                        _L_(0)       /**< GMAC signal line function value: GMAC_GCRS */
#define PIO_PD10A_GMAC_GCRS                        _UL_(1 << 10) /**< GMAC signal: GMAC_GCRS */
#define PIN_PD8A_GMAC_GMDC                         _L_(104)     /**< GMAC signal: GMAC_GMDC on PD8 mux A */
#define MUX_PD8A_GMAC_GMDC                         _L_(0)       /**< GMAC signal line function value: GMAC_GMDC */
#define PIO_PD8A_GMAC_GMDC                         _UL_(1 << 8) /**< GMAC signal: GMAC_GMDC */
#define PIN_PD9A_GMAC_GMDIO                        _L_(105)     /**< GMAC signal: GMAC_GMDIO on PD9 mux A */
#define MUX_PD9A_GMAC_GMDIO                        _L_(0)       /**< GMAC signal line function value: GMAC_GMDIO */
#define PIO_PD9A_GMAC_GMDIO                        _UL_(1 << 9) /**< GMAC signal: GMAC_GMDIO */
#define PIN_PD14A_GMAC_GRXCK                       _L_(110)     /**< GMAC signal: GMAC_GRXCK on PD14 mux A */
#define MUX_PD14A_GMAC_GRXCK                       _L_(0)       /**< GMAC signal line function value: GMAC_GRXCK */
#define PIO_PD14A_GMAC_GRXCK                       _UL_(1 << 14) /**< GMAC signal: GMAC_GRXCK */
#define PIN_PD4A_GMAC_GRXDV                        _L_(100)     /**< GMAC signal: GMAC_GRXDV on PD4 mux A */
#define MUX_PD4A_GMAC_GRXDV                        _L_(0)       /**< GMAC signal line function value: GMAC_GRXDV */
#define PIO_PD4A_GMAC_GRXDV                        _UL_(1 << 4) /**< GMAC signal: GMAC_GRXDV */
#define PIN_PD7A_GMAC_GRXER                        _L_(103)     /**< GMAC signal: GMAC_GRXER on PD7 mux A */
#define MUX_PD7A_GMAC_GRXER                        _L_(0)       /**< GMAC signal line function value: GMAC_GRXER */
#define PIO_PD7A_GMAC_GRXER                        _UL_(1 << 7) /**< GMAC signal: GMAC_GRXER */
#define PIN_PD5A_GMAC_GRX0                         _L_(101)     /**< GMAC signal: GMAC_GRX0 on PD5 mux A */
#define MUX_PD5A_GMAC_GRX0                         _L_(0)       /**< GMAC signal line function value: GMAC_GRX0 */
#define PIO_PD5A_GMAC_GRX0                         _UL_(1 << 5) /**< GMAC signal: GMAC_GRX0 */
#define PIN_PD6A_GMAC_GRX1                         _L_(102)     /**< GMAC signal: GMAC_GRX1 on PD6 mux A */
#define MUX_PD6A_GMAC_GRX1                         _L_(0)       /**< GMAC signal line function value: GMAC_GRX1 */
#define PIO_PD6A_GMAC_GRX1                         _UL_(1 << 6) /**< GMAC signal: GMAC_GRX1 */
#define PIN_PD11A_GMAC_GRX2                        _L_(107)     /**< GMAC signal: GMAC_GRX2 on PD11 mux A */
#define MUX_PD11A_GMAC_GRX2                        _L_(0)       /**< GMAC signal line function value: GMAC_GRX2 */
#define PIO_PD11A_GMAC_GRX2                        _UL_(1 << 11) /**< GMAC signal: GMAC_GRX2 */
#define PIN_PD12A_GMAC_GRX3                        _L_(108)     /**< GMAC signal: GMAC_GRX3 on PD12 mux A */
#define MUX_PD12A_GMAC_GRX3                        _L_(0)       /**< GMAC signal line function value: GMAC_GRX3 */
#define PIO_PD12A_GMAC_GRX3                        _UL_(1 << 12) /**< GMAC signal: GMAC_GRX3 */
#define PIN_PB1B_GMAC_GTSUCOMP                     _L_(33)      /**< GMAC signal: GMAC_GTSUCOMP on PB1 mux B */
#define MUX_PB1B_GMAC_GTSUCOMP                     _L_(1)       /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PB1B_GMAC_GTSUCOMP                     _UL_(1 << 1) /**< GMAC signal: GMAC_GTSUCOMP */
#define PIN_PB12B_GMAC_GTSUCOMP                    _L_(44)      /**< GMAC signal: GMAC_GTSUCOMP on PB12 mux B */
#define MUX_PB12B_GMAC_GTSUCOMP                    _L_(1)       /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PB12B_GMAC_GTSUCOMP                    _UL_(1 << 12) /**< GMAC signal: GMAC_GTSUCOMP */
#define PIN_PD11C_GMAC_GTSUCOMP                    _L_(107)     /**< GMAC signal: GMAC_GTSUCOMP on PD11 mux C */
#define MUX_PD11C_GMAC_GTSUCOMP                    _L_(2)       /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PD11C_GMAC_GTSUCOMP                    _UL_(1 << 11) /**< GMAC signal: GMAC_GTSUCOMP */
#define PIN_PD20C_GMAC_GTSUCOMP                    _L_(116)     /**< GMAC signal: GMAC_GTSUCOMP on PD20 mux C */
#define MUX_PD20C_GMAC_GTSUCOMP                    _L_(2)       /**< GMAC signal line function value: GMAC_GTSUCOMP */
#define PIO_PD20C_GMAC_GTSUCOMP                    _UL_(1 << 20) /**< GMAC signal: GMAC_GTSUCOMP */
#define PIN_PD0A_GMAC_GTXCK                        _L_(96)      /**< GMAC signal: GMAC_GTXCK on PD0 mux A */
#define MUX_PD0A_GMAC_GTXCK                        _L_(0)       /**< GMAC signal line function value: GMAC_GTXCK */
#define PIO_PD0A_GMAC_GTXCK                        _UL_(1 << 0) /**< GMAC signal: GMAC_GTXCK */
#define PIN_PD1A_GMAC_GTXEN                        _L_(97)      /**< GMAC signal: GMAC_GTXEN on PD1 mux A */
#define MUX_PD1A_GMAC_GTXEN                        _L_(0)       /**< GMAC signal line function value: GMAC_GTXEN */
#define PIO_PD1A_GMAC_GTXEN                        _UL_(1 << 1) /**< GMAC signal: GMAC_GTXEN */
#define PIN_PD17A_GMAC_GTXER                       _L_(113)     /**< GMAC signal: GMAC_GTXER on PD17 mux A */
#define MUX_PD17A_GMAC_GTXER                       _L_(0)       /**< GMAC signal line function value: GMAC_GTXER */
#define PIO_PD17A_GMAC_GTXER                       _UL_(1 << 17) /**< GMAC signal: GMAC_GTXER */
#define PIN_PD2A_GMAC_GTX0                         _L_(98)      /**< GMAC signal: GMAC_GTX0 on PD2 mux A */
#define MUX_PD2A_GMAC_GTX0                         _L_(0)       /**< GMAC signal line function value: GMAC_GTX0 */
#define PIO_PD2A_GMAC_GTX0                         _UL_(1 << 2) /**< GMAC signal: GMAC_GTX0 */
#define PIN_PD3A_GMAC_GTX1                         _L_(99)      /**< GMAC signal: GMAC_GTX1 on PD3 mux A */
#define MUX_PD3A_GMAC_GTX1                         _L_(0)       /**< GMAC signal line function value: GMAC_GTX1 */
#define PIO_PD3A_GMAC_GTX1                         _UL_(1 << 3) /**< GMAC signal: GMAC_GTX1 */
#define PIN_PD15A_GMAC_GTX2                        _L_(111)     /**< GMAC signal: GMAC_GTX2 on PD15 mux A */
#define MUX_PD15A_GMAC_GTX2                        _L_(0)       /**< GMAC signal line function value: GMAC_GTX2 */
#define PIO_PD15A_GMAC_GTX2                        _UL_(1 << 15) /**< GMAC signal: GMAC_GTX2 */
#define PIN_PD16A_GMAC_GTX3                        _L_(112)     /**< GMAC signal: GMAC_GTX3 on PD16 mux A */
#define MUX_PD16A_GMAC_GTX3                        _L_(0)       /**< GMAC signal line function value: GMAC_GTX3 */
#define PIO_PD16A_GMAC_GTX3                        _UL_(1 << 16) /**< GMAC signal: GMAC_GTX3 */
/* ========== PIO definition for HSMCI peripheral ========== */
#define PIN_PA28C_HSMCI_MCCDA                      _L_(28)      /**< HSMCI signal: HSMCI_MCCDA on PA28 mux C */
#define MUX_PA28C_HSMCI_MCCDA                      _L_(2)       /**< HSMCI signal line function value: HSMCI_MCCDA */
#define PIO_PA28C_HSMCI_MCCDA                      _UL_(1 << 28) /**< HSMCI signal: HSMCI_MCCDA */
#define PIN_PA25D_HSMCI_MCCK                       _L_(25)      /**< HSMCI signal: HSMCI_MCCK on PA25 mux D */
#define MUX_PA25D_HSMCI_MCCK                       _L_(3)       /**< HSMCI signal line function value: HSMCI_MCCK */
#define PIO_PA25D_HSMCI_MCCK                       _UL_(1 << 25) /**< HSMCI signal: HSMCI_MCCK */
#define PIN_PA30C_HSMCI_MCDA0                      _L_(30)      /**< HSMCI signal: HSMCI_MCDA0 on PA30 mux C */
#define MUX_PA30C_HSMCI_MCDA0                      _L_(2)       /**< HSMCI signal line function value: HSMCI_MCDA0 */
#define PIO_PA30C_HSMCI_MCDA0                      _UL_(1 << 30) /**< HSMCI signal: HSMCI_MCDA0 */
#define PIN_PA31C_HSMCI_MCDA1                      _L_(31)      /**< HSMCI signal: HSMCI_MCDA1 on PA31 mux C */
#define MUX_PA31C_HSMCI_MCDA1                      _L_(2)       /**< HSMCI signal line function value: HSMCI_MCDA1 */
#define PIO_PA31C_HSMCI_MCDA1                      _UL_(1 << 31) /**< HSMCI signal: HSMCI_MCDA1 */
#define PIN_PA26C_HSMCI_MCDA2                      _L_(26)      /**< HSMCI signal: HSMCI_MCDA2 on PA26 mux C */
#define MUX_PA26C_HSMCI_MCDA2                      _L_(2)       /**< HSMCI signal line function value: HSMCI_MCDA2 */
#define PIO_PA26C_HSMCI_MCDA2                      _UL_(1 << 26) /**< HSMCI signal: HSMCI_MCDA2 */
#define PIN_PA27C_HSMCI_MCDA3                      _L_(27)      /**< HSMCI signal: HSMCI_MCDA3 on PA27 mux C */
#define MUX_PA27C_HSMCI_MCDA3                      _L_(2)       /**< HSMCI signal line function value: HSMCI_MCDA3 */
#define PIO_PA27C_HSMCI_MCDA3                      _UL_(1 << 27) /**< HSMCI signal: HSMCI_MCDA3 */
/* ========== PIO definition for I2SC0 peripheral ========== */
#define PIN_PA1D_I2SC0_CK                          _L_(1)       /**< I2SC0 signal: I2SC0_CK on PA1 mux D */
#define MUX_PA1D_I2SC0_CK                          _L_(3)       /**< I2SC0 signal line function value: I2SC0_CK */
#define PIO_PA1D_I2SC0_CK                          _UL_(1 << 1) /**< I2SC0 signal: I2SC0_CK */
#define PIN_PA16D_I2SC0_DI0                        _L_(16)      /**< I2SC0 signal: I2SC0_DI0 on PA16 mux D */
#define MUX_PA16D_I2SC0_DI0                        _L_(3)       /**< I2SC0 signal line function value: I2SC0_DI0 */
#define PIO_PA16D_I2SC0_DI0                        _UL_(1 << 16) /**< I2SC0 signal: I2SC0_DI0 */
#define PIN_PA30D_I2SC0_DO0                        _L_(30)      /**< I2SC0 signal: I2SC0_DO0 on PA30 mux D */
#define MUX_PA30D_I2SC0_DO0                        _L_(3)       /**< I2SC0 signal line function value: I2SC0_DO0 */
#define PIO_PA30D_I2SC0_DO0                        _UL_(1 << 30) /**< I2SC0 signal: I2SC0_DO0 */
#define PIN_PA0D_I2SC0_MCK                         _L_(0)       /**< I2SC0 signal: I2SC0_MCK on PA0 mux D */
#define MUX_PA0D_I2SC0_MCK                         _L_(3)       /**< I2SC0 signal line function value: I2SC0_MCK */
#define PIO_PA0D_I2SC0_MCK                         _UL_(1 << 0) /**< I2SC0 signal: I2SC0_MCK */
#define PIN_PA15D_I2SC0_WS                         _L_(15)      /**< I2SC0 signal: I2SC0_WS on PA15 mux D */
#define MUX_PA15D_I2SC0_WS                         _L_(3)       /**< I2SC0 signal line function value: I2SC0_WS */
#define PIO_PA15D_I2SC0_WS                         _UL_(1 << 15) /**< I2SC0 signal: I2SC0_WS */
/* ========== PIO definition for I2SC1 peripheral ========== */
#define PIN_PA20D_I2SC1_CK                         _L_(20)      /**< I2SC1 signal: I2SC1_CK on PA20 mux D */
#define MUX_PA20D_I2SC1_CK                         _L_(3)       /**< I2SC1 signal line function value: I2SC1_CK */
#define PIO_PA20D_I2SC1_CK                         _UL_(1 << 20) /**< I2SC1 signal: I2SC1_CK */
#define PIN_PE2C_I2SC1_DI0                         _L_(130)     /**< I2SC1 signal: I2SC1_DI0 on PE2 mux C */
#define MUX_PE2C_I2SC1_DI0                         _L_(2)       /**< I2SC1 signal line function value: I2SC1_DI0 */
#define PIO_PE2C_I2SC1_DI0                         _UL_(1 << 2) /**< I2SC1 signal: I2SC1_DI0 */
#define PIN_PE1C_I2SC1_DO0                         _L_(129)     /**< I2SC1 signal: I2SC1_DO0 on PE1 mux C */
#define MUX_PE1C_I2SC1_DO0                         _L_(2)       /**< I2SC1 signal line function value: I2SC1_DO0 */
#define PIO_PE1C_I2SC1_DO0                         _UL_(1 << 1) /**< I2SC1 signal: I2SC1_DO0 */
#define PIN_PA19D_I2SC1_MCK                        _L_(19)      /**< I2SC1 signal: I2SC1_MCK on PA19 mux D */
#define MUX_PA19D_I2SC1_MCK                        _L_(3)       /**< I2SC1 signal line function value: I2SC1_MCK */
#define PIO_PA19D_I2SC1_MCK                        _UL_(1 << 19) /**< I2SC1 signal: I2SC1_MCK */
#define PIN_PE0C_I2SC1_WS                          _L_(128)     /**< I2SC1 signal: I2SC1_WS on PE0 mux C */
#define MUX_PE0C_I2SC1_WS                          _L_(2)       /**< I2SC1 signal line function value: I2SC1_WS */
#define PIO_PE0C_I2SC1_WS                          _UL_(1 << 0) /**< I2SC1 signal: I2SC1_WS */
/* ========== PIO definition for ISI peripheral ========== */
#define PIN_PD22D_ISI_D0                           _L_(118)     /**< ISI signal: ISI_D0 on PD22 mux D */
#define MUX_PD22D_ISI_D0                           _L_(3)       /**< ISI signal line function value: ISI_D0 */
#define PIO_PD22D_ISI_D0                           _UL_(1 << 22) /**< ISI signal: ISI_D0 */
#define PIN_PD21D_ISI_D1                           _L_(117)     /**< ISI signal: ISI_D1 on PD21 mux D */
#define MUX_PD21D_ISI_D1                           _L_(3)       /**< ISI signal line function value: ISI_D1 */
#define PIO_PD21D_ISI_D1                           _UL_(1 << 21) /**< ISI signal: ISI_D1 */
#define PIN_PB3D_ISI_D2                            _L_(35)      /**< ISI signal: ISI_D2 on PB3 mux D */
#define MUX_PB3D_ISI_D2                            _L_(3)       /**< ISI signal line function value: ISI_D2 */
#define PIO_PB3D_ISI_D2                            _UL_(1 << 3) /**< ISI signal: ISI_D2 */
#define PIN_PA9B_ISI_D3                            _L_(9)       /**< ISI signal: ISI_D3 on PA9 mux B */
#define MUX_PA9B_ISI_D3                            _L_(1)       /**< ISI signal line function value: ISI_D3 */
#define PIO_PA9B_ISI_D3                            _UL_(1 << 9) /**< ISI signal: ISI_D3 */
#define PIN_PA5B_ISI_D4                            _L_(5)       /**< ISI signal: ISI_D4 on PA5 mux B */
#define MUX_PA5B_ISI_D4                            _L_(1)       /**< ISI signal line function value: ISI_D4 */
#define PIO_PA5B_ISI_D4                            _UL_(1 << 5) /**< ISI signal: ISI_D4 */
#define PIN_PD11D_ISI_D5                           _L_(107)     /**< ISI signal: ISI_D5 on PD11 mux D */
#define MUX_PD11D_ISI_D5                           _L_(3)       /**< ISI signal line function value: ISI_D5 */
#define PIO_PD11D_ISI_D5                           _UL_(1 << 11) /**< ISI signal: ISI_D5 */
#define PIN_PD12D_ISI_D6                           _L_(108)     /**< ISI signal: ISI_D6 on PD12 mux D */
#define MUX_PD12D_ISI_D6                           _L_(3)       /**< ISI signal line function value: ISI_D6 */
#define PIO_PD12D_ISI_D6                           _UL_(1 << 12) /**< ISI signal: ISI_D6 */
#define PIN_PA27D_ISI_D7                           _L_(27)      /**< ISI signal: ISI_D7 on PA27 mux D */
#define MUX_PA27D_ISI_D7                           _L_(3)       /**< ISI signal line function value: ISI_D7 */
#define PIO_PA27D_ISI_D7                           _UL_(1 << 27) /**< ISI signal: ISI_D7 */
#define PIN_PD27D_ISI_D8                           _L_(123)     /**< ISI signal: ISI_D8 on PD27 mux D */
#define MUX_PD27D_ISI_D8                           _L_(3)       /**< ISI signal line function value: ISI_D8 */
#define PIO_PD27D_ISI_D8                           _UL_(1 << 27) /**< ISI signal: ISI_D8 */
#define PIN_PD28D_ISI_D9                           _L_(124)     /**< ISI signal: ISI_D9 on PD28 mux D */
#define MUX_PD28D_ISI_D9                           _L_(3)       /**< ISI signal line function value: ISI_D9 */
#define PIO_PD28D_ISI_D9                           _UL_(1 << 28) /**< ISI signal: ISI_D9 */
#define PIN_PD30D_ISI_D10                          _L_(126)     /**< ISI signal: ISI_D10 on PD30 mux D */
#define MUX_PD30D_ISI_D10                          _L_(3)       /**< ISI signal line function value: ISI_D10 */
#define PIO_PD30D_ISI_D10                          _UL_(1 << 30) /**< ISI signal: ISI_D10 */
#define PIN_PD31D_ISI_D11                          _L_(127)     /**< ISI signal: ISI_D11 on PD31 mux D */
#define MUX_PD31D_ISI_D11                          _L_(3)       /**< ISI signal line function value: ISI_D11 */
#define PIO_PD31D_ISI_D11                          _UL_(1 << 31) /**< ISI signal: ISI_D11 */
#define PIN_PD24D_ISI_HSYNC                        _L_(120)     /**< ISI signal: ISI_HSYNC on PD24 mux D */
#define MUX_PD24D_ISI_HSYNC                        _L_(3)       /**< ISI signal line function value: ISI_HSYNC */
#define PIO_PD24D_ISI_HSYNC                        _UL_(1 << 24) /**< ISI signal: ISI_HSYNC */
#define PIN_PA24D_ISI_PCK                          _L_(24)      /**< ISI signal: ISI_PCK on PA24 mux D */
#define MUX_PA24D_ISI_PCK                          _L_(3)       /**< ISI signal line function value: ISI_PCK */
#define PIO_PA24D_ISI_PCK                          _UL_(1 << 24) /**< ISI signal: ISI_PCK */
#define PIN_PD25D_ISI_VSYNC                        _L_(121)     /**< ISI signal: ISI_VSYNC on PD25 mux D */
#define MUX_PD25D_ISI_VSYNC                        _L_(3)       /**< ISI signal line function value: ISI_VSYNC */
#define PIO_PD25D_ISI_VSYNC                        _UL_(1 << 25) /**< ISI signal: ISI_VSYNC */
/* ========== PIO definition for MCAN0 peripheral ========== */
#define PIN_PB3A_MCAN0_CANRX0                      _L_(35)      /**< MCAN0 signal: MCAN0_CANRX0 on PB3 mux A */
#define MUX_PB3A_MCAN0_CANRX0                      _L_(0)       /**< MCAN0 signal line function value: MCAN0_CANRX0 */
#define PIO_PB3A_MCAN0_CANRX0                      _UL_(1 << 3) /**< MCAN0 signal: MCAN0_CANRX0 */
#define PIN_PB2A_MCAN0_CANTX0                      _L_(34)      /**< MCAN0 signal: MCAN0_CANTX0 on PB2 mux A */
#define MUX_PB2A_MCAN0_CANTX0                      _L_(0)       /**< MCAN0 signal line function value: MCAN0_CANTX0 */
#define PIO_PB2A_MCAN0_CANTX0                      _UL_(1 << 2) /**< MCAN0 signal: MCAN0_CANTX0 */
/* ========== PIO definition for MCAN1 peripheral ========== */
#define PIN_PC12C_MCAN1_CANRX1                     _L_(76)      /**< MCAN1 signal: MCAN1_CANRX1 on PC12 mux C */
#define MUX_PC12C_MCAN1_CANRX1                     _L_(2)       /**< MCAN1 signal line function value: MCAN1_CANRX1 */
#define PIO_PC12C_MCAN1_CANRX1                     _UL_(1 << 12) /**< MCAN1 signal: MCAN1_CANRX1 */
#define PIN_PC14C_MCAN1_CANTX1                     _L_(78)      /**< MCAN1 signal: MCAN1_CANTX1 on PC14 mux C */
#define MUX_PC14C_MCAN1_CANTX1                     _L_(2)       /**< MCAN1 signal line function value: MCAN1_CANTX1 */
#define PIO_PC14C_MCAN1_CANTX1                     _UL_(1 << 14) /**< MCAN1 signal: MCAN1_CANTX1 */
#define PIN_PD12B_MCAN1_CANTX1                     _L_(108)     /**< MCAN1 signal: MCAN1_CANTX1 on PD12 mux B */
#define MUX_PD12B_MCAN1_CANTX1                     _L_(1)       /**< MCAN1 signal line function value: MCAN1_CANTX1 */
#define PIO_PD12B_MCAN1_CANTX1                     _UL_(1 << 12) /**< MCAN1 signal: MCAN1_CANTX1 */
/* ========== PIO definition for PMC peripheral ========== */
#define PIN_PA6B_PMC_PCK0                          _L_(6)       /**< PMC signal: PMC_PCK0 on PA6 mux B */
#define MUX_PA6B_PMC_PCK0                          _L_(1)       /**< PMC signal line function value: PMC_PCK0 */
#define PIO_PA6B_PMC_PCK0                          _UL_(1 << 6) /**< PMC signal: PMC_PCK0 */
#define PIN_PB12D_PMC_PCK0                         _L_(44)      /**< PMC signal: PMC_PCK0 on PB12 mux D */
#define MUX_PB12D_PMC_PCK0                         _L_(3)       /**< PMC signal line function value: PMC_PCK0 */
#define PIO_PB12D_PMC_PCK0                         _UL_(1 << 12) /**< PMC signal: PMC_PCK0 */
#define PIN_PB13B_PMC_PCK0                         _L_(45)      /**< PMC signal: PMC_PCK0 on PB13 mux B */
#define MUX_PB13B_PMC_PCK0                         _L_(1)       /**< PMC signal line function value: PMC_PCK0 */
#define PIO_PB13B_PMC_PCK0                         _UL_(1 << 13) /**< PMC signal: PMC_PCK0 */
#define PIN_PA17B_PMC_PCK1                         _L_(17)      /**< PMC signal: PMC_PCK1 on PA17 mux B */
#define MUX_PA17B_PMC_PCK1                         _L_(1)       /**< PMC signal line function value: PMC_PCK1 */
#define PIO_PA17B_PMC_PCK1                         _UL_(1 << 17) /**< PMC signal: PMC_PCK1 */
#define PIN_PA21B_PMC_PCK1                         _L_(21)      /**< PMC signal: PMC_PCK1 on PA21 mux B */
#define MUX_PA21B_PMC_PCK1                         _L_(1)       /**< PMC signal line function value: PMC_PCK1 */
#define PIO_PA21B_PMC_PCK1                         _UL_(1 << 21) /**< PMC signal: PMC_PCK1 */
#define PIN_PA3C_PMC_PCK2                          _L_(3)       /**< PMC signal: PMC_PCK2 on PA3 mux C */
#define MUX_PA3C_PMC_PCK2                          _L_(2)       /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PA3C_PMC_PCK2                          _UL_(1 << 3) /**< PMC signal: PMC_PCK2 */
#define PIN_PA18B_PMC_PCK2                         _L_(18)      /**< PMC signal: PMC_PCK2 on PA18 mux B */
#define MUX_PA18B_PMC_PCK2                         _L_(1)       /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PA18B_PMC_PCK2                         _UL_(1 << 18) /**< PMC signal: PMC_PCK2 */
#define PIN_PA31B_PMC_PCK2                         _L_(31)      /**< PMC signal: PMC_PCK2 on PA31 mux B */
#define MUX_PA31B_PMC_PCK2                         _L_(1)       /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PA31B_PMC_PCK2                         _UL_(1 << 31) /**< PMC signal: PMC_PCK2 */
#define PIN_PB3B_PMC_PCK2                          _L_(35)      /**< PMC signal: PMC_PCK2 on PB3 mux B */
#define MUX_PB3B_PMC_PCK2                          _L_(1)       /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PB3B_PMC_PCK2                          _UL_(1 << 3) /**< PMC signal: PMC_PCK2 */
#define PIN_PD31C_PMC_PCK2                         _L_(127)     /**< PMC signal: PMC_PCK2 on PD31 mux C */
#define MUX_PD31C_PMC_PCK2                         _L_(2)       /**< PMC signal line function value: PMC_PCK2 */
#define PIO_PD31C_PMC_PCK2                         _UL_(1 << 31) /**< PMC signal: PMC_PCK2 */
#define PIN_PB9X1_PMC_XIN                          _L_(41)      /**< PMC signal: PMC_XIN on PB9 mux X1 */
#define PIO_PB9X1_PMC_XIN                          _UL_(1 << 9) /**< PMC signal: PMC_XIN */
#define PIN_PB8X1_PMC_XOUT                         _L_(40)      /**< PMC signal: PMC_XOUT on PB8 mux X1 */
#define PIO_PB8X1_PMC_XOUT                         _UL_(1 << 8) /**< PMC signal: PMC_XOUT */
#define PIN_PA7X1_PMC_XIN32                        _L_(7)       /**< PMC signal: PMC_XIN32 on PA7 mux X1 */
#define PIO_PA7X1_PMC_XIN32                        _UL_(1 << 7) /**< PMC signal: PMC_XIN32 */
#define PIN_PA8X1_PMC_XOUT32                       _L_(8)       /**< PMC signal: PMC_XOUT32 on PA8 mux X1 */
#define PIO_PA8X1_PMC_XOUT32                       _UL_(1 << 8) /**< PMC signal: PMC_XOUT32 */
/* ========== PIO definition for PWM0 peripheral ========== */
#define PIN_PA10B_PWM0_PWMEXTRG0                   _L_(10)      /**< PWM0 signal: PWM0_PWMEXTRG0 on PA10 mux B */
#define MUX_PA10B_PWM0_PWMEXTRG0                   _L_(1)       /**< PWM0 signal line function value: PWM0_PWMEXTRG0 */
#define PIO_PA10B_PWM0_PWMEXTRG0                   _UL_(1 << 10) /**< PWM0 signal: PWM0_PWMEXTRG0 */
#define PIN_PA22B_PWM0_PWMEXTRG1                   _L_(22)      /**< PWM0 signal: PWM0_PWMEXTRG1 on PA22 mux B */
#define MUX_PA22B_PWM0_PWMEXTRG1                   _L_(1)       /**< PWM0 signal line function value: PWM0_PWMEXTRG1 */
#define PIO_PA22B_PWM0_PWMEXTRG1                   _UL_(1 << 22) /**< PWM0 signal: PWM0_PWMEXTRG1 */
#define PIN_PA9C_PWM0_PWMFI0                       _L_(9)       /**< PWM0 signal: PWM0_PWMFI0 on PA9 mux C */
#define MUX_PA9C_PWM0_PWMFI0                       _L_(2)       /**< PWM0 signal line function value: PWM0_PWMFI0 */
#define PIO_PA9C_PWM0_PWMFI0                       _UL_(1 << 9) /**< PWM0 signal: PWM0_PWMFI0 */
#define PIN_PD8B_PWM0_PWMFI1                       _L_(104)     /**< PWM0 signal: PWM0_PWMFI1 on PD8 mux B */
#define MUX_PD8B_PWM0_PWMFI1                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMFI1 */
#define PIO_PD8B_PWM0_PWMFI1                       _UL_(1 << 8) /**< PWM0 signal: PWM0_PWMFI1 */
#define PIN_PD9B_PWM0_PWMFI2                       _L_(105)     /**< PWM0 signal: PWM0_PWMFI2 on PD9 mux B */
#define MUX_PD9B_PWM0_PWMFI2                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMFI2 */
#define PIO_PD9B_PWM0_PWMFI2                       _UL_(1 << 9) /**< PWM0 signal: PWM0_PWMFI2 */
#define PIN_PA0A_PWM0_PWMH0                        _L_(0)       /**< PWM0 signal: PWM0_PWMH0 on PA0 mux A */
#define MUX_PA0A_PWM0_PWMH0                        _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PA0A_PWM0_PWMH0                        _UL_(1 << 0) /**< PWM0 signal: PWM0_PWMH0 */
#define PIN_PA11B_PWM0_PWMH0                       _L_(11)      /**< PWM0 signal: PWM0_PWMH0 on PA11 mux B */
#define MUX_PA11B_PWM0_PWMH0                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PA11B_PWM0_PWMH0                       _UL_(1 << 11) /**< PWM0 signal: PWM0_PWMH0 */
#define PIN_PA23B_PWM0_PWMH0                       _L_(23)      /**< PWM0 signal: PWM0_PWMH0 on PA23 mux B */
#define MUX_PA23B_PWM0_PWMH0                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PA23B_PWM0_PWMH0                       _UL_(1 << 23) /**< PWM0 signal: PWM0_PWMH0 */
#define PIN_PB0A_PWM0_PWMH0                        _L_(32)      /**< PWM0 signal: PWM0_PWMH0 on PB0 mux A */
#define MUX_PB0A_PWM0_PWMH0                        _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PB0A_PWM0_PWMH0                        _UL_(1 << 0) /**< PWM0 signal: PWM0_PWMH0 */
#define PIN_PD11B_PWM0_PWMH0                       _L_(107)     /**< PWM0 signal: PWM0_PWMH0 on PD11 mux B */
#define MUX_PD11B_PWM0_PWMH0                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PD11B_PWM0_PWMH0                       _UL_(1 << 11) /**< PWM0 signal: PWM0_PWMH0 */
#define PIN_PD20A_PWM0_PWMH0                       _L_(116)     /**< PWM0 signal: PWM0_PWMH0 on PD20 mux A */
#define MUX_PD20A_PWM0_PWMH0                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH0 */
#define PIO_PD20A_PWM0_PWMH0                       _UL_(1 << 20) /**< PWM0 signal: PWM0_PWMH0 */
#define PIN_PA2A_PWM0_PWMH1                        _L_(2)       /**< PWM0 signal: PWM0_PWMH1 on PA2 mux A */
#define MUX_PA2A_PWM0_PWMH1                        _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PA2A_PWM0_PWMH1                        _UL_(1 << 2) /**< PWM0 signal: PWM0_PWMH1 */
#define PIN_PA12B_PWM0_PWMH1                       _L_(12)      /**< PWM0 signal: PWM0_PWMH1 on PA12 mux B */
#define MUX_PA12B_PWM0_PWMH1                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PA12B_PWM0_PWMH1                       _UL_(1 << 12) /**< PWM0 signal: PWM0_PWMH1 */
#define PIN_PA24B_PWM0_PWMH1                       _L_(24)      /**< PWM0 signal: PWM0_PWMH1 on PA24 mux B */
#define MUX_PA24B_PWM0_PWMH1                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PA24B_PWM0_PWMH1                       _UL_(1 << 24) /**< PWM0 signal: PWM0_PWMH1 */
#define PIN_PB1A_PWM0_PWMH1                        _L_(33)      /**< PWM0 signal: PWM0_PWMH1 on PB1 mux A */
#define MUX_PB1A_PWM0_PWMH1                        _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PB1A_PWM0_PWMH1                        _UL_(1 << 1) /**< PWM0 signal: PWM0_PWMH1 */
#define PIN_PD21A_PWM0_PWMH1                       _L_(117)     /**< PWM0 signal: PWM0_PWMH1 on PD21 mux A */
#define MUX_PD21A_PWM0_PWMH1                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH1 */
#define PIO_PD21A_PWM0_PWMH1                       _UL_(1 << 21) /**< PWM0 signal: PWM0_PWMH1 */
#define PIN_PA13B_PWM0_PWMH2                       _L_(13)      /**< PWM0 signal: PWM0_PWMH2 on PA13 mux B */
#define MUX_PA13B_PWM0_PWMH2                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PA13B_PWM0_PWMH2                       _UL_(1 << 13) /**< PWM0 signal: PWM0_PWMH2 */
#define PIN_PA25B_PWM0_PWMH2                       _L_(25)      /**< PWM0 signal: PWM0_PWMH2 on PA25 mux B */
#define MUX_PA25B_PWM0_PWMH2                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PA25B_PWM0_PWMH2                       _UL_(1 << 25) /**< PWM0 signal: PWM0_PWMH2 */
#define PIN_PB4B_PWM0_PWMH2                        _L_(36)      /**< PWM0 signal: PWM0_PWMH2 on PB4 mux B */
#define MUX_PB4B_PWM0_PWMH2                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PB4B_PWM0_PWMH2                        _UL_(1 << 4) /**< PWM0 signal: PWM0_PWMH2 */
#define PIN_PC19B_PWM0_PWMH2                       _L_(83)      /**< PWM0 signal: PWM0_PWMH2 on PC19 mux B */
#define MUX_PC19B_PWM0_PWMH2                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PC19B_PWM0_PWMH2                       _UL_(1 << 19) /**< PWM0 signal: PWM0_PWMH2 */
#define PIN_PD22A_PWM0_PWMH2                       _L_(118)     /**< PWM0 signal: PWM0_PWMH2 on PD22 mux A */
#define MUX_PD22A_PWM0_PWMH2                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH2 */
#define PIO_PD22A_PWM0_PWMH2                       _UL_(1 << 22) /**< PWM0 signal: PWM0_PWMH2 */
#define PIN_PA7B_PWM0_PWMH3                        _L_(7)       /**< PWM0 signal: PWM0_PWMH3 on PA7 mux B */
#define MUX_PA7B_PWM0_PWMH3                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PA7B_PWM0_PWMH3                        _UL_(1 << 7) /**< PWM0 signal: PWM0_PWMH3 */
#define PIN_PA14B_PWM0_PWMH3                       _L_(14)      /**< PWM0 signal: PWM0_PWMH3 on PA14 mux B */
#define MUX_PA14B_PWM0_PWMH3                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PA14B_PWM0_PWMH3                       _UL_(1 << 14) /**< PWM0 signal: PWM0_PWMH3 */
#define PIN_PA17C_PWM0_PWMH3                       _L_(17)      /**< PWM0 signal: PWM0_PWMH3 on PA17 mux C */
#define MUX_PA17C_PWM0_PWMH3                       _L_(2)       /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PA17C_PWM0_PWMH3                       _UL_(1 << 17) /**< PWM0 signal: PWM0_PWMH3 */
#define PIN_PC13B_PWM0_PWMH3                       _L_(77)      /**< PWM0 signal: PWM0_PWMH3 on PC13 mux B */
#define MUX_PC13B_PWM0_PWMH3                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PC13B_PWM0_PWMH3                       _UL_(1 << 13) /**< PWM0 signal: PWM0_PWMH3 */
#define PIN_PC21B_PWM0_PWMH3                       _L_(85)      /**< PWM0 signal: PWM0_PWMH3 on PC21 mux B */
#define MUX_PC21B_PWM0_PWMH3                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PC21B_PWM0_PWMH3                       _UL_(1 << 21) /**< PWM0 signal: PWM0_PWMH3 */
#define PIN_PD23A_PWM0_PWMH3                       _L_(119)     /**< PWM0 signal: PWM0_PWMH3 on PD23 mux A */
#define MUX_PD23A_PWM0_PWMH3                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWMH3 */
#define PIO_PD23A_PWM0_PWMH3                       _UL_(1 << 23) /**< PWM0 signal: PWM0_PWMH3 */
#define PIN_PA1A_PWM0_PWML0                        _L_(1)       /**< PWM0 signal: PWM0_PWML0 on PA1 mux A */
#define MUX_PA1A_PWM0_PWML0                        _L_(0)       /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PA1A_PWM0_PWML0                        _UL_(1 << 1) /**< PWM0 signal: PWM0_PWML0 */
#define PIN_PA19B_PWM0_PWML0                       _L_(19)      /**< PWM0 signal: PWM0_PWML0 on PA19 mux B */
#define MUX_PA19B_PWM0_PWML0                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PA19B_PWM0_PWML0                       _UL_(1 << 19) /**< PWM0 signal: PWM0_PWML0 */
#define PIN_PB5B_PWM0_PWML0                        _L_(37)      /**< PWM0 signal: PWM0_PWML0 on PB5 mux B */
#define MUX_PB5B_PWM0_PWML0                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PB5B_PWM0_PWML0                        _UL_(1 << 5) /**< PWM0 signal: PWM0_PWML0 */
#define PIN_PC0B_PWM0_PWML0                        _L_(64)      /**< PWM0 signal: PWM0_PWML0 on PC0 mux B */
#define MUX_PC0B_PWM0_PWML0                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PC0B_PWM0_PWML0                        _UL_(1 << 0) /**< PWM0 signal: PWM0_PWML0 */
#define PIN_PD10B_PWM0_PWML0                       _L_(106)     /**< PWM0 signal: PWM0_PWML0 on PD10 mux B */
#define MUX_PD10B_PWM0_PWML0                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PD10B_PWM0_PWML0                       _UL_(1 << 10) /**< PWM0 signal: PWM0_PWML0 */
#define PIN_PD24A_PWM0_PWML0                       _L_(120)     /**< PWM0 signal: PWM0_PWML0 on PD24 mux A */
#define MUX_PD24A_PWM0_PWML0                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML0 */
#define PIO_PD24A_PWM0_PWML0                       _UL_(1 << 24) /**< PWM0 signal: PWM0_PWML0 */
#define PIN_PA20B_PWM0_PWML1                       _L_(20)      /**< PWM0 signal: PWM0_PWML1 on PA20 mux B */
#define MUX_PA20B_PWM0_PWML1                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PA20B_PWM0_PWML1                       _UL_(1 << 20) /**< PWM0 signal: PWM0_PWML1 */
#define PIN_PB12A_PWM0_PWML1                       _L_(44)      /**< PWM0 signal: PWM0_PWML1 on PB12 mux A */
#define MUX_PB12A_PWM0_PWML1                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PB12A_PWM0_PWML1                       _UL_(1 << 12) /**< PWM0 signal: PWM0_PWML1 */
#define PIN_PC1B_PWM0_PWML1                        _L_(65)      /**< PWM0 signal: PWM0_PWML1 on PC1 mux B */
#define MUX_PC1B_PWM0_PWML1                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PC1B_PWM0_PWML1                        _UL_(1 << 1) /**< PWM0 signal: PWM0_PWML1 */
#define PIN_PC18B_PWM0_PWML1                       _L_(82)      /**< PWM0 signal: PWM0_PWML1 on PC18 mux B */
#define MUX_PC18B_PWM0_PWML1                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PC18B_PWM0_PWML1                       _UL_(1 << 18) /**< PWM0 signal: PWM0_PWML1 */
#define PIN_PD25A_PWM0_PWML1                       _L_(121)     /**< PWM0 signal: PWM0_PWML1 on PD25 mux A */
#define MUX_PD25A_PWM0_PWML1                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML1 */
#define PIO_PD25A_PWM0_PWML1                       _UL_(1 << 25) /**< PWM0 signal: PWM0_PWML1 */
#define PIN_PA16C_PWM0_PWML2                       _L_(16)      /**< PWM0 signal: PWM0_PWML2 on PA16 mux C */
#define MUX_PA16C_PWM0_PWML2                       _L_(2)       /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PA16C_PWM0_PWML2                       _UL_(1 << 16) /**< PWM0 signal: PWM0_PWML2 */
#define PIN_PA30A_PWM0_PWML2                       _L_(30)      /**< PWM0 signal: PWM0_PWML2 on PA30 mux A */
#define MUX_PA30A_PWM0_PWML2                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PA30A_PWM0_PWML2                       _UL_(1 << 30) /**< PWM0 signal: PWM0_PWML2 */
#define PIN_PB13A_PWM0_PWML2                       _L_(45)      /**< PWM0 signal: PWM0_PWML2 on PB13 mux A */
#define MUX_PB13A_PWM0_PWML2                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PB13A_PWM0_PWML2                       _UL_(1 << 13) /**< PWM0 signal: PWM0_PWML2 */
#define PIN_PC2B_PWM0_PWML2                        _L_(66)      /**< PWM0 signal: PWM0_PWML2 on PC2 mux B */
#define MUX_PC2B_PWM0_PWML2                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PC2B_PWM0_PWML2                        _UL_(1 << 2) /**< PWM0 signal: PWM0_PWML2 */
#define PIN_PC20B_PWM0_PWML2                       _L_(84)      /**< PWM0 signal: PWM0_PWML2 on PC20 mux B */
#define MUX_PC20B_PWM0_PWML2                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PC20B_PWM0_PWML2                       _UL_(1 << 20) /**< PWM0 signal: PWM0_PWML2 */
#define PIN_PD26A_PWM0_PWML2                       _L_(122)     /**< PWM0 signal: PWM0_PWML2 on PD26 mux A */
#define MUX_PD26A_PWM0_PWML2                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML2 */
#define PIO_PD26A_PWM0_PWML2                       _UL_(1 << 26) /**< PWM0 signal: PWM0_PWML2 */
#define PIN_PA15C_PWM0_PWML3                       _L_(15)      /**< PWM0 signal: PWM0_PWML3 on PA15 mux C */
#define MUX_PA15C_PWM0_PWML3                       _L_(2)       /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PA15C_PWM0_PWML3                       _UL_(1 << 15) /**< PWM0 signal: PWM0_PWML3 */
#define PIN_PC3B_PWM0_PWML3                        _L_(67)      /**< PWM0 signal: PWM0_PWML3 on PC3 mux B */
#define MUX_PC3B_PWM0_PWML3                        _L_(1)       /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PC3B_PWM0_PWML3                        _UL_(1 << 3) /**< PWM0 signal: PWM0_PWML3 */
#define PIN_PC15B_PWM0_PWML3                       _L_(79)      /**< PWM0 signal: PWM0_PWML3 on PC15 mux B */
#define MUX_PC15B_PWM0_PWML3                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PC15B_PWM0_PWML3                       _UL_(1 << 15) /**< PWM0 signal: PWM0_PWML3 */
#define PIN_PC22B_PWM0_PWML3                       _L_(86)      /**< PWM0 signal: PWM0_PWML3 on PC22 mux B */
#define MUX_PC22B_PWM0_PWML3                       _L_(1)       /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PC22B_PWM0_PWML3                       _UL_(1 << 22) /**< PWM0 signal: PWM0_PWML3 */
#define PIN_PD27A_PWM0_PWML3                       _L_(123)     /**< PWM0 signal: PWM0_PWML3 on PD27 mux A */
#define MUX_PD27A_PWM0_PWML3                       _L_(0)       /**< PWM0 signal line function value: PWM0_PWML3 */
#define PIO_PD27A_PWM0_PWML3                       _UL_(1 << 27) /**< PWM0 signal: PWM0_PWML3 */
/* ========== PIO definition for PWM1 peripheral ========== */
#define PIN_PA30B_PWM1_PWMEXTRG0                   _L_(30)      /**< PWM1 signal: PWM1_PWMEXTRG0 on PA30 mux B */
#define MUX_PA30B_PWM1_PWMEXTRG0                   _L_(1)       /**< PWM1 signal line function value: PWM1_PWMEXTRG0 */
#define PIO_PA30B_PWM1_PWMEXTRG0                   _UL_(1 << 30) /**< PWM1 signal: PWM1_PWMEXTRG0 */
#define PIN_PA18A_PWM1_PWMEXTRG1                   _L_(18)      /**< PWM1 signal: PWM1_PWMEXTRG1 on PA18 mux A */
#define MUX_PA18A_PWM1_PWMEXTRG1                   _L_(0)       /**< PWM1 signal line function value: PWM1_PWMEXTRG1 */
#define PIO_PA18A_PWM1_PWMEXTRG1                   _UL_(1 << 18) /**< PWM1 signal: PWM1_PWMEXTRG1 */
#define PIN_PA21C_PWM1_PWMFI0                      _L_(21)      /**< PWM1 signal: PWM1_PWMFI0 on PA21 mux C */
#define MUX_PA21C_PWM1_PWMFI0                      _L_(2)       /**< PWM1 signal line function value: PWM1_PWMFI0 */
#define PIO_PA21C_PWM1_PWMFI0                      _UL_(1 << 21) /**< PWM1 signal: PWM1_PWMFI0 */
#define PIN_PA26D_PWM1_PWMFI1                      _L_(26)      /**< PWM1 signal: PWM1_PWMFI1 on PA26 mux D */
#define MUX_PA26D_PWM1_PWMFI1                      _L_(3)       /**< PWM1 signal line function value: PWM1_PWMFI1 */
#define PIO_PA26D_PWM1_PWMFI1                      _UL_(1 << 26) /**< PWM1 signal: PWM1_PWMFI1 */
#define PIN_PA28D_PWM1_PWMFI2                      _L_(28)      /**< PWM1 signal: PWM1_PWMFI2 on PA28 mux D */
#define MUX_PA28D_PWM1_PWMFI2                      _L_(3)       /**< PWM1 signal line function value: PWM1_PWMFI2 */
#define PIO_PA28D_PWM1_PWMFI2                      _UL_(1 << 28) /**< PWM1 signal: PWM1_PWMFI2 */
#define PIN_PA12C_PWM1_PWMH0                       _L_(12)      /**< PWM1 signal: PWM1_PWMH0 on PA12 mux C */
#define MUX_PA12C_PWM1_PWMH0                       _L_(2)       /**< PWM1 signal line function value: PWM1_PWMH0 */
#define PIO_PA12C_PWM1_PWMH0                       _UL_(1 << 12) /**< PWM1 signal: PWM1_PWMH0 */
#define PIN_PD1B_PWM1_PWMH0                        _L_(97)      /**< PWM1 signal: PWM1_PWMH0 on PD1 mux B */
#define MUX_PD1B_PWM1_PWMH0                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWMH0 */
#define PIO_PD1B_PWM1_PWMH0                        _UL_(1 << 1) /**< PWM1 signal: PWM1_PWMH0 */
#define PIN_PA14C_PWM1_PWMH1                       _L_(14)      /**< PWM1 signal: PWM1_PWMH1 on PA14 mux C */
#define MUX_PA14C_PWM1_PWMH1                       _L_(2)       /**< PWM1 signal line function value: PWM1_PWMH1 */
#define PIO_PA14C_PWM1_PWMH1                       _UL_(1 << 14) /**< PWM1 signal: PWM1_PWMH1 */
#define PIN_PD3B_PWM1_PWMH1                        _L_(99)      /**< PWM1 signal: PWM1_PWMH1 on PD3 mux B */
#define MUX_PD3B_PWM1_PWMH1                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWMH1 */
#define PIO_PD3B_PWM1_PWMH1                        _UL_(1 << 3) /**< PWM1 signal: PWM1_PWMH1 */
#define PIN_PA31D_PWM1_PWMH2                       _L_(31)      /**< PWM1 signal: PWM1_PWMH2 on PA31 mux D */
#define MUX_PA31D_PWM1_PWMH2                       _L_(3)       /**< PWM1 signal line function value: PWM1_PWMH2 */
#define PIO_PA31D_PWM1_PWMH2                       _UL_(1 << 31) /**< PWM1 signal: PWM1_PWMH2 */
#define PIN_PD5B_PWM1_PWMH2                        _L_(101)     /**< PWM1 signal: PWM1_PWMH2 on PD5 mux B */
#define MUX_PD5B_PWM1_PWMH2                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWMH2 */
#define PIO_PD5B_PWM1_PWMH2                        _UL_(1 << 5) /**< PWM1 signal: PWM1_PWMH2 */
#define PIN_PA8A_PWM1_PWMH3                        _L_(8)       /**< PWM1 signal: PWM1_PWMH3 on PA8 mux A */
#define MUX_PA8A_PWM1_PWMH3                        _L_(0)       /**< PWM1 signal line function value: PWM1_PWMH3 */
#define PIO_PA8A_PWM1_PWMH3                        _UL_(1 << 8) /**< PWM1 signal: PWM1_PWMH3 */
#define PIN_PD7B_PWM1_PWMH3                        _L_(103)     /**< PWM1 signal: PWM1_PWMH3 on PD7 mux B */
#define MUX_PD7B_PWM1_PWMH3                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWMH3 */
#define PIO_PD7B_PWM1_PWMH3                        _UL_(1 << 7) /**< PWM1 signal: PWM1_PWMH3 */
#define PIN_PA11C_PWM1_PWML0                       _L_(11)      /**< PWM1 signal: PWM1_PWML0 on PA11 mux C */
#define MUX_PA11C_PWM1_PWML0                       _L_(2)       /**< PWM1 signal line function value: PWM1_PWML0 */
#define PIO_PA11C_PWM1_PWML0                       _UL_(1 << 11) /**< PWM1 signal: PWM1_PWML0 */
#define PIN_PD0B_PWM1_PWML0                        _L_(96)      /**< PWM1 signal: PWM1_PWML0 on PD0 mux B */
#define MUX_PD0B_PWM1_PWML0                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWML0 */
#define PIO_PD0B_PWM1_PWML0                        _UL_(1 << 0) /**< PWM1 signal: PWM1_PWML0 */
#define PIN_PA13C_PWM1_PWML1                       _L_(13)      /**< PWM1 signal: PWM1_PWML1 on PA13 mux C */
#define MUX_PA13C_PWM1_PWML1                       _L_(2)       /**< PWM1 signal line function value: PWM1_PWML1 */
#define PIO_PA13C_PWM1_PWML1                       _UL_(1 << 13) /**< PWM1 signal: PWM1_PWML1 */
#define PIN_PD2B_PWM1_PWML1                        _L_(98)      /**< PWM1 signal: PWM1_PWML1 on PD2 mux B */
#define MUX_PD2B_PWM1_PWML1                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWML1 */
#define PIO_PD2B_PWM1_PWML1                        _UL_(1 << 2) /**< PWM1 signal: PWM1_PWML1 */
#define PIN_PA23D_PWM1_PWML2                       _L_(23)      /**< PWM1 signal: PWM1_PWML2 on PA23 mux D */
#define MUX_PA23D_PWM1_PWML2                       _L_(3)       /**< PWM1 signal line function value: PWM1_PWML2 */
#define PIO_PA23D_PWM1_PWML2                       _UL_(1 << 23) /**< PWM1 signal: PWM1_PWML2 */
#define PIN_PD4B_PWM1_PWML2                        _L_(100)     /**< PWM1 signal: PWM1_PWML2 on PD4 mux B */
#define MUX_PD4B_PWM1_PWML2                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWML2 */
#define PIO_PD4B_PWM1_PWML2                        _UL_(1 << 4) /**< PWM1 signal: PWM1_PWML2 */
#define PIN_PA5A_PWM1_PWML3                        _L_(5)       /**< PWM1 signal: PWM1_PWML3 on PA5 mux A */
#define MUX_PA5A_PWM1_PWML3                        _L_(0)       /**< PWM1 signal line function value: PWM1_PWML3 */
#define PIO_PA5A_PWM1_PWML3                        _UL_(1 << 5) /**< PWM1 signal: PWM1_PWML3 */
#define PIN_PD6B_PWM1_PWML3                        _L_(102)     /**< PWM1 signal: PWM1_PWML3 on PD6 mux B */
#define MUX_PD6B_PWM1_PWML3                        _L_(1)       /**< PWM1 signal line function value: PWM1_PWML3 */
#define PIO_PD6B_PWM1_PWML3                        _UL_(1 << 6) /**< PWM1 signal: PWM1_PWML3 */
/* ========== PIO definition for QSPI peripheral ========== */
#define PIN_PA11A_QSPI_QCS                         _L_(11)      /**< QSPI signal: QSPI_QCS on PA11 mux A */
#define MUX_PA11A_QSPI_QCS                         _L_(0)       /**< QSPI signal line function value: QSPI_QCS */
#define PIO_PA11A_QSPI_QCS                         _UL_(1 << 11) /**< QSPI signal: QSPI_QCS */
#define PIN_PA13A_QSPI_QIO0                        _L_(13)      /**< QSPI signal: QSPI_QIO0 on PA13 mux A */
#define MUX_PA13A_QSPI_QIO0                        _L_(0)       /**< QSPI signal line function value: QSPI_QIO0 */
#define PIO_PA13A_QSPI_QIO0                        _UL_(1 << 13) /**< QSPI signal: QSPI_QIO0 */
#define PIN_PA12A_QSPI_QIO1                        _L_(12)      /**< QSPI signal: QSPI_QIO1 on PA12 mux A */
#define MUX_PA12A_QSPI_QIO1                        _L_(0)       /**< QSPI signal line function value: QSPI_QIO1 */
#define PIO_PA12A_QSPI_QIO1                        _UL_(1 << 12) /**< QSPI signal: QSPI_QIO1 */
#define PIN_PA17A_QSPI_QIO2                        _L_(17)      /**< QSPI signal: QSPI_QIO2 on PA17 mux A */
#define MUX_PA17A_QSPI_QIO2                        _L_(0)       /**< QSPI signal line function value: QSPI_QIO2 */
#define PIO_PA17A_QSPI_QIO2                        _UL_(1 << 17) /**< QSPI signal: QSPI_QIO2 */
#define PIN_PD31A_QSPI_QIO3                        _L_(127)     /**< QSPI signal: QSPI_QIO3 on PD31 mux A */
#define MUX_PD31A_QSPI_QIO3                        _L_(0)       /**< QSPI signal line function value: QSPI_QIO3 */
#define PIO_PD31A_QSPI_QIO3                        _UL_(1 << 31) /**< QSPI signal: QSPI_QIO3 */
#define PIN_PA14A_QSPI_QSCK                        _L_(14)      /**< QSPI signal: QSPI_QSCK on PA14 mux A */
#define MUX_PA14A_QSPI_QSCK                        _L_(0)       /**< QSPI signal line function value: QSPI_QSCK */
#define PIO_PA14A_QSPI_QSCK                        _UL_(1 << 14) /**< QSPI signal: QSPI_QSCK */
/* ========== PIO definition for RTC peripheral ========== */
#define PIN_PB0X1_RTC_RTCOUT0                      _L_(32)      /**< RTC signal: RTC_RTCOUT0 on PB0 mux X1 */
#define PIO_PB0X1_RTC_RTCOUT0                      _UL_(1 << 0) /**< RTC signal: RTC_RTCOUT0 */
#define PIN_PB1X1_RTC_RTCOUT1                      _L_(33)      /**< RTC signal: RTC_RTCOUT1 on PB1 mux X1 */
#define PIO_PB1X1_RTC_RTCOUT1                      _UL_(1 << 1) /**< RTC signal: RTC_RTCOUT1 */
/* ========== PIO definition for SPI0 peripheral ========== */
#define PIN_PD20B_SPI0_MISO                        _L_(116)     /**< SPI0 signal: SPI0_MISO on PD20 mux B */
#define MUX_PD20B_SPI0_MISO                        _L_(1)       /**< SPI0 signal line function value: SPI0_MISO */
#define PIO_PD20B_SPI0_MISO                        _UL_(1 << 20) /**< SPI0 signal: SPI0_MISO */
#define PIN_PD21B_SPI0_MOSI                        _L_(117)     /**< SPI0 signal: SPI0_MOSI on PD21 mux B */
#define MUX_PD21B_SPI0_MOSI                        _L_(1)       /**< SPI0 signal line function value: SPI0_MOSI */
#define PIO_PD21B_SPI0_MOSI                        _UL_(1 << 21) /**< SPI0 signal: SPI0_MOSI */
#define PIN_PB2D_SPI0_NPCS0                        _L_(34)      /**< SPI0 signal: SPI0_NPCS0 on PB2 mux D */
#define MUX_PB2D_SPI0_NPCS0                        _L_(3)       /**< SPI0 signal line function value: SPI0_NPCS0 */
#define PIO_PB2D_SPI0_NPCS0                        _UL_(1 << 2) /**< SPI0 signal: SPI0_NPCS0 */
#define PIN_PA31A_SPI0_NPCS1                       _L_(31)      /**< SPI0 signal: SPI0_NPCS1 on PA31 mux A */
#define MUX_PA31A_SPI0_NPCS1                       _L_(0)       /**< SPI0 signal line function value: SPI0_NPCS1 */
#define PIO_PA31A_SPI0_NPCS1                       _UL_(1 << 31) /**< SPI0 signal: SPI0_NPCS1 */
#define PIN_PD25B_SPI0_NPCS1                       _L_(121)     /**< SPI0 signal: SPI0_NPCS1 on PD25 mux B */
#define MUX_PD25B_SPI0_NPCS1                       _L_(1)       /**< SPI0 signal line function value: SPI0_NPCS1 */
#define PIO_PD25B_SPI0_NPCS1                       _UL_(1 << 25) /**< SPI0 signal: SPI0_NPCS1 */
#define PIN_PD12C_SPI0_NPCS2                       _L_(108)     /**< SPI0 signal: SPI0_NPCS2 on PD12 mux C */
#define MUX_PD12C_SPI0_NPCS2                       _L_(2)       /**< SPI0 signal line function value: SPI0_NPCS2 */
#define PIO_PD12C_SPI0_NPCS2                       _UL_(1 << 12) /**< SPI0 signal: SPI0_NPCS2 */
#define PIN_PD27B_SPI0_NPCS3                       _L_(123)     /**< SPI0 signal: SPI0_NPCS3 on PD27 mux B */
#define MUX_PD27B_SPI0_NPCS3                       _L_(1)       /**< SPI0 signal line function value: SPI0_NPCS3 */
#define PIO_PD27B_SPI0_NPCS3                       _UL_(1 << 27) /**< SPI0 signal: SPI0_NPCS3 */
#define PIN_PD22B_SPI0_SPCK                        _L_(118)     /**< SPI0 signal: SPI0_SPCK on PD22 mux B */
#define MUX_PD22B_SPI0_SPCK                        _L_(1)       /**< SPI0 signal line function value: SPI0_SPCK */
#define PIO_PD22B_SPI0_SPCK                        _UL_(1 << 22) /**< SPI0 signal: SPI0_SPCK */
/* ========== PIO definition for SPI1 peripheral ========== */
#define PIN_PC26C_SPI1_MISO                        _L_(90)      /**< SPI1 signal: SPI1_MISO on PC26 mux C */
#define MUX_PC26C_SPI1_MISO                        _L_(2)       /**< SPI1 signal line function value: SPI1_MISO */
#define PIO_PC26C_SPI1_MISO                        _UL_(1 << 26) /**< SPI1 signal: SPI1_MISO */
#define PIN_PC27C_SPI1_MOSI                        _L_(91)      /**< SPI1 signal: SPI1_MOSI on PC27 mux C */
#define MUX_PC27C_SPI1_MOSI                        _L_(2)       /**< SPI1 signal line function value: SPI1_MOSI */
#define PIO_PC27C_SPI1_MOSI                        _UL_(1 << 27) /**< SPI1 signal: SPI1_MOSI */
#define PIN_PC25C_SPI1_NPCS0                       _L_(89)      /**< SPI1 signal: SPI1_NPCS0 on PC25 mux C */
#define MUX_PC25C_SPI1_NPCS0                       _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS0 */
#define PIO_PC25C_SPI1_NPCS0                       _UL_(1 << 25) /**< SPI1 signal: SPI1_NPCS0 */
#define PIN_PC28C_SPI1_NPCS1                       _L_(92)      /**< SPI1 signal: SPI1_NPCS1 on PC28 mux C */
#define MUX_PC28C_SPI1_NPCS1                       _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS1 */
#define PIO_PC28C_SPI1_NPCS1                       _UL_(1 << 28) /**< SPI1 signal: SPI1_NPCS1 */
#define PIN_PD0C_SPI1_NPCS1                        _L_(96)      /**< SPI1 signal: SPI1_NPCS1 on PD0 mux C */
#define MUX_PD0C_SPI1_NPCS1                        _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS1 */
#define PIO_PD0C_SPI1_NPCS1                        _UL_(1 << 0) /**< SPI1 signal: SPI1_NPCS1 */
#define PIN_PC29C_SPI1_NPCS2                       _L_(93)      /**< SPI1 signal: SPI1_NPCS2 on PC29 mux C */
#define MUX_PC29C_SPI1_NPCS2                       _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS2 */
#define PIO_PC29C_SPI1_NPCS2                       _UL_(1 << 29) /**< SPI1 signal: SPI1_NPCS2 */
#define PIN_PD1C_SPI1_NPCS2                        _L_(97)      /**< SPI1 signal: SPI1_NPCS2 on PD1 mux C */
#define MUX_PD1C_SPI1_NPCS2                        _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS2 */
#define PIO_PD1C_SPI1_NPCS2                        _UL_(1 << 1) /**< SPI1 signal: SPI1_NPCS2 */
#define PIN_PC30C_SPI1_NPCS3                       _L_(94)      /**< SPI1 signal: SPI1_NPCS3 on PC30 mux C */
#define MUX_PC30C_SPI1_NPCS3                       _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS3 */
#define PIO_PC30C_SPI1_NPCS3                       _UL_(1 << 30) /**< SPI1 signal: SPI1_NPCS3 */
#define PIN_PD2C_SPI1_NPCS3                        _L_(98)      /**< SPI1 signal: SPI1_NPCS3 on PD2 mux C */
#define MUX_PD2C_SPI1_NPCS3                        _L_(2)       /**< SPI1 signal line function value: SPI1_NPCS3 */
#define PIO_PD2C_SPI1_NPCS3                        _UL_(1 << 2) /**< SPI1 signal: SPI1_NPCS3 */
#define PIN_PC24C_SPI1_SPCK                        _L_(88)      /**< SPI1 signal: SPI1_SPCK on PC24 mux C */
#define MUX_PC24C_SPI1_SPCK                        _L_(2)       /**< SPI1 signal line function value: SPI1_SPCK */
#define PIO_PC24C_SPI1_SPCK                        _UL_(1 << 24) /**< SPI1 signal: SPI1_SPCK */
/* ========== PIO definition for SSC peripheral ========== */
#define PIN_PA10C_SSC_RD                           _L_(10)      /**< SSC signal: SSC_RD on PA10 mux C */
#define MUX_PA10C_SSC_RD                           _L_(2)       /**< SSC signal line function value: SSC_RD */
#define PIO_PA10C_SSC_RD                           _UL_(1 << 10) /**< SSC signal: SSC_RD */
#define PIN_PD24B_SSC_RF                           _L_(120)     /**< SSC signal: SSC_RF on PD24 mux B */
#define MUX_PD24B_SSC_RF                           _L_(1)       /**< SSC signal line function value: SSC_RF */
#define PIO_PD24B_SSC_RF                           _UL_(1 << 24) /**< SSC signal: SSC_RF */
#define PIN_PA22A_SSC_RK                           _L_(22)      /**< SSC signal: SSC_RK on PA22 mux A */
#define MUX_PA22A_SSC_RK                           _L_(0)       /**< SSC signal line function value: SSC_RK */
#define PIO_PA22A_SSC_RK                           _UL_(1 << 22) /**< SSC signal: SSC_RK */
#define PIN_PB5D_SSC_TD                            _L_(37)      /**< SSC signal: SSC_TD on PB5 mux D */
#define MUX_PB5D_SSC_TD                            _L_(3)       /**< SSC signal line function value: SSC_TD */
#define PIO_PB5D_SSC_TD                            _UL_(1 << 5) /**< SSC signal: SSC_TD */
#define PIN_PD10C_SSC_TD                           _L_(106)     /**< SSC signal: SSC_TD on PD10 mux C */
#define MUX_PD10C_SSC_TD                           _L_(2)       /**< SSC signal line function value: SSC_TD */
#define PIO_PD10C_SSC_TD                           _UL_(1 << 10) /**< SSC signal: SSC_TD */
#define PIN_PD26B_SSC_TD                           _L_(122)     /**< SSC signal: SSC_TD on PD26 mux B */
#define MUX_PD26B_SSC_TD                           _L_(1)       /**< SSC signal line function value: SSC_TD */
#define PIO_PD26B_SSC_TD                           _UL_(1 << 26) /**< SSC signal: SSC_TD */
#define PIN_PB0D_SSC_TF                            _L_(32)      /**< SSC signal: SSC_TF on PB0 mux D */
#define MUX_PB0D_SSC_TF                            _L_(3)       /**< SSC signal line function value: SSC_TF */
#define PIO_PB0D_SSC_TF                            _UL_(1 << 0) /**< SSC signal: SSC_TF */
#define PIN_PB1D_SSC_TK                            _L_(33)      /**< SSC signal: SSC_TK on PB1 mux D */
#define MUX_PB1D_SSC_TK                            _L_(3)       /**< SSC signal line function value: SSC_TK */
#define PIO_PB1D_SSC_TK                            _UL_(1 << 1) /**< SSC signal: SSC_TK */
/* ========== PIO definition for SUPC peripheral ========== */
#define PIN_PA0X1_SUPC_WKUP0                       _L_(0)       /**< SUPC signal: SUPC_WKUP0 on PA0 mux X1 */
#define PIO_PA0X1_SUPC_WKUP0                       _UL_(1 << 0) /**< SUPC signal: SUPC_WKUP0 */
#define PIN_PA1X1_SUPC_WKUP1                       _L_(1)       /**< SUPC signal: SUPC_WKUP1 on PA1 mux X1 */
#define PIO_PA1X1_SUPC_WKUP1                       _UL_(1 << 1) /**< SUPC signal: SUPC_WKUP1 */
#define PIN_PA2X1_SUPC_WKUP2                       _L_(2)       /**< SUPC signal: SUPC_WKUP2 on PA2 mux X1 */
#define PIO_PA2X1_SUPC_WKUP2                       _UL_(1 << 2) /**< SUPC signal: SUPC_WKUP2 */
#define PIN_PA4X1_SUPC_WKUP3                       _L_(4)       /**< SUPC signal: SUPC_WKUP3 on PA4 mux X1 */
#define PIO_PA4X1_SUPC_WKUP3                       _UL_(1 << 4) /**< SUPC signal: SUPC_WKUP3 */
#define PIN_PA5X1_SUPC_WKUP4                       _L_(5)       /**< SUPC signal: SUPC_WKUP4 on PA5 mux X1 */
#define PIO_PA5X1_SUPC_WKUP4                       _UL_(1 << 5) /**< SUPC signal: SUPC_WKUP4 */
#define PIN_PA9X1_SUPC_WKUP6                       _L_(9)       /**< SUPC signal: SUPC_WKUP6 on PA9 mux X1 */
#define PIO_PA9X1_SUPC_WKUP6                       _UL_(1 << 9) /**< SUPC signal: SUPC_WKUP6 */
#define PIN_PA11X1_SUPC_WKUP7                      _L_(11)      /**< SUPC signal: SUPC_WKUP7 on PA11 mux X1 */
#define PIO_PA11X1_SUPC_WKUP7                      _UL_(1 << 11) /**< SUPC signal: SUPC_WKUP7 */
#define PIN_PA14X1_SUPC_WKUP8                      _L_(14)      /**< SUPC signal: SUPC_WKUP8 on PA14 mux X1 */
#define PIO_PA14X1_SUPC_WKUP8                      _UL_(1 << 14) /**< SUPC signal: SUPC_WKUP8 */
#define PIN_PA19X1_SUPC_WKUP9                      _L_(19)      /**< SUPC signal: SUPC_WKUP9 on PA19 mux X1 */
#define PIO_PA19X1_SUPC_WKUP9                      _UL_(1 << 19) /**< SUPC signal: SUPC_WKUP9 */
#define PIN_PA20X1_SUPC_WKUP10                     _L_(20)      /**< SUPC signal: SUPC_WKUP10 on PA20 mux X1 */
#define PIO_PA20X1_SUPC_WKUP10                     _UL_(1 << 20) /**< SUPC signal: SUPC_WKUP10 */
#define PIN_PA30X1_SUPC_WKUP11                     _L_(30)      /**< SUPC signal: SUPC_WKUP11 on PA30 mux X1 */
#define PIO_PA30X1_SUPC_WKUP11                     _UL_(1 << 30) /**< SUPC signal: SUPC_WKUP11 */
#define PIN_PB3X1_SUPC_WKUP12                      _L_(35)      /**< SUPC signal: SUPC_WKUP12 on PB3 mux X1 */
#define PIO_PB3X1_SUPC_WKUP12                      _UL_(1 << 3) /**< SUPC signal: SUPC_WKUP12 */
#define PIN_PB5X1_SUPC_WKUP13                      _L_(37)      /**< SUPC signal: SUPC_WKUP13 on PB5 mux X1 */
#define PIO_PB5X1_SUPC_WKUP13                      _UL_(1 << 5) /**< SUPC signal: SUPC_WKUP13 */
#define PIN_PD28X1_SUPC_WKUP5                      _L_(124)     /**< SUPC signal: SUPC_WKUP5 on PD28 mux X1 */
#define PIO_PD28X1_SUPC_WKUP5                      _UL_(1 << 28) /**< SUPC signal: SUPC_WKUP5 */
/* ========== PIO definition for TC0 peripheral ========== */
#define PIN_PA4B_TC0_TCLK0                         _L_(4)       /**< TC0 signal: TC0_TCLK0 on PA4 mux B */
#define MUX_PA4B_TC0_TCLK0                         _L_(1)       /**< TC0 signal line function value: TC0_TCLK0 */
#define PIO_PA4B_TC0_TCLK0                         _UL_(1 << 4) /**< TC0 signal: TC0_TCLK0 */
#define PIN_PA28B_TC0_TCLK1                        _L_(28)      /**< TC0 signal: TC0_TCLK1 on PA28 mux B */
#define MUX_PA28B_TC0_TCLK1                        _L_(1)       /**< TC0 signal line function value: TC0_TCLK1 */
#define PIO_PA28B_TC0_TCLK1                        _UL_(1 << 28) /**< TC0 signal: TC0_TCLK1 */
#define PIN_PA29B_TC0_TCLK2                        _L_(29)      /**< TC0 signal: TC0_TCLK2 on PA29 mux B */
#define MUX_PA29B_TC0_TCLK2                        _L_(1)       /**< TC0 signal line function value: TC0_TCLK2 */
#define PIO_PA29B_TC0_TCLK2                        _UL_(1 << 29) /**< TC0 signal: TC0_TCLK2 */
#define PIN_PA0B_TC0_TIOA0                         _L_(0)       /**< TC0 signal: TC0_TIOA0 on PA0 mux B */
#define MUX_PA0B_TC0_TIOA0                         _L_(1)       /**< TC0 signal line function value: TC0_TIOA0 */
#define PIO_PA0B_TC0_TIOA0                         _UL_(1 << 0) /**< TC0 signal: TC0_TIOA0 */
#define PIN_PA15B_TC0_TIOA1                        _L_(15)      /**< TC0 signal: TC0_TIOA1 on PA15 mux B */
#define MUX_PA15B_TC0_TIOA1                        _L_(1)       /**< TC0 signal line function value: TC0_TIOA1 */
#define PIO_PA15B_TC0_TIOA1                        _UL_(1 << 15) /**< TC0 signal: TC0_TIOA1 */
#define PIN_PA26B_TC0_TIOA2                        _L_(26)      /**< TC0 signal: TC0_TIOA2 on PA26 mux B */
#define MUX_PA26B_TC0_TIOA2                        _L_(1)       /**< TC0 signal line function value: TC0_TIOA2 */
#define PIO_PA26B_TC0_TIOA2                        _UL_(1 << 26) /**< TC0 signal: TC0_TIOA2 */
#define PIN_PA1B_TC0_TIOB0                         _L_(1)       /**< TC0 signal: TC0_TIOB0 on PA1 mux B */
#define MUX_PA1B_TC0_TIOB0                         _L_(1)       /**< TC0 signal line function value: TC0_TIOB0 */
#define PIO_PA1B_TC0_TIOB0                         _UL_(1 << 1) /**< TC0 signal: TC0_TIOB0 */
#define PIN_PA16B_TC0_TIOB1                        _L_(16)      /**< TC0 signal: TC0_TIOB1 on PA16 mux B */
#define MUX_PA16B_TC0_TIOB1                        _L_(1)       /**< TC0 signal line function value: TC0_TIOB1 */
#define PIO_PA16B_TC0_TIOB1                        _UL_(1 << 16) /**< TC0 signal: TC0_TIOB1 */
#define PIN_PA27B_TC0_TIOB2                        _L_(27)      /**< TC0 signal: TC0_TIOB2 on PA27 mux B */
#define MUX_PA27B_TC0_TIOB2                        _L_(1)       /**< TC0 signal line function value: TC0_TIOB2 */
#define PIO_PA27B_TC0_TIOB2                        _UL_(1 << 27) /**< TC0 signal: TC0_TIOB2 */
/* ========== PIO definition for TC1 peripheral ========== */
#define PIN_PC25B_TC1_TCLK3                        _L_(89)      /**< TC1 signal: TC1_TCLK3 on PC25 mux B */
#define MUX_PC25B_TC1_TCLK3                        _L_(1)       /**< TC1 signal line function value: TC1_TCLK3 */
#define PIO_PC25B_TC1_TCLK3                        _UL_(1 << 25) /**< TC1 signal: TC1_TCLK3 */
#define PIN_PC28B_TC1_TCLK4                        _L_(92)      /**< TC1 signal: TC1_TCLK4 on PC28 mux B */
#define MUX_PC28B_TC1_TCLK4                        _L_(1)       /**< TC1 signal line function value: TC1_TCLK4 */
#define PIO_PC28B_TC1_TCLK4                        _UL_(1 << 28) /**< TC1 signal: TC1_TCLK4 */
#define PIN_PC31B_TC1_TCLK5                        _L_(95)      /**< TC1 signal: TC1_TCLK5 on PC31 mux B */
#define MUX_PC31B_TC1_TCLK5                        _L_(1)       /**< TC1 signal line function value: TC1_TCLK5 */
#define PIO_PC31B_TC1_TCLK5                        _UL_(1 << 31) /**< TC1 signal: TC1_TCLK5 */
#define PIN_PC23B_TC1_TIOA3                        _L_(87)      /**< TC1 signal: TC1_TIOA3 on PC23 mux B */
#define MUX_PC23B_TC1_TIOA3                        _L_(1)       /**< TC1 signal line function value: TC1_TIOA3 */
#define PIO_PC23B_TC1_TIOA3                        _UL_(1 << 23) /**< TC1 signal: TC1_TIOA3 */
#define PIN_PC26B_TC1_TIOA4                        _L_(90)      /**< TC1 signal: TC1_TIOA4 on PC26 mux B */
#define MUX_PC26B_TC1_TIOA4                        _L_(1)       /**< TC1 signal line function value: TC1_TIOA4 */
#define PIO_PC26B_TC1_TIOA4                        _UL_(1 << 26) /**< TC1 signal: TC1_TIOA4 */
#define PIN_PC29B_TC1_TIOA5                        _L_(93)      /**< TC1 signal: TC1_TIOA5 on PC29 mux B */
#define MUX_PC29B_TC1_TIOA5                        _L_(1)       /**< TC1 signal line function value: TC1_TIOA5 */
#define PIO_PC29B_TC1_TIOA5                        _UL_(1 << 29) /**< TC1 signal: TC1_TIOA5 */
#define PIN_PC24B_TC1_TIOB3                        _L_(88)      /**< TC1 signal: TC1_TIOB3 on PC24 mux B */
#define MUX_PC24B_TC1_TIOB3                        _L_(1)       /**< TC1 signal line function value: TC1_TIOB3 */
#define PIO_PC24B_TC1_TIOB3                        _UL_(1 << 24) /**< TC1 signal: TC1_TIOB3 */
#define PIN_PC27B_TC1_TIOB4                        _L_(91)      /**< TC1 signal: TC1_TIOB4 on PC27 mux B */
#define MUX_PC27B_TC1_TIOB4                        _L_(1)       /**< TC1 signal line function value: TC1_TIOB4 */
#define PIO_PC27B_TC1_TIOB4                        _UL_(1 << 27) /**< TC1 signal: TC1_TIOB4 */
#define PIN_PC30B_TC1_TIOB5                        _L_(94)      /**< TC1 signal: TC1_TIOB5 on PC30 mux B */
#define MUX_PC30B_TC1_TIOB5                        _L_(1)       /**< TC1 signal line function value: TC1_TIOB5 */
#define PIO_PC30B_TC1_TIOB5                        _UL_(1 << 30) /**< TC1 signal: TC1_TIOB5 */
/* ========== PIO definition for TC2 peripheral ========== */
#define PIN_PC7B_TC2_TCLK6                         _L_(71)      /**< TC2 signal: TC2_TCLK6 on PC7 mux B */
#define MUX_PC7B_TC2_TCLK6                         _L_(1)       /**< TC2 signal line function value: TC2_TCLK6 */
#define PIO_PC7B_TC2_TCLK6                         _UL_(1 << 7) /**< TC2 signal: TC2_TCLK6 */
#define PIN_PC10B_TC2_TCLK7                        _L_(74)      /**< TC2 signal: TC2_TCLK7 on PC10 mux B */
#define MUX_PC10B_TC2_TCLK7                        _L_(1)       /**< TC2 signal line function value: TC2_TCLK7 */
#define PIO_PC10B_TC2_TCLK7                        _UL_(1 << 10) /**< TC2 signal: TC2_TCLK7 */
#define PIN_PC14B_TC2_TCLK8                        _L_(78)      /**< TC2 signal: TC2_TCLK8 on PC14 mux B */
#define MUX_PC14B_TC2_TCLK8                        _L_(1)       /**< TC2 signal line function value: TC2_TCLK8 */
#define PIO_PC14B_TC2_TCLK8                        _UL_(1 << 14) /**< TC2 signal: TC2_TCLK8 */
#define PIN_PC5B_TC2_TIOA6                         _L_(69)      /**< TC2 signal: TC2_TIOA6 on PC5 mux B */
#define MUX_PC5B_TC2_TIOA6                         _L_(1)       /**< TC2 signal line function value: TC2_TIOA6 */
#define PIO_PC5B_TC2_TIOA6                         _UL_(1 << 5) /**< TC2 signal: TC2_TIOA6 */
#define PIN_PC8B_TC2_TIOA7                         _L_(72)      /**< TC2 signal: TC2_TIOA7 on PC8 mux B */
#define MUX_PC8B_TC2_TIOA7                         _L_(1)       /**< TC2 signal line function value: TC2_TIOA7 */
#define PIO_PC8B_TC2_TIOA7                         _UL_(1 << 8) /**< TC2 signal: TC2_TIOA7 */
#define PIN_PC11B_TC2_TIOA8                        _L_(75)      /**< TC2 signal: TC2_TIOA8 on PC11 mux B */
#define MUX_PC11B_TC2_TIOA8                        _L_(1)       /**< TC2 signal line function value: TC2_TIOA8 */
#define PIO_PC11B_TC2_TIOA8                        _UL_(1 << 11) /**< TC2 signal: TC2_TIOA8 */
#define PIN_PC6B_TC2_TIOB6                         _L_(70)      /**< TC2 signal: TC2_TIOB6 on PC6 mux B */
#define MUX_PC6B_TC2_TIOB6                         _L_(1)       /**< TC2 signal line function value: TC2_TIOB6 */
#define PIO_PC6B_TC2_TIOB6                         _UL_(1 << 6) /**< TC2 signal: TC2_TIOB6 */
#define PIN_PC9B_TC2_TIOB7                         _L_(73)      /**< TC2 signal: TC2_TIOB7 on PC9 mux B */
#define MUX_PC9B_TC2_TIOB7                         _L_(1)       /**< TC2 signal line function value: TC2_TIOB7 */
#define PIO_PC9B_TC2_TIOB7                         _UL_(1 << 9) /**< TC2 signal: TC2_TIOB7 */
#define PIN_PC12B_TC2_TIOB8                        _L_(76)      /**< TC2 signal: TC2_TIOB8 on PC12 mux B */
#define MUX_PC12B_TC2_TIOB8                        _L_(1)       /**< TC2 signal line function value: TC2_TIOB8 */
#define PIO_PC12B_TC2_TIOB8                        _UL_(1 << 12) /**< TC2 signal: TC2_TIOB8 */
/* ========== PIO definition for TC3 peripheral ========== */
#define PIN_PE2B_TC3_TCLK9                         _L_(130)     /**< TC3 signal: TC3_TCLK9 on PE2 mux B */
#define MUX_PE2B_TC3_TCLK9                         _L_(1)       /**< TC3 signal line function value: TC3_TCLK9 */
#define PIO_PE2B_TC3_TCLK9                         _UL_(1 << 2) /**< TC3 signal: TC3_TCLK9 */
#define PIN_PE5B_TC3_TCLK10                        _L_(133)     /**< TC3 signal: TC3_TCLK10 on PE5 mux B */
#define MUX_PE5B_TC3_TCLK10                        _L_(1)       /**< TC3 signal line function value: TC3_TCLK10 */
#define PIO_PE5B_TC3_TCLK10                        _UL_(1 << 5) /**< TC3 signal: TC3_TCLK10 */
#define PIN_PD24C_TC3_TCLK11                       _L_(120)     /**< TC3 signal: TC3_TCLK11 on PD24 mux C */
#define MUX_PD24C_TC3_TCLK11                       _L_(2)       /**< TC3 signal line function value: TC3_TCLK11 */
#define PIO_PD24C_TC3_TCLK11                       _UL_(1 << 24) /**< TC3 signal: TC3_TCLK11 */
#define PIN_PE0B_TC3_TIOA9                         _L_(128)     /**< TC3 signal: TC3_TIOA9 on PE0 mux B */
#define MUX_PE0B_TC3_TIOA9                         _L_(1)       /**< TC3 signal line function value: TC3_TIOA9 */
#define PIO_PE0B_TC3_TIOA9                         _UL_(1 << 0) /**< TC3 signal: TC3_TIOA9 */
#define PIN_PE3B_TC3_TIOA10                        _L_(131)     /**< TC3 signal: TC3_TIOA10 on PE3 mux B */
#define MUX_PE3B_TC3_TIOA10                        _L_(1)       /**< TC3 signal line function value: TC3_TIOA10 */
#define PIO_PE3B_TC3_TIOA10                        _UL_(1 << 3) /**< TC3 signal: TC3_TIOA10 */
#define PIN_PD21C_TC3_TIOA11                       _L_(117)     /**< TC3 signal: TC3_TIOA11 on PD21 mux C */
#define MUX_PD21C_TC3_TIOA11                       _L_(2)       /**< TC3 signal line function value: TC3_TIOA11 */
#define PIO_PD21C_TC3_TIOA11                       _UL_(1 << 21) /**< TC3 signal: TC3_TIOA11 */
#define PIN_PE1B_TC3_TIOB9                         _L_(129)     /**< TC3 signal: TC3_TIOB9 on PE1 mux B */
#define MUX_PE1B_TC3_TIOB9                         _L_(1)       /**< TC3 signal line function value: TC3_TIOB9 */
#define PIO_PE1B_TC3_TIOB9                         _UL_(1 << 1) /**< TC3 signal: TC3_TIOB9 */
#define PIN_PE4B_TC3_TIOB10                        _L_(132)     /**< TC3 signal: TC3_TIOB10 on PE4 mux B */
#define MUX_PE4B_TC3_TIOB10                        _L_(1)       /**< TC3 signal line function value: TC3_TIOB10 */
#define PIO_PE4B_TC3_TIOB10                        _UL_(1 << 4) /**< TC3 signal: TC3_TIOB10 */
#define PIN_PD22C_TC3_TIOB11                       _L_(118)     /**< TC3 signal: TC3_TIOB11 on PD22 mux C */
#define MUX_PD22C_TC3_TIOB11                       _L_(2)       /**< TC3 signal line function value: TC3_TIOB11 */
#define PIO_PD22C_TC3_TIOB11                       _UL_(1 << 22) /**< TC3 signal: TC3_TIOB11 */
/* ========== PIO definition for TWIHS0 peripheral ========== */
#define PIN_PA4A_TWIHS0_TWCK0                      _L_(4)       /**< TWIHS0 signal: TWIHS0_TWCK0 on PA4 mux A */
#define MUX_PA4A_TWIHS0_TWCK0                      _L_(0)       /**< TWIHS0 signal line function value: TWIHS0_TWCK0 */
#define PIO_PA4A_TWIHS0_TWCK0                      _UL_(1 << 4) /**< TWIHS0 signal: TWIHS0_TWCK0 */
#define PIN_PA3A_TWIHS0_TWD0                       _L_(3)       /**< TWIHS0 signal: TWIHS0_TWD0 on PA3 mux A */
#define MUX_PA3A_TWIHS0_TWD0                       _L_(0)       /**< TWIHS0 signal line function value: TWIHS0_TWD0 */
#define PIO_PA3A_TWIHS0_TWD0                       _UL_(1 << 3) /**< TWIHS0 signal: TWIHS0_TWD0 */
/* ========== PIO definition for TWIHS1 peripheral ========== */
#define PIN_PB5A_TWIHS1_TWCK1                      _L_(37)      /**< TWIHS1 signal: TWIHS1_TWCK1 on PB5 mux A */
#define MUX_PB5A_TWIHS1_TWCK1                      _L_(0)       /**< TWIHS1 signal line function value: TWIHS1_TWCK1 */
#define PIO_PB5A_TWIHS1_TWCK1                      _UL_(1 << 5) /**< TWIHS1 signal: TWIHS1_TWCK1 */
#define PIN_PB4A_TWIHS1_TWD1                       _L_(36)      /**< TWIHS1 signal: TWIHS1_TWD1 on PB4 mux A */
#define MUX_PB4A_TWIHS1_TWD1                       _L_(0)       /**< TWIHS1 signal line function value: TWIHS1_TWD1 */
#define PIO_PB4A_TWIHS1_TWD1                       _UL_(1 << 4) /**< TWIHS1 signal: TWIHS1_TWD1 */
/* ========== PIO definition for TWIHS2 peripheral ========== */
#define PIN_PD28C_TWIHS2_TWCK2                     _L_(124)     /**< TWIHS2 signal: TWIHS2_TWCK2 on PD28 mux C */
#define MUX_PD28C_TWIHS2_TWCK2                     _L_(2)       /**< TWIHS2 signal line function value: TWIHS2_TWCK2 */
#define PIO_PD28C_TWIHS2_TWCK2                     _UL_(1 << 28) /**< TWIHS2 signal: TWIHS2_TWCK2 */
#define PIN_PD27C_TWIHS2_TWD2                      _L_(123)     /**< TWIHS2 signal: TWIHS2_TWD2 on PD27 mux C */
#define MUX_PD27C_TWIHS2_TWD2                      _L_(2)       /**< TWIHS2 signal line function value: TWIHS2_TWD2 */
#define PIO_PD27C_TWIHS2_TWD2                      _UL_(1 << 27) /**< TWIHS2 signal: TWIHS2_TWD2 */
/* ========== PIO definition for UART0 peripheral ========== */
#define PIN_PA9A_UART0_URXD0                       _L_(9)       /**< UART0 signal: UART0_URXD0 on PA9 mux A */
#define MUX_PA9A_UART0_URXD0                       _L_(0)       /**< UART0 signal line function value: UART0_URXD0 */
#define PIO_PA9A_UART0_URXD0                       _UL_(1 << 9) /**< UART0 signal: UART0_URXD0 */
#define PIN_PA10A_UART0_UTXD0                      _L_(10)      /**< UART0 signal: UART0_UTXD0 on PA10 mux A */
#define MUX_PA10A_UART0_UTXD0                      _L_(0)       /**< UART0 signal line function value: UART0_UTXD0 */
#define PIO_PA10A_UART0_UTXD0                      _UL_(1 << 10) /**< UART0 signal: UART0_UTXD0 */
/* ========== PIO definition for UART1 peripheral ========== */
#define PIN_PA5C_UART1_URXD1                       _L_(5)       /**< UART1 signal: UART1_URXD1 on PA5 mux C */
#define MUX_PA5C_UART1_URXD1                       _L_(2)       /**< UART1 signal line function value: UART1_URXD1 */
#define PIO_PA5C_UART1_URXD1                       _UL_(1 << 5) /**< UART1 signal: UART1_URXD1 */
#define PIN_PA4C_UART1_UTXD1                       _L_(4)       /**< UART1 signal: UART1_UTXD1 on PA4 mux C */
#define MUX_PA4C_UART1_UTXD1                       _L_(2)       /**< UART1 signal line function value: UART1_UTXD1 */
#define PIO_PA4C_UART1_UTXD1                       _UL_(1 << 4) /**< UART1 signal: UART1_UTXD1 */
#define PIN_PA6C_UART1_UTXD1                       _L_(6)       /**< UART1 signal: UART1_UTXD1 on PA6 mux C */
#define MUX_PA6C_UART1_UTXD1                       _L_(2)       /**< UART1 signal line function value: UART1_UTXD1 */
#define PIO_PA6C_UART1_UTXD1                       _UL_(1 << 6) /**< UART1 signal: UART1_UTXD1 */
#define PIN_PD26D_UART1_UTXD1                      _L_(122)     /**< UART1 signal: UART1_UTXD1 on PD26 mux D */
#define MUX_PD26D_UART1_UTXD1                      _L_(3)       /**< UART1 signal line function value: UART1_UTXD1 */
#define PIO_PD26D_UART1_UTXD1                      _UL_(1 << 26) /**< UART1 signal: UART1_UTXD1 */
/* ========== PIO definition for UART2 peripheral ========== */
#define PIN_PD25C_UART2_URXD2                      _L_(121)     /**< UART2 signal: UART2_URXD2 on PD25 mux C */
#define MUX_PD25C_UART2_URXD2                      _L_(2)       /**< UART2 signal line function value: UART2_URXD2 */
#define PIO_PD25C_UART2_URXD2                      _UL_(1 << 25) /**< UART2 signal: UART2_URXD2 */
#define PIN_PD26C_UART2_UTXD2                      _L_(122)     /**< UART2 signal: UART2_UTXD2 on PD26 mux C */
#define MUX_PD26C_UART2_UTXD2                      _L_(2)       /**< UART2 signal line function value: UART2_UTXD2 */
#define PIO_PD26C_UART2_UTXD2                      _UL_(1 << 26) /**< UART2 signal: UART2_UTXD2 */
/* ========== PIO definition for UART3 peripheral ========== */
#define PIN_PD28A_UART3_URXD3                      _L_(124)     /**< UART3 signal: UART3_URXD3 on PD28 mux A */
#define MUX_PD28A_UART3_URXD3                      _L_(0)       /**< UART3 signal line function value: UART3_URXD3 */
#define PIO_PD28A_UART3_URXD3                      _UL_(1 << 28) /**< UART3 signal: UART3_URXD3 */
#define PIN_PD30A_UART3_UTXD3                      _L_(126)     /**< UART3 signal: UART3_UTXD3 on PD30 mux A */
#define MUX_PD30A_UART3_UTXD3                      _L_(0)       /**< UART3 signal line function value: UART3_UTXD3 */
#define PIO_PD30A_UART3_UTXD3                      _UL_(1 << 30) /**< UART3 signal: UART3_UTXD3 */
#define PIN_PD31B_UART3_UTXD3                      _L_(127)     /**< UART3 signal: UART3_UTXD3 on PD31 mux B */
#define MUX_PD31B_UART3_UTXD3                      _L_(1)       /**< UART3 signal line function value: UART3_UTXD3 */
#define PIO_PD31B_UART3_UTXD3                      _UL_(1 << 31) /**< UART3 signal: UART3_UTXD3 */
/* ========== PIO definition for UART4 peripheral ========== */
#define PIN_PD18C_UART4_URXD4                      _L_(114)     /**< UART4 signal: UART4_URXD4 on PD18 mux C */
#define MUX_PD18C_UART4_URXD4                      _L_(2)       /**< UART4 signal line function value: UART4_URXD4 */
#define PIO_PD18C_UART4_URXD4                      _UL_(1 << 18) /**< UART4 signal: UART4_URXD4 */
#define PIN_PD3C_UART4_UTXD4                       _L_(99)      /**< UART4 signal: UART4_UTXD4 on PD3 mux C */
#define MUX_PD3C_UART4_UTXD4                       _L_(2)       /**< UART4 signal line function value: UART4_UTXD4 */
#define PIO_PD3C_UART4_UTXD4                       _UL_(1 << 3) /**< UART4 signal: UART4_UTXD4 */
#define PIN_PD19C_UART4_UTXD4                      _L_(115)     /**< UART4 signal: UART4_UTXD4 on PD19 mux C */
#define MUX_PD19C_UART4_UTXD4                      _L_(2)       /**< UART4 signal line function value: UART4_UTXD4 */
#define PIO_PD19C_UART4_UTXD4                      _UL_(1 << 19) /**< UART4 signal: UART4_UTXD4 */
/* ========== PIO definition for USART0 peripheral ========== */
#define PIN_PB2C_USART0_CTS0                       _L_(34)      /**< USART0 signal: USART0_CTS0 on PB2 mux C */
#define MUX_PB2C_USART0_CTS0                       _L_(2)       /**< USART0 signal line function value: USART0_CTS0 */
#define PIO_PB2C_USART0_CTS0                       _UL_(1 << 2) /**< USART0 signal: USART0_CTS0 */
#define PIN_PD0D_USART0_DCD0                       _L_(96)      /**< USART0 signal: USART0_DCD0 on PD0 mux D */
#define MUX_PD0D_USART0_DCD0                       _L_(3)       /**< USART0 signal line function value: USART0_DCD0 */
#define PIO_PD0D_USART0_DCD0                       _UL_(1 << 0) /**< USART0 signal: USART0_DCD0 */
#define PIN_PD2D_USART0_DSR0                       _L_(98)      /**< USART0 signal: USART0_DSR0 on PD2 mux D */
#define MUX_PD2D_USART0_DSR0                       _L_(3)       /**< USART0 signal line function value: USART0_DSR0 */
#define PIO_PD2D_USART0_DSR0                       _UL_(1 << 2) /**< USART0 signal: USART0_DSR0 */
#define PIN_PD1D_USART0_DTR0                       _L_(97)      /**< USART0 signal: USART0_DTR0 on PD1 mux D */
#define MUX_PD1D_USART0_DTR0                       _L_(3)       /**< USART0 signal line function value: USART0_DTR0 */
#define PIO_PD1D_USART0_DTR0                       _UL_(1 << 1) /**< USART0 signal: USART0_DTR0 */
#define PIN_PD3D_USART0_RI0                        _L_(99)      /**< USART0 signal: USART0_RI0 on PD3 mux D */
#define MUX_PD3D_USART0_RI0                        _L_(3)       /**< USART0 signal line function value: USART0_RI0 */
#define PIO_PD3D_USART0_RI0                        _UL_(1 << 3) /**< USART0 signal: USART0_RI0 */
#define PIN_PB3C_USART0_RTS0                       _L_(35)      /**< USART0 signal: USART0_RTS0 on PB3 mux C */
#define MUX_PB3C_USART0_RTS0                       _L_(2)       /**< USART0 signal line function value: USART0_RTS0 */
#define PIO_PB3C_USART0_RTS0                       _UL_(1 << 3) /**< USART0 signal: USART0_RTS0 */
#define PIN_PB0C_USART0_RXD0                       _L_(32)      /**< USART0 signal: USART0_RXD0 on PB0 mux C */
#define MUX_PB0C_USART0_RXD0                       _L_(2)       /**< USART0 signal line function value: USART0_RXD0 */
#define PIO_PB0C_USART0_RXD0                       _UL_(1 << 0) /**< USART0 signal: USART0_RXD0 */
#define PIN_PB13C_USART0_SCK0                      _L_(45)      /**< USART0 signal: USART0_SCK0 on PB13 mux C */
#define MUX_PB13C_USART0_SCK0                      _L_(2)       /**< USART0 signal line function value: USART0_SCK0 */
#define PIO_PB13C_USART0_SCK0                      _UL_(1 << 13) /**< USART0 signal: USART0_SCK0 */
#define PIN_PB1C_USART0_TXD0                       _L_(33)      /**< USART0 signal: USART0_TXD0 on PB1 mux C */
#define MUX_PB1C_USART0_TXD0                       _L_(2)       /**< USART0 signal line function value: USART0_TXD0 */
#define PIO_PB1C_USART0_TXD0                       _UL_(1 << 1) /**< USART0 signal: USART0_TXD0 */
/* ========== PIO definition for USART1 peripheral ========== */
#define PIN_PA25A_USART1_CTS1                      _L_(25)      /**< USART1 signal: USART1_CTS1 on PA25 mux A */
#define MUX_PA25A_USART1_CTS1                      _L_(0)       /**< USART1 signal line function value: USART1_CTS1 */
#define PIO_PA25A_USART1_CTS1                      _UL_(1 << 25) /**< USART1 signal: USART1_CTS1 */
#define PIN_PA26A_USART1_DCD1                      _L_(26)      /**< USART1 signal: USART1_DCD1 on PA26 mux A */
#define MUX_PA26A_USART1_DCD1                      _L_(0)       /**< USART1 signal line function value: USART1_DCD1 */
#define PIO_PA26A_USART1_DCD1                      _UL_(1 << 26) /**< USART1 signal: USART1_DCD1 */
#define PIN_PA28A_USART1_DSR1                      _L_(28)      /**< USART1 signal: USART1_DSR1 on PA28 mux A */
#define MUX_PA28A_USART1_DSR1                      _L_(0)       /**< USART1 signal line function value: USART1_DSR1 */
#define PIO_PA28A_USART1_DSR1                      _UL_(1 << 28) /**< USART1 signal: USART1_DSR1 */
#define PIN_PA27A_USART1_DTR1                      _L_(27)      /**< USART1 signal: USART1_DTR1 on PA27 mux A */
#define MUX_PA27A_USART1_DTR1                      _L_(0)       /**< USART1 signal line function value: USART1_DTR1 */
#define PIO_PA27A_USART1_DTR1                      _UL_(1 << 27) /**< USART1 signal: USART1_DTR1 */
#define PIN_PA3B_USART1_LONCOL1                    _L_(3)       /**< USART1 signal: USART1_LONCOL1 on PA3 mux B */
#define MUX_PA3B_USART1_LONCOL1                    _L_(1)       /**< USART1 signal line function value: USART1_LONCOL1 */
#define PIO_PA3B_USART1_LONCOL1                    _UL_(1 << 3) /**< USART1 signal: USART1_LONCOL1 */
#define PIN_PA29A_USART1_RI1                       _L_(29)      /**< USART1 signal: USART1_RI1 on PA29 mux A */
#define MUX_PA29A_USART1_RI1                       _L_(0)       /**< USART1 signal line function value: USART1_RI1 */
#define PIO_PA29A_USART1_RI1                       _UL_(1 << 29) /**< USART1 signal: USART1_RI1 */
#define PIN_PA24A_USART1_RTS1                      _L_(24)      /**< USART1 signal: USART1_RTS1 on PA24 mux A */
#define MUX_PA24A_USART1_RTS1                      _L_(0)       /**< USART1 signal line function value: USART1_RTS1 */
#define PIO_PA24A_USART1_RTS1                      _UL_(1 << 24) /**< USART1 signal: USART1_RTS1 */
#define PIN_PA21A_USART1_RXD1                      _L_(21)      /**< USART1 signal: USART1_RXD1 on PA21 mux A */
#define MUX_PA21A_USART1_RXD1                      _L_(0)       /**< USART1 signal line function value: USART1_RXD1 */
#define PIO_PA21A_USART1_RXD1                      _UL_(1 << 21) /**< USART1 signal: USART1_RXD1 */
#define PIN_PA23A_USART1_SCK1                      _L_(23)      /**< USART1 signal: USART1_SCK1 on PA23 mux A */
#define MUX_PA23A_USART1_SCK1                      _L_(0)       /**< USART1 signal line function value: USART1_SCK1 */
#define PIO_PA23A_USART1_SCK1                      _UL_(1 << 23) /**< USART1 signal: USART1_SCK1 */
#define PIN_PB4D_USART1_TXD1                       _L_(36)      /**< USART1 signal: USART1_TXD1 on PB4 mux D */
#define MUX_PB4D_USART1_TXD1                       _L_(3)       /**< USART1 signal line function value: USART1_TXD1 */
#define PIO_PB4D_USART1_TXD1                       _UL_(1 << 4) /**< USART1 signal: USART1_TXD1 */
/* ========== PIO definition for USART2 peripheral ========== */
#define PIN_PD19B_USART2_CTS2                      _L_(115)     /**< USART2 signal: USART2_CTS2 on PD19 mux B */
#define MUX_PD19B_USART2_CTS2                      _L_(1)       /**< USART2 signal line function value: USART2_CTS2 */
#define PIO_PD19B_USART2_CTS2                      _UL_(1 << 19) /**< USART2 signal: USART2_CTS2 */
#define PIN_PD4D_USART2_DCD2                       _L_(100)     /**< USART2 signal: USART2_DCD2 on PD4 mux D */
#define MUX_PD4D_USART2_DCD2                       _L_(3)       /**< USART2 signal line function value: USART2_DCD2 */
#define PIO_PD4D_USART2_DCD2                       _UL_(1 << 4) /**< USART2 signal: USART2_DCD2 */
#define PIN_PD6D_USART2_DSR2                       _L_(102)     /**< USART2 signal: USART2_DSR2 on PD6 mux D */
#define MUX_PD6D_USART2_DSR2                       _L_(3)       /**< USART2 signal line function value: USART2_DSR2 */
#define PIO_PD6D_USART2_DSR2                       _UL_(1 << 6) /**< USART2 signal: USART2_DSR2 */
#define PIN_PD5D_USART2_DTR2                       _L_(101)     /**< USART2 signal: USART2_DTR2 on PD5 mux D */
#define MUX_PD5D_USART2_DTR2                       _L_(3)       /**< USART2 signal line function value: USART2_DTR2 */
#define PIO_PD5D_USART2_DTR2                       _UL_(1 << 5) /**< USART2 signal: USART2_DTR2 */
#define PIN_PD7D_USART2_RI2                        _L_(103)     /**< USART2 signal: USART2_RI2 on PD7 mux D */
#define MUX_PD7D_USART2_RI2                        _L_(3)       /**< USART2 signal line function value: USART2_RI2 */
#define PIO_PD7D_USART2_RI2                        _UL_(1 << 7) /**< USART2 signal: USART2_RI2 */
#define PIN_PD18B_USART2_RTS2                      _L_(114)     /**< USART2 signal: USART2_RTS2 on PD18 mux B */
#define MUX_PD18B_USART2_RTS2                      _L_(1)       /**< USART2 signal line function value: USART2_RTS2 */
#define PIO_PD18B_USART2_RTS2                      _UL_(1 << 18) /**< USART2 signal: USART2_RTS2 */
#define PIN_PD15B_USART2_RXD2                      _L_(111)     /**< USART2 signal: USART2_RXD2 on PD15 mux B */
#define MUX_PD15B_USART2_RXD2                      _L_(1)       /**< USART2 signal line function value: USART2_RXD2 */
#define PIO_PD15B_USART2_RXD2                      _UL_(1 << 15) /**< USART2 signal: USART2_RXD2 */
#define PIN_PD17B_USART2_SCK2                      _L_(113)     /**< USART2 signal: USART2_SCK2 on PD17 mux B */
#define MUX_PD17B_USART2_SCK2                      _L_(1)       /**< USART2 signal line function value: USART2_SCK2 */
#define PIO_PD17B_USART2_SCK2                      _UL_(1 << 17) /**< USART2 signal: USART2_SCK2 */
#define PIN_PD16B_USART2_TXD2                      _L_(112)     /**< USART2 signal: USART2_TXD2 on PD16 mux B */
#define MUX_PD16B_USART2_TXD2                      _L_(1)       /**< USART2 signal line function value: USART2_TXD2 */
#define PIO_PD16B_USART2_TXD2                      _UL_(1 << 16) /**< USART2 signal: USART2_TXD2 */
/* ========== PIO definition for ICE peripheral ========== */
#define PIN_PB4X1_ICE_TDI                          _L_(36)      /**< ICE signal: ICE_TDI on PB4 mux X1 */
#define PIO_PB4X1_ICE_TDI                          _UL_(1 << 4) /**< ICE signal: ICE_TDI */
#define PIN_PB5X1_ICE_TDO                          _L_(37)      /**< ICE signal: ICE_TDO on PB5 mux X1 */
#define PIO_PB5X1_ICE_TDO                          _UL_(1 << 5) /**< ICE signal: ICE_TDO */
#define PIN_PB5X1_ICE_TRACESWO                     _L_(37)      /**< ICE signal: ICE_TRACESWO on PB5 mux X1 */
#define PIO_PB5X1_ICE_TRACESWO                     _UL_(1 << 5) /**< ICE signal: ICE_TRACESWO */
#define PIN_PB6X1_ICE_TMS                          _L_(38)      /**< ICE signal: ICE_TMS on PB6 mux X1 */
#define PIO_PB6X1_ICE_TMS                          _UL_(1 << 6) /**< ICE signal: ICE_TMS */
#define PIN_PB6X1_ICE_SWDIO                        _L_(38)      /**< ICE signal: ICE_SWDIO on PB6 mux X1 */
#define PIO_PB6X1_ICE_SWDIO                        _UL_(1 << 6) /**< ICE signal: ICE_SWDIO */
#define PIN_PB7X1_ICE_TCK                          _L_(39)      /**< ICE signal: ICE_TCK on PB7 mux X1 */
#define PIO_PB7X1_ICE_TCK                          _UL_(1 << 7) /**< ICE signal: ICE_TCK */
#define PIN_PB7X1_ICE_SWDCLK                       _L_(39)      /**< ICE signal: ICE_SWDCLK on PB7 mux X1 */
#define PIO_PB7X1_ICE_SWDCLK                       _UL_(1 << 7) /**< ICE signal: ICE_SWDCLK */
/* ========== PIO definition for TPIU peripheral ========== */
#define PIN_PD8D_TPIU_TRACECLK                     _L_(104)     /**< TPIU signal: TPIU_TRACECLK on PD8 mux D */
#define MUX_PD8D_TPIU_TRACECLK                     _L_(3)       /**< TPIU signal line function value: TPIU_TRACECLK */
#define PIO_PD8D_TPIU_TRACECLK                     _UL_(1 << 8) /**< TPIU signal: TPIU_TRACECLK */
#define PIN_PD4C_TPIU_TRACED0                      _L_(100)     /**< TPIU signal: TPIU_TRACED0 on PD4 mux C */
#define MUX_PD4C_TPIU_TRACED0                      _L_(2)       /**< TPIU signal line function value: TPIU_TRACED0 */
#define PIO_PD4C_TPIU_TRACED0                      _UL_(1 << 4) /**< TPIU signal: TPIU_TRACED0 */
#define PIN_PD5C_TPIU_TRACED1                      _L_(101)     /**< TPIU signal: TPIU_TRACED1 on PD5 mux C */
#define MUX_PD5C_TPIU_TRACED1                      _L_(2)       /**< TPIU signal line function value: TPIU_TRACED1 */
#define PIO_PD5C_TPIU_TRACED1                      _UL_(1 << 5) /**< TPIU signal: TPIU_TRACED1 */
#define PIN_PD6C_TPIU_TRACED2                      _L_(102)     /**< TPIU signal: TPIU_TRACED2 on PD6 mux C */
#define MUX_PD6C_TPIU_TRACED2                      _L_(2)       /**< TPIU signal line function value: TPIU_TRACED2 */
#define PIO_PD6C_TPIU_TRACED2                      _UL_(1 << 6) /**< TPIU signal: TPIU_TRACED2 */
#define PIN_PD7C_TPIU_TRACED3                      _L_(103)     /**< TPIU signal: TPIU_TRACED3 on PD7 mux C */
#define MUX_PD7C_TPIU_TRACED3                      _L_(2)       /**< TPIU signal line function value: TPIU_TRACED3 */
#define PIO_PD7C_TPIU_TRACED3                      _UL_(1 << 7) /**< TPIU signal: TPIU_TRACED3 */

#endif /* _SAME70Q21B_GPIO_H_ */

