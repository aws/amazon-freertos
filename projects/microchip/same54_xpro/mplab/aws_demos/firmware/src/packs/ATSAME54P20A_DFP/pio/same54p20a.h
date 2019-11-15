/**
 * \brief Peripheral I/O description for SAME54P20A
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

/* file generated from device description version 2019-06-03T16:18:52Z */
#ifndef _SAME54P20A_GPIO_H_
#define _SAME54P20A_GPIO_H_

/* ========== Peripheral I/O pin numbers ========== */
#define PIN_PA00                    (  0  )  /**< Pin Number for PA00 */
#define PIN_PA01                    (  1  )  /**< Pin Number for PA01 */
#define PIN_PA02                    (  2  )  /**< Pin Number for PA02 */
#define PIN_PA03                    (  3  )  /**< Pin Number for PA03 */
#define PIN_PA04                    (  4  )  /**< Pin Number for PA04 */
#define PIN_PA05                    (  5  )  /**< Pin Number for PA05 */
#define PIN_PA06                    (  6  )  /**< Pin Number for PA06 */
#define PIN_PA07                    (  7  )  /**< Pin Number for PA07 */
#define PIN_PA08                    (  8  )  /**< Pin Number for PA08 */
#define PIN_PA09                    (  9  )  /**< Pin Number for PA09 */
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
#define PIN_PA27                    ( 27  )  /**< Pin Number for PA27 */
#define PIN_PA30                    ( 30  )  /**< Pin Number for PA30 */
#define PIN_PA31                    ( 31  )  /**< Pin Number for PA31 */
#define PIN_PB00                    ( 32  )  /**< Pin Number for PB00 */
#define PIN_PB01                    ( 33  )  /**< Pin Number for PB01 */
#define PIN_PB02                    ( 34  )  /**< Pin Number for PB02 */
#define PIN_PB03                    ( 35  )  /**< Pin Number for PB03 */
#define PIN_PB04                    ( 36  )  /**< Pin Number for PB04 */
#define PIN_PB05                    ( 37  )  /**< Pin Number for PB05 */
#define PIN_PB06                    ( 38  )  /**< Pin Number for PB06 */
#define PIN_PB07                    ( 39  )  /**< Pin Number for PB07 */
#define PIN_PB08                    ( 40  )  /**< Pin Number for PB08 */
#define PIN_PB09                    ( 41  )  /**< Pin Number for PB09 */
#define PIN_PB10                    ( 42  )  /**< Pin Number for PB10 */
#define PIN_PB11                    ( 43  )  /**< Pin Number for PB11 */
#define PIN_PB12                    ( 44  )  /**< Pin Number for PB12 */
#define PIN_PB13                    ( 45  )  /**< Pin Number for PB13 */
#define PIN_PB14                    ( 46  )  /**< Pin Number for PB14 */
#define PIN_PB15                    ( 47  )  /**< Pin Number for PB15 */
#define PIN_PB16                    ( 48  )  /**< Pin Number for PB16 */
#define PIN_PB17                    ( 49  )  /**< Pin Number for PB17 */
#define PIN_PB18                    ( 50  )  /**< Pin Number for PB18 */
#define PIN_PB19                    ( 51  )  /**< Pin Number for PB19 */
#define PIN_PB20                    ( 52  )  /**< Pin Number for PB20 */
#define PIN_PB21                    ( 53  )  /**< Pin Number for PB21 */
#define PIN_PB22                    ( 54  )  /**< Pin Number for PB22 */
#define PIN_PB23                    ( 55  )  /**< Pin Number for PB23 */
#define PIN_PB24                    ( 56  )  /**< Pin Number for PB24 */
#define PIN_PB25                    ( 57  )  /**< Pin Number for PB25 */
#define PIN_PB26                    ( 58  )  /**< Pin Number for PB26 */
#define PIN_PB27                    ( 59  )  /**< Pin Number for PB27 */
#define PIN_PB28                    ( 60  )  /**< Pin Number for PB28 */
#define PIN_PB29                    ( 61  )  /**< Pin Number for PB29 */
#define PIN_PB30                    ( 62  )  /**< Pin Number for PB30 */
#define PIN_PB31                    ( 63  )  /**< Pin Number for PB31 */
#define PIN_PC00                    ( 64  )  /**< Pin Number for PC00 */
#define PIN_PC01                    ( 65  )  /**< Pin Number for PC01 */
#define PIN_PC02                    ( 66  )  /**< Pin Number for PC02 */
#define PIN_PC03                    ( 67  )  /**< Pin Number for PC03 */
#define PIN_PC04                    ( 68  )  /**< Pin Number for PC04 */
#define PIN_PC05                    ( 69  )  /**< Pin Number for PC05 */
#define PIN_PC06                    ( 70  )  /**< Pin Number for PC06 */
#define PIN_PC07                    ( 71  )  /**< Pin Number for PC07 */
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
#define PIN_PC30                    ( 94  )  /**< Pin Number for PC30 */
#define PIN_PC31                    ( 95  )  /**< Pin Number for PC31 */
#define PIN_PD00                    ( 96  )  /**< Pin Number for PD00 */
#define PIN_PD01                    ( 97  )  /**< Pin Number for PD01 */
#define PIN_PD08                    (104  )  /**< Pin Number for PD08 */
#define PIN_PD09                    (105  )  /**< Pin Number for PD09 */
#define PIN_PD10                    (106  )  /**< Pin Number for PD10 */
#define PIN_PD11                    (107  )  /**< Pin Number for PD11 */
#define PIN_PD12                    (108  )  /**< Pin Number for PD12 */
#define PIN_PD20                    (116  )  /**< Pin Number for PD20 */
#define PIN_PD21                    (117  )  /**< Pin Number for PD21 */

/* ========== Peripheral I/O masks ========== */
#define PORT_PA00                   _UL_(1 << 0)    /**< PORT mask for PA00 */
#define PORT_PA01                   _UL_(1 << 1)    /**< PORT mask for PA01 */
#define PORT_PA02                   _UL_(1 << 2)    /**< PORT mask for PA02 */
#define PORT_PA03                   _UL_(1 << 3)    /**< PORT mask for PA03 */
#define PORT_PA04                   _UL_(1 << 4)    /**< PORT mask for PA04 */
#define PORT_PA05                   _UL_(1 << 5)    /**< PORT mask for PA05 */
#define PORT_PA06                   _UL_(1 << 6)    /**< PORT mask for PA06 */
#define PORT_PA07                   _UL_(1 << 7)    /**< PORT mask for PA07 */
#define PORT_PA08                   _UL_(1 << 8)    /**< PORT mask for PA08 */
#define PORT_PA09                   _UL_(1 << 9)    /**< PORT mask for PA09 */
#define PORT_PA10                   _UL_(1 << 10)   /**< PORT mask for PA10 */
#define PORT_PA11                   _UL_(1 << 11)   /**< PORT mask for PA11 */
#define PORT_PA12                   _UL_(1 << 12)   /**< PORT mask for PA12 */
#define PORT_PA13                   _UL_(1 << 13)   /**< PORT mask for PA13 */
#define PORT_PA14                   _UL_(1 << 14)   /**< PORT mask for PA14 */
#define PORT_PA15                   _UL_(1 << 15)   /**< PORT mask for PA15 */
#define PORT_PA16                   _UL_(1 << 16)   /**< PORT mask for PA16 */
#define PORT_PA17                   _UL_(1 << 17)   /**< PORT mask for PA17 */
#define PORT_PA18                   _UL_(1 << 18)   /**< PORT mask for PA18 */
#define PORT_PA19                   _UL_(1 << 19)   /**< PORT mask for PA19 */
#define PORT_PA20                   _UL_(1 << 20)   /**< PORT mask for PA20 */
#define PORT_PA21                   _UL_(1 << 21)   /**< PORT mask for PA21 */
#define PORT_PA22                   _UL_(1 << 22)   /**< PORT mask for PA22 */
#define PORT_PA23                   _UL_(1 << 23)   /**< PORT mask for PA23 */
#define PORT_PA24                   _UL_(1 << 24)   /**< PORT mask for PA24 */
#define PORT_PA25                   _UL_(1 << 25)   /**< PORT mask for PA25 */
#define PORT_PA27                   _UL_(1 << 27)   /**< PORT mask for PA27 */
#define PORT_PA30                   _UL_(1 << 30)   /**< PORT mask for PA30 */
#define PORT_PA31                   _UL_(1 << 31)   /**< PORT mask for PA31 */
#define PORT_PB00                   _UL_(1 << 0)    /**< PORT mask for PB00 */
#define PORT_PB01                   _UL_(1 << 1)    /**< PORT mask for PB01 */
#define PORT_PB02                   _UL_(1 << 2)    /**< PORT mask for PB02 */
#define PORT_PB03                   _UL_(1 << 3)    /**< PORT mask for PB03 */
#define PORT_PB04                   _UL_(1 << 4)    /**< PORT mask for PB04 */
#define PORT_PB05                   _UL_(1 << 5)    /**< PORT mask for PB05 */
#define PORT_PB06                   _UL_(1 << 6)    /**< PORT mask for PB06 */
#define PORT_PB07                   _UL_(1 << 7)    /**< PORT mask for PB07 */
#define PORT_PB08                   _UL_(1 << 8)    /**< PORT mask for PB08 */
#define PORT_PB09                   _UL_(1 << 9)    /**< PORT mask for PB09 */
#define PORT_PB10                   _UL_(1 << 10)   /**< PORT mask for PB10 */
#define PORT_PB11                   _UL_(1 << 11)   /**< PORT mask for PB11 */
#define PORT_PB12                   _UL_(1 << 12)   /**< PORT mask for PB12 */
#define PORT_PB13                   _UL_(1 << 13)   /**< PORT mask for PB13 */
#define PORT_PB14                   _UL_(1 << 14)   /**< PORT mask for PB14 */
#define PORT_PB15                   _UL_(1 << 15)   /**< PORT mask for PB15 */
#define PORT_PB16                   _UL_(1 << 16)   /**< PORT mask for PB16 */
#define PORT_PB17                   _UL_(1 << 17)   /**< PORT mask for PB17 */
#define PORT_PB18                   _UL_(1 << 18)   /**< PORT mask for PB18 */
#define PORT_PB19                   _UL_(1 << 19)   /**< PORT mask for PB19 */
#define PORT_PB20                   _UL_(1 << 20)   /**< PORT mask for PB20 */
#define PORT_PB21                   _UL_(1 << 21)   /**< PORT mask for PB21 */
#define PORT_PB22                   _UL_(1 << 22)   /**< PORT mask for PB22 */
#define PORT_PB23                   _UL_(1 << 23)   /**< PORT mask for PB23 */
#define PORT_PB24                   _UL_(1 << 24)   /**< PORT mask for PB24 */
#define PORT_PB25                   _UL_(1 << 25)   /**< PORT mask for PB25 */
#define PORT_PB26                   _UL_(1 << 26)   /**< PORT mask for PB26 */
#define PORT_PB27                   _UL_(1 << 27)   /**< PORT mask for PB27 */
#define PORT_PB28                   _UL_(1 << 28)   /**< PORT mask for PB28 */
#define PORT_PB29                   _UL_(1 << 29)   /**< PORT mask for PB29 */
#define PORT_PB30                   _UL_(1 << 30)   /**< PORT mask for PB30 */
#define PORT_PB31                   _UL_(1 << 31)   /**< PORT mask for PB31 */
#define PORT_PC00                   _UL_(1 << 0)    /**< PORT mask for PC00 */
#define PORT_PC01                   _UL_(1 << 1)    /**< PORT mask for PC01 */
#define PORT_PC02                   _UL_(1 << 2)    /**< PORT mask for PC02 */
#define PORT_PC03                   _UL_(1 << 3)    /**< PORT mask for PC03 */
#define PORT_PC04                   _UL_(1 << 4)    /**< PORT mask for PC04 */
#define PORT_PC05                   _UL_(1 << 5)    /**< PORT mask for PC05 */
#define PORT_PC06                   _UL_(1 << 6)    /**< PORT mask for PC06 */
#define PORT_PC07                   _UL_(1 << 7)    /**< PORT mask for PC07 */
#define PORT_PC10                   _UL_(1 << 10)   /**< PORT mask for PC10 */
#define PORT_PC11                   _UL_(1 << 11)   /**< PORT mask for PC11 */
#define PORT_PC12                   _UL_(1 << 12)   /**< PORT mask for PC12 */
#define PORT_PC13                   _UL_(1 << 13)   /**< PORT mask for PC13 */
#define PORT_PC14                   _UL_(1 << 14)   /**< PORT mask for PC14 */
#define PORT_PC15                   _UL_(1 << 15)   /**< PORT mask for PC15 */
#define PORT_PC16                   _UL_(1 << 16)   /**< PORT mask for PC16 */
#define PORT_PC17                   _UL_(1 << 17)   /**< PORT mask for PC17 */
#define PORT_PC18                   _UL_(1 << 18)   /**< PORT mask for PC18 */
#define PORT_PC19                   _UL_(1 << 19)   /**< PORT mask for PC19 */
#define PORT_PC20                   _UL_(1 << 20)   /**< PORT mask for PC20 */
#define PORT_PC21                   _UL_(1 << 21)   /**< PORT mask for PC21 */
#define PORT_PC22                   _UL_(1 << 22)   /**< PORT mask for PC22 */
#define PORT_PC23                   _UL_(1 << 23)   /**< PORT mask for PC23 */
#define PORT_PC24                   _UL_(1 << 24)   /**< PORT mask for PC24 */
#define PORT_PC25                   _UL_(1 << 25)   /**< PORT mask for PC25 */
#define PORT_PC26                   _UL_(1 << 26)   /**< PORT mask for PC26 */
#define PORT_PC27                   _UL_(1 << 27)   /**< PORT mask for PC27 */
#define PORT_PC28                   _UL_(1 << 28)   /**< PORT mask for PC28 */
#define PORT_PC30                   _UL_(1 << 30)   /**< PORT mask for PC30 */
#define PORT_PC31                   _UL_(1 << 31)   /**< PORT mask for PC31 */
#define PORT_PD00                   _UL_(1 << 0)    /**< PORT mask for PD00 */
#define PORT_PD01                   _UL_(1 << 1)    /**< PORT mask for PD01 */
#define PORT_PD08                   _UL_(1 << 8)    /**< PORT mask for PD08 */
#define PORT_PD09                   _UL_(1 << 9)    /**< PORT mask for PD09 */
#define PORT_PD10                   _UL_(1 << 10)   /**< PORT mask for PD10 */
#define PORT_PD11                   _UL_(1 << 11)   /**< PORT mask for PD11 */
#define PORT_PD12                   _UL_(1 << 12)   /**< PORT mask for PD12 */
#define PORT_PD20                   _UL_(1 << 20)   /**< PORT mask for PD20 */
#define PORT_PD21                   _UL_(1 << 21)   /**< PORT mask for PD21 */

/* ========== PORT definition for AC peripheral ========== */
#define PIN_PA04B_AC_AIN0                          (4L)         
#define MUX_PA04B_AC_AIN0                          (1L)        
#define PINMUX_PA04B_AC_AIN0                       ((PIN_PA04B_AC_AIN0 << 16) | MUX_PA04B_AC_AIN0)
#define PORT_PA04B_AC_AIN0                         ((1UL) << 4)

#define PIN_PA05B_AC_AIN1                          (5L)         
#define MUX_PA05B_AC_AIN1                          (1L)        
#define PINMUX_PA05B_AC_AIN1                       ((PIN_PA05B_AC_AIN1 << 16) | MUX_PA05B_AC_AIN1)
#define PORT_PA05B_AC_AIN1                         ((1UL) << 5)

#define PIN_PA06B_AC_AIN2                          (6L)         
#define MUX_PA06B_AC_AIN2                          (1L)        
#define PINMUX_PA06B_AC_AIN2                       ((PIN_PA06B_AC_AIN2 << 16) | MUX_PA06B_AC_AIN2)
#define PORT_PA06B_AC_AIN2                         ((1UL) << 6)

#define PIN_PA07B_AC_AIN3                          (7L)         
#define MUX_PA07B_AC_AIN3                          (1L)        
#define PINMUX_PA07B_AC_AIN3                       ((PIN_PA07B_AC_AIN3 << 16) | MUX_PA07B_AC_AIN3)
#define PORT_PA07B_AC_AIN3                         ((1UL) << 7)

#define PIN_PA12M_AC_CMP0                          (12L)        
#define MUX_PA12M_AC_CMP0                          (12L)       
#define PINMUX_PA12M_AC_CMP0                       ((PIN_PA12M_AC_CMP0 << 16) | MUX_PA12M_AC_CMP0)
#define PORT_PA12M_AC_CMP0                         ((1UL) << 12)

#define PIN_PA18M_AC_CMP0                          (18L)        
#define MUX_PA18M_AC_CMP0                          (12L)       
#define PINMUX_PA18M_AC_CMP0                       ((PIN_PA18M_AC_CMP0 << 16) | MUX_PA18M_AC_CMP0)
#define PORT_PA18M_AC_CMP0                         ((1UL) << 18)

#define PIN_PB24M_AC_CMP0                          (56L)        
#define MUX_PB24M_AC_CMP0                          (12L)       
#define PINMUX_PB24M_AC_CMP0                       ((PIN_PB24M_AC_CMP0 << 16) | MUX_PB24M_AC_CMP0)
#define PORT_PB24M_AC_CMP0                         ((1UL) << 24)

#define PIN_PA13M_AC_CMP1                          (13L)        
#define MUX_PA13M_AC_CMP1                          (12L)       
#define PINMUX_PA13M_AC_CMP1                       ((PIN_PA13M_AC_CMP1 << 16) | MUX_PA13M_AC_CMP1)
#define PORT_PA13M_AC_CMP1                         ((1UL) << 13)

#define PIN_PA19M_AC_CMP1                          (19L)        
#define MUX_PA19M_AC_CMP1                          (12L)       
#define PINMUX_PA19M_AC_CMP1                       ((PIN_PA19M_AC_CMP1 << 16) | MUX_PA19M_AC_CMP1)
#define PORT_PA19M_AC_CMP1                         ((1UL) << 19)

#define PIN_PB25M_AC_CMP1                          (57L)        
#define MUX_PB25M_AC_CMP1                          (12L)       
#define PINMUX_PB25M_AC_CMP1                       ((PIN_PB25M_AC_CMP1 << 16) | MUX_PB25M_AC_CMP1)
#define PORT_PB25M_AC_CMP1                         ((1UL) << 25)

/* ========== PORT definition for ADC0 peripheral ========== */
#define PIN_PA02B_ADC0_AIN0                        (2L)         
#define MUX_PA02B_ADC0_AIN0                        (1L)        
#define PINMUX_PA02B_ADC0_AIN0                     ((PIN_PA02B_ADC0_AIN0 << 16) | MUX_PA02B_ADC0_AIN0)
#define PORT_PA02B_ADC0_AIN0                       ((1UL) << 2)

#define PIN_PA03B_ADC0_AIN1                        (3L)         
#define MUX_PA03B_ADC0_AIN1                        (1L)        
#define PINMUX_PA03B_ADC0_AIN1                     ((PIN_PA03B_ADC0_AIN1 << 16) | MUX_PA03B_ADC0_AIN1)
#define PORT_PA03B_ADC0_AIN1                       ((1UL) << 3)

#define PIN_PB08B_ADC0_AIN2                        (40L)        
#define MUX_PB08B_ADC0_AIN2                        (1L)        
#define PINMUX_PB08B_ADC0_AIN2                     ((PIN_PB08B_ADC0_AIN2 << 16) | MUX_PB08B_ADC0_AIN2)
#define PORT_PB08B_ADC0_AIN2                       ((1UL) << 8)

#define PIN_PB09B_ADC0_AIN3                        (41L)        
#define MUX_PB09B_ADC0_AIN3                        (1L)        
#define PINMUX_PB09B_ADC0_AIN3                     ((PIN_PB09B_ADC0_AIN3 << 16) | MUX_PB09B_ADC0_AIN3)
#define PORT_PB09B_ADC0_AIN3                       ((1UL) << 9)

#define PIN_PA04B_ADC0_AIN4                        (4L)         
#define MUX_PA04B_ADC0_AIN4                        (1L)        
#define PINMUX_PA04B_ADC0_AIN4                     ((PIN_PA04B_ADC0_AIN4 << 16) | MUX_PA04B_ADC0_AIN4)
#define PORT_PA04B_ADC0_AIN4                       ((1UL) << 4)

#define PIN_PA05B_ADC0_AIN5                        (5L)         
#define MUX_PA05B_ADC0_AIN5                        (1L)        
#define PINMUX_PA05B_ADC0_AIN5                     ((PIN_PA05B_ADC0_AIN5 << 16) | MUX_PA05B_ADC0_AIN5)
#define PORT_PA05B_ADC0_AIN5                       ((1UL) << 5)

#define PIN_PA06B_ADC0_AIN6                        (6L)         
#define MUX_PA06B_ADC0_AIN6                        (1L)        
#define PINMUX_PA06B_ADC0_AIN6                     ((PIN_PA06B_ADC0_AIN6 << 16) | MUX_PA06B_ADC0_AIN6)
#define PORT_PA06B_ADC0_AIN6                       ((1UL) << 6)

#define PIN_PA07B_ADC0_AIN7                        (7L)         
#define MUX_PA07B_ADC0_AIN7                        (1L)        
#define PINMUX_PA07B_ADC0_AIN7                     ((PIN_PA07B_ADC0_AIN7 << 16) | MUX_PA07B_ADC0_AIN7)
#define PORT_PA07B_ADC0_AIN7                       ((1UL) << 7)

#define PIN_PA08B_ADC0_AIN8                        (8L)         
#define MUX_PA08B_ADC0_AIN8                        (1L)        
#define PINMUX_PA08B_ADC0_AIN8                     ((PIN_PA08B_ADC0_AIN8 << 16) | MUX_PA08B_ADC0_AIN8)
#define PORT_PA08B_ADC0_AIN8                       ((1UL) << 8)

#define PIN_PA09B_ADC0_AIN9                        (9L)         
#define MUX_PA09B_ADC0_AIN9                        (1L)        
#define PINMUX_PA09B_ADC0_AIN9                     ((PIN_PA09B_ADC0_AIN9 << 16) | MUX_PA09B_ADC0_AIN9)
#define PORT_PA09B_ADC0_AIN9                       ((1UL) << 9)

#define PIN_PA10B_ADC0_AIN10                       (10L)        
#define MUX_PA10B_ADC0_AIN10                       (1L)        
#define PINMUX_PA10B_ADC0_AIN10                    ((PIN_PA10B_ADC0_AIN10 << 16) | MUX_PA10B_ADC0_AIN10)
#define PORT_PA10B_ADC0_AIN10                      ((1UL) << 10)

#define PIN_PA11B_ADC0_AIN11                       (11L)        
#define MUX_PA11B_ADC0_AIN11                       (1L)        
#define PINMUX_PA11B_ADC0_AIN11                    ((PIN_PA11B_ADC0_AIN11 << 16) | MUX_PA11B_ADC0_AIN11)
#define PORT_PA11B_ADC0_AIN11                      ((1UL) << 11)

#define PIN_PB00B_ADC0_AIN12                       (32L)        
#define MUX_PB00B_ADC0_AIN12                       (1L)        
#define PINMUX_PB00B_ADC0_AIN12                    ((PIN_PB00B_ADC0_AIN12 << 16) | MUX_PB00B_ADC0_AIN12)
#define PORT_PB00B_ADC0_AIN12                      ((1UL) << 0)

#define PIN_PB01B_ADC0_AIN13                       (33L)        
#define MUX_PB01B_ADC0_AIN13                       (1L)        
#define PINMUX_PB01B_ADC0_AIN13                    ((PIN_PB01B_ADC0_AIN13 << 16) | MUX_PB01B_ADC0_AIN13)
#define PORT_PB01B_ADC0_AIN13                      ((1UL) << 1)

#define PIN_PB02B_ADC0_AIN14                       (34L)        
#define MUX_PB02B_ADC0_AIN14                       (1L)        
#define PINMUX_PB02B_ADC0_AIN14                    ((PIN_PB02B_ADC0_AIN14 << 16) | MUX_PB02B_ADC0_AIN14)
#define PORT_PB02B_ADC0_AIN14                      ((1UL) << 2)

#define PIN_PB03B_ADC0_AIN15                       (35L)        
#define MUX_PB03B_ADC0_AIN15                       (1L)        
#define PINMUX_PB03B_ADC0_AIN15                    ((PIN_PB03B_ADC0_AIN15 << 16) | MUX_PB03B_ADC0_AIN15)
#define PORT_PB03B_ADC0_AIN15                      ((1UL) << 3)

#define PIN_PA03B_ADC0_X0                          (3L)         
#define MUX_PA03B_ADC0_X0                          (1L)        
#define PINMUX_PA03B_ADC0_X0                       ((PIN_PA03B_ADC0_X0 << 16) | MUX_PA03B_ADC0_X0)
#define PORT_PA03B_ADC0_X0                         ((1UL) << 3)

#define PIN_PA03B_ADC0_Y0                          (3L)         
#define MUX_PA03B_ADC0_Y0                          (1L)        
#define PINMUX_PA03B_ADC0_Y0                       ((PIN_PA03B_ADC0_Y0 << 16) | MUX_PA03B_ADC0_Y0)
#define PORT_PA03B_ADC0_Y0                         ((1UL) << 3)

#define PIN_PB08B_ADC0_X1                          (40L)        
#define MUX_PB08B_ADC0_X1                          (1L)        
#define PINMUX_PB08B_ADC0_X1                       ((PIN_PB08B_ADC0_X1 << 16) | MUX_PB08B_ADC0_X1)
#define PORT_PB08B_ADC0_X1                         ((1UL) << 8)

#define PIN_PB08B_ADC0_Y1                          (40L)        
#define MUX_PB08B_ADC0_Y1                          (1L)        
#define PINMUX_PB08B_ADC0_Y1                       ((PIN_PB08B_ADC0_Y1 << 16) | MUX_PB08B_ADC0_Y1)
#define PORT_PB08B_ADC0_Y1                         ((1UL) << 8)

#define PIN_PB09B_ADC0_X2                          (41L)        
#define MUX_PB09B_ADC0_X2                          (1L)        
#define PINMUX_PB09B_ADC0_X2                       ((PIN_PB09B_ADC0_X2 << 16) | MUX_PB09B_ADC0_X2)
#define PORT_PB09B_ADC0_X2                         ((1UL) << 9)

#define PIN_PB09B_ADC0_Y2                          (41L)        
#define MUX_PB09B_ADC0_Y2                          (1L)        
#define PINMUX_PB09B_ADC0_Y2                       ((PIN_PB09B_ADC0_Y2 << 16) | MUX_PB09B_ADC0_Y2)
#define PORT_PB09B_ADC0_Y2                         ((1UL) << 9)

#define PIN_PA04B_ADC0_X3                          (4L)         
#define MUX_PA04B_ADC0_X3                          (1L)        
#define PINMUX_PA04B_ADC0_X3                       ((PIN_PA04B_ADC0_X3 << 16) | MUX_PA04B_ADC0_X3)
#define PORT_PA04B_ADC0_X3                         ((1UL) << 4)

#define PIN_PA04B_ADC0_Y3                          (4L)         
#define MUX_PA04B_ADC0_Y3                          (1L)        
#define PINMUX_PA04B_ADC0_Y3                       ((PIN_PA04B_ADC0_Y3 << 16) | MUX_PA04B_ADC0_Y3)
#define PORT_PA04B_ADC0_Y3                         ((1UL) << 4)

#define PIN_PA06B_ADC0_X4                          (6L)         
#define MUX_PA06B_ADC0_X4                          (1L)        
#define PINMUX_PA06B_ADC0_X4                       ((PIN_PA06B_ADC0_X4 << 16) | MUX_PA06B_ADC0_X4)
#define PORT_PA06B_ADC0_X4                         ((1UL) << 6)

#define PIN_PA06B_ADC0_Y4                          (6L)         
#define MUX_PA06B_ADC0_Y4                          (1L)        
#define PINMUX_PA06B_ADC0_Y4                       ((PIN_PA06B_ADC0_Y4 << 16) | MUX_PA06B_ADC0_Y4)
#define PORT_PA06B_ADC0_Y4                         ((1UL) << 6)

#define PIN_PA07B_ADC0_X5                          (7L)         
#define MUX_PA07B_ADC0_X5                          (1L)        
#define PINMUX_PA07B_ADC0_X5                       ((PIN_PA07B_ADC0_X5 << 16) | MUX_PA07B_ADC0_X5)
#define PORT_PA07B_ADC0_X5                         ((1UL) << 7)

#define PIN_PA07B_ADC0_Y5                          (7L)         
#define MUX_PA07B_ADC0_Y5                          (1L)        
#define PINMUX_PA07B_ADC0_Y5                       ((PIN_PA07B_ADC0_Y5 << 16) | MUX_PA07B_ADC0_Y5)
#define PORT_PA07B_ADC0_Y5                         ((1UL) << 7)

#define PIN_PA08B_ADC0_X6                          (8L)         
#define MUX_PA08B_ADC0_X6                          (1L)        
#define PINMUX_PA08B_ADC0_X6                       ((PIN_PA08B_ADC0_X6 << 16) | MUX_PA08B_ADC0_X6)
#define PORT_PA08B_ADC0_X6                         ((1UL) << 8)

#define PIN_PA08B_ADC0_Y6                          (8L)         
#define MUX_PA08B_ADC0_Y6                          (1L)        
#define PINMUX_PA08B_ADC0_Y6                       ((PIN_PA08B_ADC0_Y6 << 16) | MUX_PA08B_ADC0_Y6)
#define PORT_PA08B_ADC0_Y6                         ((1UL) << 8)

#define PIN_PA09B_ADC0_X7                          (9L)         
#define MUX_PA09B_ADC0_X7                          (1L)        
#define PINMUX_PA09B_ADC0_X7                       ((PIN_PA09B_ADC0_X7 << 16) | MUX_PA09B_ADC0_X7)
#define PORT_PA09B_ADC0_X7                         ((1UL) << 9)

#define PIN_PA09B_ADC0_Y7                          (9L)         
#define MUX_PA09B_ADC0_Y7                          (1L)        
#define PINMUX_PA09B_ADC0_Y7                       ((PIN_PA09B_ADC0_Y7 << 16) | MUX_PA09B_ADC0_Y7)
#define PORT_PA09B_ADC0_Y7                         ((1UL) << 9)

#define PIN_PA10B_ADC0_X8                          (10L)        
#define MUX_PA10B_ADC0_X8                          (1L)        
#define PINMUX_PA10B_ADC0_X8                       ((PIN_PA10B_ADC0_X8 << 16) | MUX_PA10B_ADC0_X8)
#define PORT_PA10B_ADC0_X8                         ((1UL) << 10)

#define PIN_PA10B_ADC0_Y8                          (10L)        
#define MUX_PA10B_ADC0_Y8                          (1L)        
#define PINMUX_PA10B_ADC0_Y8                       ((PIN_PA10B_ADC0_Y8 << 16) | MUX_PA10B_ADC0_Y8)
#define PORT_PA10B_ADC0_Y8                         ((1UL) << 10)

#define PIN_PA11B_ADC0_X9                          (11L)        
#define MUX_PA11B_ADC0_X9                          (1L)        
#define PINMUX_PA11B_ADC0_X9                       ((PIN_PA11B_ADC0_X9 << 16) | MUX_PA11B_ADC0_X9)
#define PORT_PA11B_ADC0_X9                         ((1UL) << 11)

#define PIN_PA11B_ADC0_Y9                          (11L)        
#define MUX_PA11B_ADC0_Y9                          (1L)        
#define PINMUX_PA11B_ADC0_Y9                       ((PIN_PA11B_ADC0_Y9 << 16) | MUX_PA11B_ADC0_Y9)
#define PORT_PA11B_ADC0_Y9                         ((1UL) << 11)

#define PIN_PA16B_ADC0_X10                         (16L)        
#define MUX_PA16B_ADC0_X10                         (1L)        
#define PINMUX_PA16B_ADC0_X10                      ((PIN_PA16B_ADC0_X10 << 16) | MUX_PA16B_ADC0_X10)
#define PORT_PA16B_ADC0_X10                        ((1UL) << 16)

#define PIN_PA16B_ADC0_Y10                         (16L)        
#define MUX_PA16B_ADC0_Y10                         (1L)        
#define PINMUX_PA16B_ADC0_Y10                      ((PIN_PA16B_ADC0_Y10 << 16) | MUX_PA16B_ADC0_Y10)
#define PORT_PA16B_ADC0_Y10                        ((1UL) << 16)

#define PIN_PA17B_ADC0_X11                         (17L)        
#define MUX_PA17B_ADC0_X11                         (1L)        
#define PINMUX_PA17B_ADC0_X11                      ((PIN_PA17B_ADC0_X11 << 16) | MUX_PA17B_ADC0_X11)
#define PORT_PA17B_ADC0_X11                        ((1UL) << 17)

#define PIN_PA17B_ADC0_Y11                         (17L)        
#define MUX_PA17B_ADC0_Y11                         (1L)        
#define PINMUX_PA17B_ADC0_Y11                      ((PIN_PA17B_ADC0_Y11 << 16) | MUX_PA17B_ADC0_Y11)
#define PORT_PA17B_ADC0_Y11                        ((1UL) << 17)

#define PIN_PA18B_ADC0_X12                         (18L)        
#define MUX_PA18B_ADC0_X12                         (1L)        
#define PINMUX_PA18B_ADC0_X12                      ((PIN_PA18B_ADC0_X12 << 16) | MUX_PA18B_ADC0_X12)
#define PORT_PA18B_ADC0_X12                        ((1UL) << 18)

#define PIN_PA18B_ADC0_Y12                         (18L)        
#define MUX_PA18B_ADC0_Y12                         (1L)        
#define PINMUX_PA18B_ADC0_Y12                      ((PIN_PA18B_ADC0_Y12 << 16) | MUX_PA18B_ADC0_Y12)
#define PORT_PA18B_ADC0_Y12                        ((1UL) << 18)

#define PIN_PA19B_ADC0_X13                         (19L)        
#define MUX_PA19B_ADC0_X13                         (1L)        
#define PINMUX_PA19B_ADC0_X13                      ((PIN_PA19B_ADC0_X13 << 16) | MUX_PA19B_ADC0_X13)
#define PORT_PA19B_ADC0_X13                        ((1UL) << 19)

#define PIN_PA19B_ADC0_Y13                         (19L)        
#define MUX_PA19B_ADC0_Y13                         (1L)        
#define PINMUX_PA19B_ADC0_Y13                      ((PIN_PA19B_ADC0_Y13 << 16) | MUX_PA19B_ADC0_Y13)
#define PORT_PA19B_ADC0_Y13                        ((1UL) << 19)

#define PIN_PA20B_ADC0_X14                         (20L)        
#define MUX_PA20B_ADC0_X14                         (1L)        
#define PINMUX_PA20B_ADC0_X14                      ((PIN_PA20B_ADC0_X14 << 16) | MUX_PA20B_ADC0_X14)
#define PORT_PA20B_ADC0_X14                        ((1UL) << 20)

#define PIN_PA20B_ADC0_Y14                         (20L)        
#define MUX_PA20B_ADC0_Y14                         (1L)        
#define PINMUX_PA20B_ADC0_Y14                      ((PIN_PA20B_ADC0_Y14 << 16) | MUX_PA20B_ADC0_Y14)
#define PORT_PA20B_ADC0_Y14                        ((1UL) << 20)

#define PIN_PA21B_ADC0_X15                         (21L)        
#define MUX_PA21B_ADC0_X15                         (1L)        
#define PINMUX_PA21B_ADC0_X15                      ((PIN_PA21B_ADC0_X15 << 16) | MUX_PA21B_ADC0_X15)
#define PORT_PA21B_ADC0_X15                        ((1UL) << 21)

#define PIN_PA21B_ADC0_Y15                         (21L)        
#define MUX_PA21B_ADC0_Y15                         (1L)        
#define PINMUX_PA21B_ADC0_Y15                      ((PIN_PA21B_ADC0_Y15 << 16) | MUX_PA21B_ADC0_Y15)
#define PORT_PA21B_ADC0_Y15                        ((1UL) << 21)

#define PIN_PA22B_ADC0_X16                         (22L)        
#define MUX_PA22B_ADC0_X16                         (1L)        
#define PINMUX_PA22B_ADC0_X16                      ((PIN_PA22B_ADC0_X16 << 16) | MUX_PA22B_ADC0_X16)
#define PORT_PA22B_ADC0_X16                        ((1UL) << 22)

#define PIN_PA22B_ADC0_Y16                         (22L)        
#define MUX_PA22B_ADC0_Y16                         (1L)        
#define PINMUX_PA22B_ADC0_Y16                      ((PIN_PA22B_ADC0_Y16 << 16) | MUX_PA22B_ADC0_Y16)
#define PORT_PA22B_ADC0_Y16                        ((1UL) << 22)

#define PIN_PA23B_ADC0_X17                         (23L)        
#define MUX_PA23B_ADC0_X17                         (1L)        
#define PINMUX_PA23B_ADC0_X17                      ((PIN_PA23B_ADC0_X17 << 16) | MUX_PA23B_ADC0_X17)
#define PORT_PA23B_ADC0_X17                        ((1UL) << 23)

#define PIN_PA23B_ADC0_Y17                         (23L)        
#define MUX_PA23B_ADC0_Y17                         (1L)        
#define PINMUX_PA23B_ADC0_Y17                      ((PIN_PA23B_ADC0_Y17 << 16) | MUX_PA23B_ADC0_Y17)
#define PORT_PA23B_ADC0_Y17                        ((1UL) << 23)

#define PIN_PA27B_ADC0_X18                         (27L)        
#define MUX_PA27B_ADC0_X18                         (1L)        
#define PINMUX_PA27B_ADC0_X18                      ((PIN_PA27B_ADC0_X18 << 16) | MUX_PA27B_ADC0_X18)
#define PORT_PA27B_ADC0_X18                        ((1UL) << 27)

#define PIN_PA27B_ADC0_Y18                         (27L)        
#define MUX_PA27B_ADC0_Y18                         (1L)        
#define PINMUX_PA27B_ADC0_Y18                      ((PIN_PA27B_ADC0_Y18 << 16) | MUX_PA27B_ADC0_Y18)
#define PORT_PA27B_ADC0_Y18                        ((1UL) << 27)

#define PIN_PA30B_ADC0_X19                         (30L)        
#define MUX_PA30B_ADC0_X19                         (1L)        
#define PINMUX_PA30B_ADC0_X19                      ((PIN_PA30B_ADC0_X19 << 16) | MUX_PA30B_ADC0_X19)
#define PORT_PA30B_ADC0_X19                        ((1UL) << 30)

#define PIN_PA30B_ADC0_Y19                         (30L)        
#define MUX_PA30B_ADC0_Y19                         (1L)        
#define PINMUX_PA30B_ADC0_Y19                      ((PIN_PA30B_ADC0_Y19 << 16) | MUX_PA30B_ADC0_Y19)
#define PORT_PA30B_ADC0_Y19                        ((1UL) << 30)

#define PIN_PB02B_ADC0_X20                         (34L)        
#define MUX_PB02B_ADC0_X20                         (1L)        
#define PINMUX_PB02B_ADC0_X20                      ((PIN_PB02B_ADC0_X20 << 16) | MUX_PB02B_ADC0_X20)
#define PORT_PB02B_ADC0_X20                        ((1UL) << 2)

#define PIN_PB02B_ADC0_Y20                         (34L)        
#define MUX_PB02B_ADC0_Y20                         (1L)        
#define PINMUX_PB02B_ADC0_Y20                      ((PIN_PB02B_ADC0_Y20 << 16) | MUX_PB02B_ADC0_Y20)
#define PORT_PB02B_ADC0_Y20                        ((1UL) << 2)

#define PIN_PB03B_ADC0_X21                         (35L)        
#define MUX_PB03B_ADC0_X21                         (1L)        
#define PINMUX_PB03B_ADC0_X21                      ((PIN_PB03B_ADC0_X21 << 16) | MUX_PB03B_ADC0_X21)
#define PORT_PB03B_ADC0_X21                        ((1UL) << 3)

#define PIN_PB03B_ADC0_Y21                         (35L)        
#define MUX_PB03B_ADC0_Y21                         (1L)        
#define PINMUX_PB03B_ADC0_Y21                      ((PIN_PB03B_ADC0_Y21 << 16) | MUX_PB03B_ADC0_Y21)
#define PORT_PB03B_ADC0_Y21                        ((1UL) << 3)

#define PIN_PB04B_ADC0_X22                         (36L)        
#define MUX_PB04B_ADC0_X22                         (1L)        
#define PINMUX_PB04B_ADC0_X22                      ((PIN_PB04B_ADC0_X22 << 16) | MUX_PB04B_ADC0_X22)
#define PORT_PB04B_ADC0_X22                        ((1UL) << 4)

#define PIN_PB04B_ADC0_Y22                         (36L)        
#define MUX_PB04B_ADC0_Y22                         (1L)        
#define PINMUX_PB04B_ADC0_Y22                      ((PIN_PB04B_ADC0_Y22 << 16) | MUX_PB04B_ADC0_Y22)
#define PORT_PB04B_ADC0_Y22                        ((1UL) << 4)

#define PIN_PB05B_ADC0_X23                         (37L)        
#define MUX_PB05B_ADC0_X23                         (1L)        
#define PINMUX_PB05B_ADC0_X23                      ((PIN_PB05B_ADC0_X23 << 16) | MUX_PB05B_ADC0_X23)
#define PORT_PB05B_ADC0_X23                        ((1UL) << 5)

#define PIN_PB05B_ADC0_Y23                         (37L)        
#define MUX_PB05B_ADC0_Y23                         (1L)        
#define PINMUX_PB05B_ADC0_Y23                      ((PIN_PB05B_ADC0_Y23 << 16) | MUX_PB05B_ADC0_Y23)
#define PORT_PB05B_ADC0_Y23                        ((1UL) << 5)

#define PIN_PB06B_ADC0_X24                         (38L)        
#define MUX_PB06B_ADC0_X24                         (1L)        
#define PINMUX_PB06B_ADC0_X24                      ((PIN_PB06B_ADC0_X24 << 16) | MUX_PB06B_ADC0_X24)
#define PORT_PB06B_ADC0_X24                        ((1UL) << 6)

#define PIN_PB06B_ADC0_Y24                         (38L)        
#define MUX_PB06B_ADC0_Y24                         (1L)        
#define PINMUX_PB06B_ADC0_Y24                      ((PIN_PB06B_ADC0_Y24 << 16) | MUX_PB06B_ADC0_Y24)
#define PORT_PB06B_ADC0_Y24                        ((1UL) << 6)

#define PIN_PB07B_ADC0_X25                         (39L)        
#define MUX_PB07B_ADC0_X25                         (1L)        
#define PINMUX_PB07B_ADC0_X25                      ((PIN_PB07B_ADC0_X25 << 16) | MUX_PB07B_ADC0_X25)
#define PORT_PB07B_ADC0_X25                        ((1UL) << 7)

#define PIN_PB07B_ADC0_Y25                         (39L)        
#define MUX_PB07B_ADC0_Y25                         (1L)        
#define PINMUX_PB07B_ADC0_Y25                      ((PIN_PB07B_ADC0_Y25 << 16) | MUX_PB07B_ADC0_Y25)
#define PORT_PB07B_ADC0_Y25                        ((1UL) << 7)

#define PIN_PB12B_ADC0_X26                         (44L)        
#define MUX_PB12B_ADC0_X26                         (1L)        
#define PINMUX_PB12B_ADC0_X26                      ((PIN_PB12B_ADC0_X26 << 16) | MUX_PB12B_ADC0_X26)
#define PORT_PB12B_ADC0_X26                        ((1UL) << 12)

#define PIN_PB12B_ADC0_Y26                         (44L)        
#define MUX_PB12B_ADC0_Y26                         (1L)        
#define PINMUX_PB12B_ADC0_Y26                      ((PIN_PB12B_ADC0_Y26 << 16) | MUX_PB12B_ADC0_Y26)
#define PORT_PB12B_ADC0_Y26                        ((1UL) << 12)

#define PIN_PB13B_ADC0_X27                         (45L)        
#define MUX_PB13B_ADC0_X27                         (1L)        
#define PINMUX_PB13B_ADC0_X27                      ((PIN_PB13B_ADC0_X27 << 16) | MUX_PB13B_ADC0_X27)
#define PORT_PB13B_ADC0_X27                        ((1UL) << 13)

#define PIN_PB13B_ADC0_Y27                         (45L)        
#define MUX_PB13B_ADC0_Y27                         (1L)        
#define PINMUX_PB13B_ADC0_Y27                      ((PIN_PB13B_ADC0_Y27 << 16) | MUX_PB13B_ADC0_Y27)
#define PORT_PB13B_ADC0_Y27                        ((1UL) << 13)

#define PIN_PB14B_ADC0_X28                         (46L)        
#define MUX_PB14B_ADC0_X28                         (1L)        
#define PINMUX_PB14B_ADC0_X28                      ((PIN_PB14B_ADC0_X28 << 16) | MUX_PB14B_ADC0_X28)
#define PORT_PB14B_ADC0_X28                        ((1UL) << 14)

#define PIN_PB14B_ADC0_Y28                         (46L)        
#define MUX_PB14B_ADC0_Y28                         (1L)        
#define PINMUX_PB14B_ADC0_Y28                      ((PIN_PB14B_ADC0_Y28 << 16) | MUX_PB14B_ADC0_Y28)
#define PORT_PB14B_ADC0_Y28                        ((1UL) << 14)

#define PIN_PB15B_ADC0_X29                         (47L)        
#define MUX_PB15B_ADC0_X29                         (1L)        
#define PINMUX_PB15B_ADC0_X29                      ((PIN_PB15B_ADC0_X29 << 16) | MUX_PB15B_ADC0_X29)
#define PORT_PB15B_ADC0_X29                        ((1UL) << 15)

#define PIN_PB15B_ADC0_Y29                         (47L)        
#define MUX_PB15B_ADC0_Y29                         (1L)        
#define PINMUX_PB15B_ADC0_Y29                      ((PIN_PB15B_ADC0_Y29 << 16) | MUX_PB15B_ADC0_Y29)
#define PORT_PB15B_ADC0_Y29                        ((1UL) << 15)

#define PIN_PB00B_ADC0_X30                         (32L)        
#define MUX_PB00B_ADC0_X30                         (1L)        
#define PINMUX_PB00B_ADC0_X30                      ((PIN_PB00B_ADC0_X30 << 16) | MUX_PB00B_ADC0_X30)
#define PORT_PB00B_ADC0_X30                        ((1UL) << 0)

#define PIN_PB00B_ADC0_Y30                         (32L)        
#define MUX_PB00B_ADC0_Y30                         (1L)        
#define PINMUX_PB00B_ADC0_Y30                      ((PIN_PB00B_ADC0_Y30 << 16) | MUX_PB00B_ADC0_Y30)
#define PORT_PB00B_ADC0_Y30                        ((1UL) << 0)

#define PIN_PB01B_ADC0_X31                         (33L)        
#define MUX_PB01B_ADC0_X31                         (1L)        
#define PINMUX_PB01B_ADC0_X31                      ((PIN_PB01B_ADC0_X31 << 16) | MUX_PB01B_ADC0_X31)
#define PORT_PB01B_ADC0_X31                        ((1UL) << 1)

#define PIN_PB01B_ADC0_Y31                         (33L)        
#define MUX_PB01B_ADC0_Y31                         (1L)        
#define PINMUX_PB01B_ADC0_Y31                      ((PIN_PB01B_ADC0_Y31 << 16) | MUX_PB01B_ADC0_Y31)
#define PORT_PB01B_ADC0_Y31                        ((1UL) << 1)

/* ========== PORT definition for ADC1 peripheral ========== */
#define PIN_PB08B_ADC1_AIN0                        (40L)        
#define MUX_PB08B_ADC1_AIN0                        (1L)        
#define PINMUX_PB08B_ADC1_AIN0                     ((PIN_PB08B_ADC1_AIN0 << 16) | MUX_PB08B_ADC1_AIN0)
#define PORT_PB08B_ADC1_AIN0                       ((1UL) << 8)

#define PIN_PB09B_ADC1_AIN1                        (41L)        
#define MUX_PB09B_ADC1_AIN1                        (1L)        
#define PINMUX_PB09B_ADC1_AIN1                     ((PIN_PB09B_ADC1_AIN1 << 16) | MUX_PB09B_ADC1_AIN1)
#define PORT_PB09B_ADC1_AIN1                       ((1UL) << 9)

#define PIN_PA08B_ADC1_AIN2                        (8L)         
#define MUX_PA08B_ADC1_AIN2                        (1L)        
#define PINMUX_PA08B_ADC1_AIN2                     ((PIN_PA08B_ADC1_AIN2 << 16) | MUX_PA08B_ADC1_AIN2)
#define PORT_PA08B_ADC1_AIN2                       ((1UL) << 8)

#define PIN_PA09B_ADC1_AIN3                        (9L)         
#define MUX_PA09B_ADC1_AIN3                        (1L)        
#define PINMUX_PA09B_ADC1_AIN3                     ((PIN_PA09B_ADC1_AIN3 << 16) | MUX_PA09B_ADC1_AIN3)
#define PORT_PA09B_ADC1_AIN3                       ((1UL) << 9)

#define PIN_PC02B_ADC1_AIN4                        (66L)        
#define MUX_PC02B_ADC1_AIN4                        (1L)        
#define PINMUX_PC02B_ADC1_AIN4                     ((PIN_PC02B_ADC1_AIN4 << 16) | MUX_PC02B_ADC1_AIN4)
#define PORT_PC02B_ADC1_AIN4                       ((1UL) << 2)

#define PIN_PC03B_ADC1_AIN5                        (67L)        
#define MUX_PC03B_ADC1_AIN5                        (1L)        
#define PINMUX_PC03B_ADC1_AIN5                     ((PIN_PC03B_ADC1_AIN5 << 16) | MUX_PC03B_ADC1_AIN5)
#define PORT_PC03B_ADC1_AIN5                       ((1UL) << 3)

#define PIN_PB04B_ADC1_AIN6                        (36L)        
#define MUX_PB04B_ADC1_AIN6                        (1L)        
#define PINMUX_PB04B_ADC1_AIN6                     ((PIN_PB04B_ADC1_AIN6 << 16) | MUX_PB04B_ADC1_AIN6)
#define PORT_PB04B_ADC1_AIN6                       ((1UL) << 4)

#define PIN_PB05B_ADC1_AIN7                        (37L)        
#define MUX_PB05B_ADC1_AIN7                        (1L)        
#define PINMUX_PB05B_ADC1_AIN7                     ((PIN_PB05B_ADC1_AIN7 << 16) | MUX_PB05B_ADC1_AIN7)
#define PORT_PB05B_ADC1_AIN7                       ((1UL) << 5)

#define PIN_PB06B_ADC1_AIN8                        (38L)        
#define MUX_PB06B_ADC1_AIN8                        (1L)        
#define PINMUX_PB06B_ADC1_AIN8                     ((PIN_PB06B_ADC1_AIN8 << 16) | MUX_PB06B_ADC1_AIN8)
#define PORT_PB06B_ADC1_AIN8                       ((1UL) << 6)

#define PIN_PB07B_ADC1_AIN9                        (39L)        
#define MUX_PB07B_ADC1_AIN9                        (1L)        
#define PINMUX_PB07B_ADC1_AIN9                     ((PIN_PB07B_ADC1_AIN9 << 16) | MUX_PB07B_ADC1_AIN9)
#define PORT_PB07B_ADC1_AIN9                       ((1UL) << 7)

#define PIN_PC00B_ADC1_AIN10                       (64L)        
#define MUX_PC00B_ADC1_AIN10                       (1L)        
#define PINMUX_PC00B_ADC1_AIN10                    ((PIN_PC00B_ADC1_AIN10 << 16) | MUX_PC00B_ADC1_AIN10)
#define PORT_PC00B_ADC1_AIN10                      ((1UL) << 0)

#define PIN_PC01B_ADC1_AIN11                       (65L)        
#define MUX_PC01B_ADC1_AIN11                       (1L)        
#define PINMUX_PC01B_ADC1_AIN11                    ((PIN_PC01B_ADC1_AIN11 << 16) | MUX_PC01B_ADC1_AIN11)
#define PORT_PC01B_ADC1_AIN11                      ((1UL) << 1)

#define PIN_PC30B_ADC1_AIN12                       (94L)        
#define MUX_PC30B_ADC1_AIN12                       (1L)        
#define PINMUX_PC30B_ADC1_AIN12                    ((PIN_PC30B_ADC1_AIN12 << 16) | MUX_PC30B_ADC1_AIN12)
#define PORT_PC30B_ADC1_AIN12                      ((1UL) << 30)

#define PIN_PC31B_ADC1_AIN13                       (95L)        
#define MUX_PC31B_ADC1_AIN13                       (1L)        
#define PINMUX_PC31B_ADC1_AIN13                    ((PIN_PC31B_ADC1_AIN13 << 16) | MUX_PC31B_ADC1_AIN13)
#define PORT_PC31B_ADC1_AIN13                      ((1UL) << 31)

#define PIN_PD00B_ADC1_AIN14                       (96L)        
#define MUX_PD00B_ADC1_AIN14                       (1L)        
#define PINMUX_PD00B_ADC1_AIN14                    ((PIN_PD00B_ADC1_AIN14 << 16) | MUX_PD00B_ADC1_AIN14)
#define PORT_PD00B_ADC1_AIN14                      ((1UL) << 0)

#define PIN_PD01B_ADC1_AIN15                       (97L)        
#define MUX_PD01B_ADC1_AIN15                       (1L)        
#define PINMUX_PD01B_ADC1_AIN15                    ((PIN_PD01B_ADC1_AIN15 << 16) | MUX_PD01B_ADC1_AIN15)
#define PORT_PD01B_ADC1_AIN15                      ((1UL) << 1)

/* ========== PORT definition for CAN0 peripheral ========== */
#define PIN_PA23I_CAN0_RX                          (23L)        
#define MUX_PA23I_CAN0_RX                          (8L)        
#define PINMUX_PA23I_CAN0_RX                       ((PIN_PA23I_CAN0_RX << 16) | MUX_PA23I_CAN0_RX)
#define PORT_PA23I_CAN0_RX                         ((1UL) << 23)

#define PIN_PA25I_CAN0_RX                          (25L)        
#define MUX_PA25I_CAN0_RX                          (8L)        
#define PINMUX_PA25I_CAN0_RX                       ((PIN_PA25I_CAN0_RX << 16) | MUX_PA25I_CAN0_RX)
#define PORT_PA25I_CAN0_RX                         ((1UL) << 25)

#define PIN_PA22I_CAN0_TX                          (22L)        
#define MUX_PA22I_CAN0_TX                          (8L)        
#define PINMUX_PA22I_CAN0_TX                       ((PIN_PA22I_CAN0_TX << 16) | MUX_PA22I_CAN0_TX)
#define PORT_PA22I_CAN0_TX                         ((1UL) << 22)

#define PIN_PA24I_CAN0_TX                          (24L)        
#define MUX_PA24I_CAN0_TX                          (8L)        
#define PINMUX_PA24I_CAN0_TX                       ((PIN_PA24I_CAN0_TX << 16) | MUX_PA24I_CAN0_TX)
#define PORT_PA24I_CAN0_TX                         ((1UL) << 24)

/* ========== PORT definition for CAN1 peripheral ========== */
#define PIN_PB13H_CAN1_RX                          (45L)        
#define MUX_PB13H_CAN1_RX                          (7L)        
#define PINMUX_PB13H_CAN1_RX                       ((PIN_PB13H_CAN1_RX << 16) | MUX_PB13H_CAN1_RX)
#define PORT_PB13H_CAN1_RX                         ((1UL) << 13)

#define PIN_PB15H_CAN1_RX                          (47L)        
#define MUX_PB15H_CAN1_RX                          (7L)        
#define PINMUX_PB15H_CAN1_RX                       ((PIN_PB15H_CAN1_RX << 16) | MUX_PB15H_CAN1_RX)
#define PORT_PB15H_CAN1_RX                         ((1UL) << 15)

#define PIN_PB12H_CAN1_TX                          (44L)        
#define MUX_PB12H_CAN1_TX                          (7L)        
#define PINMUX_PB12H_CAN1_TX                       ((PIN_PB12H_CAN1_TX << 16) | MUX_PB12H_CAN1_TX)
#define PORT_PB12H_CAN1_TX                         ((1UL) << 12)

#define PIN_PB14H_CAN1_TX                          (46L)        
#define MUX_PB14H_CAN1_TX                          (7L)        
#define PINMUX_PB14H_CAN1_TX                       ((PIN_PB14H_CAN1_TX << 16) | MUX_PB14H_CAN1_TX)
#define PORT_PB14H_CAN1_TX                         ((1UL) << 14)

/* ========== PORT definition for CCL peripheral ========== */
#define PIN_PA04N_CCL_IN0                          (4L)         
#define MUX_PA04N_CCL_IN0                          (13L)       
#define PINMUX_PA04N_CCL_IN0                       ((PIN_PA04N_CCL_IN0 << 16) | MUX_PA04N_CCL_IN0)
#define PORT_PA04N_CCL_IN0                         ((1UL) << 4)

#define PIN_PA16N_CCL_IN0                          (16L)        
#define MUX_PA16N_CCL_IN0                          (13L)       
#define PINMUX_PA16N_CCL_IN0                       ((PIN_PA16N_CCL_IN0 << 16) | MUX_PA16N_CCL_IN0)
#define PORT_PA16N_CCL_IN0                         ((1UL) << 16)

#define PIN_PB22N_CCL_IN0                          (54L)        
#define MUX_PB22N_CCL_IN0                          (13L)       
#define PINMUX_PB22N_CCL_IN0                       ((PIN_PB22N_CCL_IN0 << 16) | MUX_PB22N_CCL_IN0)
#define PORT_PB22N_CCL_IN0                         ((1UL) << 22)

#define PIN_PA05N_CCL_IN1                          (5L)         
#define MUX_PA05N_CCL_IN1                          (13L)       
#define PINMUX_PA05N_CCL_IN1                       ((PIN_PA05N_CCL_IN1 << 16) | MUX_PA05N_CCL_IN1)
#define PORT_PA05N_CCL_IN1                         ((1UL) << 5)

#define PIN_PA17N_CCL_IN1                          (17L)        
#define MUX_PA17N_CCL_IN1                          (13L)       
#define PINMUX_PA17N_CCL_IN1                       ((PIN_PA17N_CCL_IN1 << 16) | MUX_PA17N_CCL_IN1)
#define PORT_PA17N_CCL_IN1                         ((1UL) << 17)

#define PIN_PB00N_CCL_IN1                          (32L)        
#define MUX_PB00N_CCL_IN1                          (13L)       
#define PINMUX_PB00N_CCL_IN1                       ((PIN_PB00N_CCL_IN1 << 16) | MUX_PB00N_CCL_IN1)
#define PORT_PB00N_CCL_IN1                         ((1UL) << 0)

#define PIN_PA06N_CCL_IN2                          (6L)         
#define MUX_PA06N_CCL_IN2                          (13L)       
#define PINMUX_PA06N_CCL_IN2                       ((PIN_PA06N_CCL_IN2 << 16) | MUX_PA06N_CCL_IN2)
#define PORT_PA06N_CCL_IN2                         ((1UL) << 6)

#define PIN_PA18N_CCL_IN2                          (18L)        
#define MUX_PA18N_CCL_IN2                          (13L)       
#define PINMUX_PA18N_CCL_IN2                       ((PIN_PA18N_CCL_IN2 << 16) | MUX_PA18N_CCL_IN2)
#define PORT_PA18N_CCL_IN2                         ((1UL) << 18)

#define PIN_PB01N_CCL_IN2                          (33L)        
#define MUX_PB01N_CCL_IN2                          (13L)       
#define PINMUX_PB01N_CCL_IN2                       ((PIN_PB01N_CCL_IN2 << 16) | MUX_PB01N_CCL_IN2)
#define PORT_PB01N_CCL_IN2                         ((1UL) << 1)

#define PIN_PA08N_CCL_IN3                          (8L)         
#define MUX_PA08N_CCL_IN3                          (13L)       
#define PINMUX_PA08N_CCL_IN3                       ((PIN_PA08N_CCL_IN3 << 16) | MUX_PA08N_CCL_IN3)
#define PORT_PA08N_CCL_IN3                         ((1UL) << 8)

#define PIN_PA30N_CCL_IN3                          (30L)        
#define MUX_PA30N_CCL_IN3                          (13L)       
#define PINMUX_PA30N_CCL_IN3                       ((PIN_PA30N_CCL_IN3 << 16) | MUX_PA30N_CCL_IN3)
#define PORT_PA30N_CCL_IN3                         ((1UL) << 30)

#define PIN_PA09N_CCL_IN4                          (9L)         
#define MUX_PA09N_CCL_IN4                          (13L)       
#define PINMUX_PA09N_CCL_IN4                       ((PIN_PA09N_CCL_IN4 << 16) | MUX_PA09N_CCL_IN4)
#define PORT_PA09N_CCL_IN4                         ((1UL) << 9)

#define PIN_PC27N_CCL_IN4                          (91L)        
#define MUX_PC27N_CCL_IN4                          (13L)       
#define PINMUX_PC27N_CCL_IN4                       ((PIN_PC27N_CCL_IN4 << 16) | MUX_PC27N_CCL_IN4)
#define PORT_PC27N_CCL_IN4                         ((1UL) << 27)

#define PIN_PA10N_CCL_IN5                          (10L)        
#define MUX_PA10N_CCL_IN5                          (13L)       
#define PINMUX_PA10N_CCL_IN5                       ((PIN_PA10N_CCL_IN5 << 16) | MUX_PA10N_CCL_IN5)
#define PORT_PA10N_CCL_IN5                         ((1UL) << 10)

#define PIN_PC28N_CCL_IN5                          (92L)        
#define MUX_PC28N_CCL_IN5                          (13L)       
#define PINMUX_PC28N_CCL_IN5                       ((PIN_PC28N_CCL_IN5 << 16) | MUX_PC28N_CCL_IN5)
#define PORT_PC28N_CCL_IN5                         ((1UL) << 28)

#define PIN_PA22N_CCL_IN6                          (22L)        
#define MUX_PA22N_CCL_IN6                          (13L)       
#define PINMUX_PA22N_CCL_IN6                       ((PIN_PA22N_CCL_IN6 << 16) | MUX_PA22N_CCL_IN6)
#define PORT_PA22N_CCL_IN6                         ((1UL) << 22)

#define PIN_PB06N_CCL_IN6                          (38L)        
#define MUX_PB06N_CCL_IN6                          (13L)       
#define PINMUX_PB06N_CCL_IN6                       ((PIN_PB06N_CCL_IN6 << 16) | MUX_PB06N_CCL_IN6)
#define PORT_PB06N_CCL_IN6                         ((1UL) << 6)

#define PIN_PA23N_CCL_IN7                          (23L)        
#define MUX_PA23N_CCL_IN7                          (13L)       
#define PINMUX_PA23N_CCL_IN7                       ((PIN_PA23N_CCL_IN7 << 16) | MUX_PA23N_CCL_IN7)
#define PORT_PA23N_CCL_IN7                         ((1UL) << 23)

#define PIN_PB07N_CCL_IN7                          (39L)        
#define MUX_PB07N_CCL_IN7                          (13L)       
#define PINMUX_PB07N_CCL_IN7                       ((PIN_PB07N_CCL_IN7 << 16) | MUX_PB07N_CCL_IN7)
#define PORT_PB07N_CCL_IN7                         ((1UL) << 7)

#define PIN_PA24N_CCL_IN8                          (24L)        
#define MUX_PA24N_CCL_IN8                          (13L)       
#define PINMUX_PA24N_CCL_IN8                       ((PIN_PA24N_CCL_IN8 << 16) | MUX_PA24N_CCL_IN8)
#define PORT_PA24N_CCL_IN8                         ((1UL) << 24)

#define PIN_PB08N_CCL_IN8                          (40L)        
#define MUX_PB08N_CCL_IN8                          (13L)       
#define PINMUX_PB08N_CCL_IN8                       ((PIN_PB08N_CCL_IN8 << 16) | MUX_PB08N_CCL_IN8)
#define PORT_PB08N_CCL_IN8                         ((1UL) << 8)

#define PIN_PB14N_CCL_IN9                          (46L)        
#define MUX_PB14N_CCL_IN9                          (13L)       
#define PINMUX_PB14N_CCL_IN9                       ((PIN_PB14N_CCL_IN9 << 16) | MUX_PB14N_CCL_IN9)
#define PORT_PB14N_CCL_IN9                         ((1UL) << 14)

#define PIN_PC20N_CCL_IN9                          (84L)        
#define MUX_PC20N_CCL_IN9                          (13L)       
#define PINMUX_PC20N_CCL_IN9                       ((PIN_PC20N_CCL_IN9 << 16) | MUX_PC20N_CCL_IN9)
#define PORT_PC20N_CCL_IN9                         ((1UL) << 20)

#define PIN_PB15N_CCL_IN10                         (47L)        
#define MUX_PB15N_CCL_IN10                         (13L)       
#define PINMUX_PB15N_CCL_IN10                      ((PIN_PB15N_CCL_IN10 << 16) | MUX_PB15N_CCL_IN10)
#define PORT_PB15N_CCL_IN10                        ((1UL) << 15)

#define PIN_PC21N_CCL_IN10                         (85L)        
#define MUX_PC21N_CCL_IN10                         (13L)       
#define PINMUX_PC21N_CCL_IN10                      ((PIN_PC21N_CCL_IN10 << 16) | MUX_PC21N_CCL_IN10)
#define PORT_PC21N_CCL_IN10                        ((1UL) << 21)

#define PIN_PB10N_CCL_IN11                         (42L)        
#define MUX_PB10N_CCL_IN11                         (13L)       
#define PINMUX_PB10N_CCL_IN11                      ((PIN_PB10N_CCL_IN11 << 16) | MUX_PB10N_CCL_IN11)
#define PORT_PB10N_CCL_IN11                        ((1UL) << 10)

#define PIN_PB16N_CCL_IN11                         (48L)        
#define MUX_PB16N_CCL_IN11                         (13L)       
#define PINMUX_PB16N_CCL_IN11                      ((PIN_PB16N_CCL_IN11 << 16) | MUX_PB16N_CCL_IN11)
#define PORT_PB16N_CCL_IN11                        ((1UL) << 16)

#define PIN_PA07N_CCL_OUT0                         (7L)         
#define MUX_PA07N_CCL_OUT0                         (13L)       
#define PINMUX_PA07N_CCL_OUT0                      ((PIN_PA07N_CCL_OUT0 << 16) | MUX_PA07N_CCL_OUT0)
#define PORT_PA07N_CCL_OUT0                        ((1UL) << 7)

#define PIN_PA19N_CCL_OUT0                         (19L)        
#define MUX_PA19N_CCL_OUT0                         (13L)       
#define PINMUX_PA19N_CCL_OUT0                      ((PIN_PA19N_CCL_OUT0 << 16) | MUX_PA19N_CCL_OUT0)
#define PORT_PA19N_CCL_OUT0                        ((1UL) << 19)

#define PIN_PB02N_CCL_OUT0                         (34L)        
#define MUX_PB02N_CCL_OUT0                         (13L)       
#define PINMUX_PB02N_CCL_OUT0                      ((PIN_PB02N_CCL_OUT0 << 16) | MUX_PB02N_CCL_OUT0)
#define PORT_PB02N_CCL_OUT0                        ((1UL) << 2)

#define PIN_PB23N_CCL_OUT0                         (55L)        
#define MUX_PB23N_CCL_OUT0                         (13L)       
#define PINMUX_PB23N_CCL_OUT0                      ((PIN_PB23N_CCL_OUT0 << 16) | MUX_PB23N_CCL_OUT0)
#define PORT_PB23N_CCL_OUT0                        ((1UL) << 23)

#define PIN_PA11N_CCL_OUT1                         (11L)        
#define MUX_PA11N_CCL_OUT1                         (13L)       
#define PINMUX_PA11N_CCL_OUT1                      ((PIN_PA11N_CCL_OUT1 << 16) | MUX_PA11N_CCL_OUT1)
#define PORT_PA11N_CCL_OUT1                        ((1UL) << 11)

#define PIN_PA31N_CCL_OUT1                         (31L)        
#define MUX_PA31N_CCL_OUT1                         (13L)       
#define PINMUX_PA31N_CCL_OUT1                      ((PIN_PA31N_CCL_OUT1 << 16) | MUX_PA31N_CCL_OUT1)
#define PORT_PA31N_CCL_OUT1                        ((1UL) << 31)

#define PIN_PB11N_CCL_OUT1                         (43L)        
#define MUX_PB11N_CCL_OUT1                         (13L)       
#define PINMUX_PB11N_CCL_OUT1                      ((PIN_PB11N_CCL_OUT1 << 16) | MUX_PB11N_CCL_OUT1)
#define PORT_PB11N_CCL_OUT1                        ((1UL) << 11)

#define PIN_PA25N_CCL_OUT2                         (25L)        
#define MUX_PA25N_CCL_OUT2                         (13L)       
#define PINMUX_PA25N_CCL_OUT2                      ((PIN_PA25N_CCL_OUT2 << 16) | MUX_PA25N_CCL_OUT2)
#define PORT_PA25N_CCL_OUT2                        ((1UL) << 25)

#define PIN_PB09N_CCL_OUT2                         (41L)        
#define MUX_PB09N_CCL_OUT2                         (13L)       
#define PINMUX_PB09N_CCL_OUT2                      ((PIN_PB09N_CCL_OUT2 << 16) | MUX_PB09N_CCL_OUT2)
#define PORT_PB09N_CCL_OUT2                        ((1UL) << 9)

#define PIN_PB17N_CCL_OUT3                         (49L)        
#define MUX_PB17N_CCL_OUT3                         (13L)       
#define PINMUX_PB17N_CCL_OUT3                      ((PIN_PB17N_CCL_OUT3 << 16) | MUX_PB17N_CCL_OUT3)
#define PORT_PB17N_CCL_OUT3                        ((1UL) << 17)

/* ========== PORT definition for DAC peripheral ========== */
#define PIN_PA02B_DAC_VOUT0                        (2L)         
#define MUX_PA02B_DAC_VOUT0                        (1L)        
#define PINMUX_PA02B_DAC_VOUT0                     ((PIN_PA02B_DAC_VOUT0 << 16) | MUX_PA02B_DAC_VOUT0)
#define PORT_PA02B_DAC_VOUT0                       ((1UL) << 2)

#define PIN_PA05B_DAC_VOUT1                        (5L)         
#define MUX_PA05B_DAC_VOUT1                        (1L)        
#define PINMUX_PA05B_DAC_VOUT1                     ((PIN_PA05B_DAC_VOUT1 << 16) | MUX_PA05B_DAC_VOUT1)
#define PORT_PA05B_DAC_VOUT1                       ((1UL) << 5)

/* ========== PORT definition for EIC peripheral ========== */
#define PIN_PA00A_EIC_EXTINT0                      (0L)         
#define MUX_PA00A_EIC_EXTINT0                      (0L)        
#define PINMUX_PA00A_EIC_EXTINT0                   ((PIN_PA00A_EIC_EXTINT0 << 16) | MUX_PA00A_EIC_EXTINT0)
#define PORT_PA00A_EIC_EXTINT0                     ((1UL) << 0)
#define PIN_PA00A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PA00 External Interrupt Line */

#define PIN_PA16A_EIC_EXTINT0                      (16L)        
#define MUX_PA16A_EIC_EXTINT0                      (0L)        
#define PINMUX_PA16A_EIC_EXTINT0                   ((PIN_PA16A_EIC_EXTINT0 << 16) | MUX_PA16A_EIC_EXTINT0)
#define PORT_PA16A_EIC_EXTINT0                     ((1UL) << 16)
#define PIN_PA16A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PA16 External Interrupt Line */

#define PIN_PB00A_EIC_EXTINT0                      (32L)        
#define MUX_PB00A_EIC_EXTINT0                      (0L)        
#define PINMUX_PB00A_EIC_EXTINT0                   ((PIN_PB00A_EIC_EXTINT0 << 16) | MUX_PB00A_EIC_EXTINT0)
#define PORT_PB00A_EIC_EXTINT0                     ((1UL) << 0)
#define PIN_PB00A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PB00 External Interrupt Line */

#define PIN_PB16A_EIC_EXTINT0                      (48L)        
#define MUX_PB16A_EIC_EXTINT0                      (0L)        
#define PINMUX_PB16A_EIC_EXTINT0                   ((PIN_PB16A_EIC_EXTINT0 << 16) | MUX_PB16A_EIC_EXTINT0)
#define PORT_PB16A_EIC_EXTINT0                     ((1UL) << 16)
#define PIN_PB16A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PB16 External Interrupt Line */

#define PIN_PC00A_EIC_EXTINT0                      (64L)        
#define MUX_PC00A_EIC_EXTINT0                      (0L)        
#define PINMUX_PC00A_EIC_EXTINT0                   ((PIN_PC00A_EIC_EXTINT0 << 16) | MUX_PC00A_EIC_EXTINT0)
#define PORT_PC00A_EIC_EXTINT0                     ((1UL) << 0)
#define PIN_PC00A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PC00 External Interrupt Line */

#define PIN_PC16A_EIC_EXTINT0                      (80L)        
#define MUX_PC16A_EIC_EXTINT0                      (0L)        
#define PINMUX_PC16A_EIC_EXTINT0                   ((PIN_PC16A_EIC_EXTINT0 << 16) | MUX_PC16A_EIC_EXTINT0)
#define PORT_PC16A_EIC_EXTINT0                     ((1UL) << 16)
#define PIN_PC16A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PC16 External Interrupt Line */

#define PIN_PD00A_EIC_EXTINT0                      (96L)        
#define MUX_PD00A_EIC_EXTINT0                      (0L)        
#define PINMUX_PD00A_EIC_EXTINT0                   ((PIN_PD00A_EIC_EXTINT0 << 16) | MUX_PD00A_EIC_EXTINT0)
#define PORT_PD00A_EIC_EXTINT0                     ((1UL) << 0)
#define PIN_PD00A_EIC_EXTINT_NUM                   _L_(0)       /**< EIC signal: PIN_PD00 External Interrupt Line */

#define PIN_PA01A_EIC_EXTINT1                      (1L)         
#define MUX_PA01A_EIC_EXTINT1                      (0L)        
#define PINMUX_PA01A_EIC_EXTINT1                   ((PIN_PA01A_EIC_EXTINT1 << 16) | MUX_PA01A_EIC_EXTINT1)
#define PORT_PA01A_EIC_EXTINT1                     ((1UL) << 1)
#define PIN_PA01A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PA01 External Interrupt Line */

#define PIN_PA17A_EIC_EXTINT1                      (17L)        
#define MUX_PA17A_EIC_EXTINT1                      (0L)        
#define PINMUX_PA17A_EIC_EXTINT1                   ((PIN_PA17A_EIC_EXTINT1 << 16) | MUX_PA17A_EIC_EXTINT1)
#define PORT_PA17A_EIC_EXTINT1                     ((1UL) << 17)
#define PIN_PA17A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PA17 External Interrupt Line */

#define PIN_PB01A_EIC_EXTINT1                      (33L)        
#define MUX_PB01A_EIC_EXTINT1                      (0L)        
#define PINMUX_PB01A_EIC_EXTINT1                   ((PIN_PB01A_EIC_EXTINT1 << 16) | MUX_PB01A_EIC_EXTINT1)
#define PORT_PB01A_EIC_EXTINT1                     ((1UL) << 1)
#define PIN_PB01A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PB01 External Interrupt Line */

#define PIN_PB17A_EIC_EXTINT1                      (49L)        
#define MUX_PB17A_EIC_EXTINT1                      (0L)        
#define PINMUX_PB17A_EIC_EXTINT1                   ((PIN_PB17A_EIC_EXTINT1 << 16) | MUX_PB17A_EIC_EXTINT1)
#define PORT_PB17A_EIC_EXTINT1                     ((1UL) << 17)
#define PIN_PB17A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PB17 External Interrupt Line */

#define PIN_PC01A_EIC_EXTINT1                      (65L)        
#define MUX_PC01A_EIC_EXTINT1                      (0L)        
#define PINMUX_PC01A_EIC_EXTINT1                   ((PIN_PC01A_EIC_EXTINT1 << 16) | MUX_PC01A_EIC_EXTINT1)
#define PORT_PC01A_EIC_EXTINT1                     ((1UL) << 1)
#define PIN_PC01A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PC01 External Interrupt Line */

#define PIN_PC17A_EIC_EXTINT1                      (81L)        
#define MUX_PC17A_EIC_EXTINT1                      (0L)        
#define PINMUX_PC17A_EIC_EXTINT1                   ((PIN_PC17A_EIC_EXTINT1 << 16) | MUX_PC17A_EIC_EXTINT1)
#define PORT_PC17A_EIC_EXTINT1                     ((1UL) << 17)
#define PIN_PC17A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PC17 External Interrupt Line */

#define PIN_PD01A_EIC_EXTINT1                      (97L)        
#define MUX_PD01A_EIC_EXTINT1                      (0L)        
#define PINMUX_PD01A_EIC_EXTINT1                   ((PIN_PD01A_EIC_EXTINT1 << 16) | MUX_PD01A_EIC_EXTINT1)
#define PORT_PD01A_EIC_EXTINT1                     ((1UL) << 1)
#define PIN_PD01A_EIC_EXTINT_NUM                   _L_(1)       /**< EIC signal: PIN_PD01 External Interrupt Line */

#define PIN_PA02A_EIC_EXTINT2                      (2L)         
#define MUX_PA02A_EIC_EXTINT2                      (0L)        
#define PINMUX_PA02A_EIC_EXTINT2                   ((PIN_PA02A_EIC_EXTINT2 << 16) | MUX_PA02A_EIC_EXTINT2)
#define PORT_PA02A_EIC_EXTINT2                     ((1UL) << 2)
#define PIN_PA02A_EIC_EXTINT_NUM                   _L_(2)       /**< EIC signal: PIN_PA02 External Interrupt Line */

#define PIN_PA18A_EIC_EXTINT2                      (18L)        
#define MUX_PA18A_EIC_EXTINT2                      (0L)        
#define PINMUX_PA18A_EIC_EXTINT2                   ((PIN_PA18A_EIC_EXTINT2 << 16) | MUX_PA18A_EIC_EXTINT2)
#define PORT_PA18A_EIC_EXTINT2                     ((1UL) << 18)
#define PIN_PA18A_EIC_EXTINT_NUM                   _L_(2)       /**< EIC signal: PIN_PA18 External Interrupt Line */

#define PIN_PB02A_EIC_EXTINT2                      (34L)        
#define MUX_PB02A_EIC_EXTINT2                      (0L)        
#define PINMUX_PB02A_EIC_EXTINT2                   ((PIN_PB02A_EIC_EXTINT2 << 16) | MUX_PB02A_EIC_EXTINT2)
#define PORT_PB02A_EIC_EXTINT2                     ((1UL) << 2)
#define PIN_PB02A_EIC_EXTINT_NUM                   _L_(2)       /**< EIC signal: PIN_PB02 External Interrupt Line */

#define PIN_PB18A_EIC_EXTINT2                      (50L)        
#define MUX_PB18A_EIC_EXTINT2                      (0L)        
#define PINMUX_PB18A_EIC_EXTINT2                   ((PIN_PB18A_EIC_EXTINT2 << 16) | MUX_PB18A_EIC_EXTINT2)
#define PORT_PB18A_EIC_EXTINT2                     ((1UL) << 18)
#define PIN_PB18A_EIC_EXTINT_NUM                   _L_(2)       /**< EIC signal: PIN_PB18 External Interrupt Line */

#define PIN_PC02A_EIC_EXTINT2                      (66L)        
#define MUX_PC02A_EIC_EXTINT2                      (0L)        
#define PINMUX_PC02A_EIC_EXTINT2                   ((PIN_PC02A_EIC_EXTINT2 << 16) | MUX_PC02A_EIC_EXTINT2)
#define PORT_PC02A_EIC_EXTINT2                     ((1UL) << 2)
#define PIN_PC02A_EIC_EXTINT_NUM                   _L_(2)       /**< EIC signal: PIN_PC02 External Interrupt Line */

#define PIN_PC18A_EIC_EXTINT2                      (82L)        
#define MUX_PC18A_EIC_EXTINT2                      (0L)        
#define PINMUX_PC18A_EIC_EXTINT2                   ((PIN_PC18A_EIC_EXTINT2 << 16) | MUX_PC18A_EIC_EXTINT2)
#define PORT_PC18A_EIC_EXTINT2                     ((1UL) << 18)
#define PIN_PC18A_EIC_EXTINT_NUM                   _L_(2)       /**< EIC signal: PIN_PC18 External Interrupt Line */

#define PIN_PA03A_EIC_EXTINT3                      (3L)         
#define MUX_PA03A_EIC_EXTINT3                      (0L)        
#define PINMUX_PA03A_EIC_EXTINT3                   ((PIN_PA03A_EIC_EXTINT3 << 16) | MUX_PA03A_EIC_EXTINT3)
#define PORT_PA03A_EIC_EXTINT3                     ((1UL) << 3)
#define PIN_PA03A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PA03 External Interrupt Line */

#define PIN_PA19A_EIC_EXTINT3                      (19L)        
#define MUX_PA19A_EIC_EXTINT3                      (0L)        
#define PINMUX_PA19A_EIC_EXTINT3                   ((PIN_PA19A_EIC_EXTINT3 << 16) | MUX_PA19A_EIC_EXTINT3)
#define PORT_PA19A_EIC_EXTINT3                     ((1UL) << 19)
#define PIN_PA19A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PA19 External Interrupt Line */

#define PIN_PB03A_EIC_EXTINT3                      (35L)        
#define MUX_PB03A_EIC_EXTINT3                      (0L)        
#define PINMUX_PB03A_EIC_EXTINT3                   ((PIN_PB03A_EIC_EXTINT3 << 16) | MUX_PB03A_EIC_EXTINT3)
#define PORT_PB03A_EIC_EXTINT3                     ((1UL) << 3)
#define PIN_PB03A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PB03 External Interrupt Line */

#define PIN_PB19A_EIC_EXTINT3                      (51L)        
#define MUX_PB19A_EIC_EXTINT3                      (0L)        
#define PINMUX_PB19A_EIC_EXTINT3                   ((PIN_PB19A_EIC_EXTINT3 << 16) | MUX_PB19A_EIC_EXTINT3)
#define PORT_PB19A_EIC_EXTINT3                     ((1UL) << 19)
#define PIN_PB19A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PB19 External Interrupt Line */

#define PIN_PC03A_EIC_EXTINT3                      (67L)        
#define MUX_PC03A_EIC_EXTINT3                      (0L)        
#define PINMUX_PC03A_EIC_EXTINT3                   ((PIN_PC03A_EIC_EXTINT3 << 16) | MUX_PC03A_EIC_EXTINT3)
#define PORT_PC03A_EIC_EXTINT3                     ((1UL) << 3)
#define PIN_PC03A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PC03 External Interrupt Line */

#define PIN_PC19A_EIC_EXTINT3                      (83L)        
#define MUX_PC19A_EIC_EXTINT3                      (0L)        
#define PINMUX_PC19A_EIC_EXTINT3                   ((PIN_PC19A_EIC_EXTINT3 << 16) | MUX_PC19A_EIC_EXTINT3)
#define PORT_PC19A_EIC_EXTINT3                     ((1UL) << 19)
#define PIN_PC19A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PC19 External Interrupt Line */

#define PIN_PD08A_EIC_EXTINT3                      (104L)       
#define MUX_PD08A_EIC_EXTINT3                      (0L)        
#define PINMUX_PD08A_EIC_EXTINT3                   ((PIN_PD08A_EIC_EXTINT3 << 16) | MUX_PD08A_EIC_EXTINT3)
#define PORT_PD08A_EIC_EXTINT3                     ((1UL) << 8)
#define PIN_PD08A_EIC_EXTINT_NUM                   _L_(3)       /**< EIC signal: PIN_PD08 External Interrupt Line */

#define PIN_PA04A_EIC_EXTINT4                      (4L)         
#define MUX_PA04A_EIC_EXTINT4                      (0L)        
#define PINMUX_PA04A_EIC_EXTINT4                   ((PIN_PA04A_EIC_EXTINT4 << 16) | MUX_PA04A_EIC_EXTINT4)
#define PORT_PA04A_EIC_EXTINT4                     ((1UL) << 4)
#define PIN_PA04A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PA04 External Interrupt Line */

#define PIN_PA20A_EIC_EXTINT4                      (20L)        
#define MUX_PA20A_EIC_EXTINT4                      (0L)        
#define PINMUX_PA20A_EIC_EXTINT4                   ((PIN_PA20A_EIC_EXTINT4 << 16) | MUX_PA20A_EIC_EXTINT4)
#define PORT_PA20A_EIC_EXTINT4                     ((1UL) << 20)
#define PIN_PA20A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PA20 External Interrupt Line */

#define PIN_PB04A_EIC_EXTINT4                      (36L)        
#define MUX_PB04A_EIC_EXTINT4                      (0L)        
#define PINMUX_PB04A_EIC_EXTINT4                   ((PIN_PB04A_EIC_EXTINT4 << 16) | MUX_PB04A_EIC_EXTINT4)
#define PORT_PB04A_EIC_EXTINT4                     ((1UL) << 4)
#define PIN_PB04A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PB04 External Interrupt Line */

#define PIN_PB20A_EIC_EXTINT4                      (52L)        
#define MUX_PB20A_EIC_EXTINT4                      (0L)        
#define PINMUX_PB20A_EIC_EXTINT4                   ((PIN_PB20A_EIC_EXTINT4 << 16) | MUX_PB20A_EIC_EXTINT4)
#define PORT_PB20A_EIC_EXTINT4                     ((1UL) << 20)
#define PIN_PB20A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PB20 External Interrupt Line */

#define PIN_PC04A_EIC_EXTINT4                      (68L)        
#define MUX_PC04A_EIC_EXTINT4                      (0L)        
#define PINMUX_PC04A_EIC_EXTINT4                   ((PIN_PC04A_EIC_EXTINT4 << 16) | MUX_PC04A_EIC_EXTINT4)
#define PORT_PC04A_EIC_EXTINT4                     ((1UL) << 4)
#define PIN_PC04A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PC04 External Interrupt Line */

#define PIN_PC20A_EIC_EXTINT4                      (84L)        
#define MUX_PC20A_EIC_EXTINT4                      (0L)        
#define PINMUX_PC20A_EIC_EXTINT4                   ((PIN_PC20A_EIC_EXTINT4 << 16) | MUX_PC20A_EIC_EXTINT4)
#define PORT_PC20A_EIC_EXTINT4                     ((1UL) << 20)
#define PIN_PC20A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PC20 External Interrupt Line */

#define PIN_PD09A_EIC_EXTINT4                      (105L)       
#define MUX_PD09A_EIC_EXTINT4                      (0L)        
#define PINMUX_PD09A_EIC_EXTINT4                   ((PIN_PD09A_EIC_EXTINT4 << 16) | MUX_PD09A_EIC_EXTINT4)
#define PORT_PD09A_EIC_EXTINT4                     ((1UL) << 9)
#define PIN_PD09A_EIC_EXTINT_NUM                   _L_(4)       /**< EIC signal: PIN_PD09 External Interrupt Line */

#define PIN_PA05A_EIC_EXTINT5                      (5L)         
#define MUX_PA05A_EIC_EXTINT5                      (0L)        
#define PINMUX_PA05A_EIC_EXTINT5                   ((PIN_PA05A_EIC_EXTINT5 << 16) | MUX_PA05A_EIC_EXTINT5)
#define PORT_PA05A_EIC_EXTINT5                     ((1UL) << 5)
#define PIN_PA05A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PA05 External Interrupt Line */

#define PIN_PA21A_EIC_EXTINT5                      (21L)        
#define MUX_PA21A_EIC_EXTINT5                      (0L)        
#define PINMUX_PA21A_EIC_EXTINT5                   ((PIN_PA21A_EIC_EXTINT5 << 16) | MUX_PA21A_EIC_EXTINT5)
#define PORT_PA21A_EIC_EXTINT5                     ((1UL) << 21)
#define PIN_PA21A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PA21 External Interrupt Line */

#define PIN_PB05A_EIC_EXTINT5                      (37L)        
#define MUX_PB05A_EIC_EXTINT5                      (0L)        
#define PINMUX_PB05A_EIC_EXTINT5                   ((PIN_PB05A_EIC_EXTINT5 << 16) | MUX_PB05A_EIC_EXTINT5)
#define PORT_PB05A_EIC_EXTINT5                     ((1UL) << 5)
#define PIN_PB05A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PB05 External Interrupt Line */

#define PIN_PB21A_EIC_EXTINT5                      (53L)        
#define MUX_PB21A_EIC_EXTINT5                      (0L)        
#define PINMUX_PB21A_EIC_EXTINT5                   ((PIN_PB21A_EIC_EXTINT5 << 16) | MUX_PB21A_EIC_EXTINT5)
#define PORT_PB21A_EIC_EXTINT5                     ((1UL) << 21)
#define PIN_PB21A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PB21 External Interrupt Line */

#define PIN_PC05A_EIC_EXTINT5                      (69L)        
#define MUX_PC05A_EIC_EXTINT5                      (0L)        
#define PINMUX_PC05A_EIC_EXTINT5                   ((PIN_PC05A_EIC_EXTINT5 << 16) | MUX_PC05A_EIC_EXTINT5)
#define PORT_PC05A_EIC_EXTINT5                     ((1UL) << 5)
#define PIN_PC05A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PC05 External Interrupt Line */

#define PIN_PC21A_EIC_EXTINT5                      (85L)        
#define MUX_PC21A_EIC_EXTINT5                      (0L)        
#define PINMUX_PC21A_EIC_EXTINT5                   ((PIN_PC21A_EIC_EXTINT5 << 16) | MUX_PC21A_EIC_EXTINT5)
#define PORT_PC21A_EIC_EXTINT5                     ((1UL) << 21)
#define PIN_PC21A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PC21 External Interrupt Line */

#define PIN_PD10A_EIC_EXTINT5                      (106L)       
#define MUX_PD10A_EIC_EXTINT5                      (0L)        
#define PINMUX_PD10A_EIC_EXTINT5                   ((PIN_PD10A_EIC_EXTINT5 << 16) | MUX_PD10A_EIC_EXTINT5)
#define PORT_PD10A_EIC_EXTINT5                     ((1UL) << 10)
#define PIN_PD10A_EIC_EXTINT_NUM                   _L_(5)       /**< EIC signal: PIN_PD10 External Interrupt Line */

#define PIN_PA06A_EIC_EXTINT6                      (6L)         
#define MUX_PA06A_EIC_EXTINT6                      (0L)        
#define PINMUX_PA06A_EIC_EXTINT6                   ((PIN_PA06A_EIC_EXTINT6 << 16) | MUX_PA06A_EIC_EXTINT6)
#define PORT_PA06A_EIC_EXTINT6                     ((1UL) << 6)
#define PIN_PA06A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PA06 External Interrupt Line */

#define PIN_PA22A_EIC_EXTINT6                      (22L)        
#define MUX_PA22A_EIC_EXTINT6                      (0L)        
#define PINMUX_PA22A_EIC_EXTINT6                   ((PIN_PA22A_EIC_EXTINT6 << 16) | MUX_PA22A_EIC_EXTINT6)
#define PORT_PA22A_EIC_EXTINT6                     ((1UL) << 22)
#define PIN_PA22A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PA22 External Interrupt Line */

#define PIN_PB06A_EIC_EXTINT6                      (38L)        
#define MUX_PB06A_EIC_EXTINT6                      (0L)        
#define PINMUX_PB06A_EIC_EXTINT6                   ((PIN_PB06A_EIC_EXTINT6 << 16) | MUX_PB06A_EIC_EXTINT6)
#define PORT_PB06A_EIC_EXTINT6                     ((1UL) << 6)
#define PIN_PB06A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PB06 External Interrupt Line */

#define PIN_PB22A_EIC_EXTINT6                      (54L)        
#define MUX_PB22A_EIC_EXTINT6                      (0L)        
#define PINMUX_PB22A_EIC_EXTINT6                   ((PIN_PB22A_EIC_EXTINT6 << 16) | MUX_PB22A_EIC_EXTINT6)
#define PORT_PB22A_EIC_EXTINT6                     ((1UL) << 22)
#define PIN_PB22A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PB22 External Interrupt Line */

#define PIN_PC06A_EIC_EXTINT6                      (70L)        
#define MUX_PC06A_EIC_EXTINT6                      (0L)        
#define PINMUX_PC06A_EIC_EXTINT6                   ((PIN_PC06A_EIC_EXTINT6 << 16) | MUX_PC06A_EIC_EXTINT6)
#define PORT_PC06A_EIC_EXTINT6                     ((1UL) << 6)
#define PIN_PC06A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PC06 External Interrupt Line */

#define PIN_PC22A_EIC_EXTINT6                      (86L)        
#define MUX_PC22A_EIC_EXTINT6                      (0L)        
#define PINMUX_PC22A_EIC_EXTINT6                   ((PIN_PC22A_EIC_EXTINT6 << 16) | MUX_PC22A_EIC_EXTINT6)
#define PORT_PC22A_EIC_EXTINT6                     ((1UL) << 22)
#define PIN_PC22A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PC22 External Interrupt Line */

#define PIN_PD11A_EIC_EXTINT6                      (107L)       
#define MUX_PD11A_EIC_EXTINT6                      (0L)        
#define PINMUX_PD11A_EIC_EXTINT6                   ((PIN_PD11A_EIC_EXTINT6 << 16) | MUX_PD11A_EIC_EXTINT6)
#define PORT_PD11A_EIC_EXTINT6                     ((1UL) << 11)
#define PIN_PD11A_EIC_EXTINT_NUM                   _L_(6)       /**< EIC signal: PIN_PD11 External Interrupt Line */

#define PIN_PA07A_EIC_EXTINT7                      (7L)         
#define MUX_PA07A_EIC_EXTINT7                      (0L)        
#define PINMUX_PA07A_EIC_EXTINT7                   ((PIN_PA07A_EIC_EXTINT7 << 16) | MUX_PA07A_EIC_EXTINT7)
#define PORT_PA07A_EIC_EXTINT7                     ((1UL) << 7)
#define PIN_PA07A_EIC_EXTINT_NUM                   _L_(7)       /**< EIC signal: PIN_PA07 External Interrupt Line */

#define PIN_PA23A_EIC_EXTINT7                      (23L)        
#define MUX_PA23A_EIC_EXTINT7                      (0L)        
#define PINMUX_PA23A_EIC_EXTINT7                   ((PIN_PA23A_EIC_EXTINT7 << 16) | MUX_PA23A_EIC_EXTINT7)
#define PORT_PA23A_EIC_EXTINT7                     ((1UL) << 23)
#define PIN_PA23A_EIC_EXTINT_NUM                   _L_(7)       /**< EIC signal: PIN_PA23 External Interrupt Line */

#define PIN_PB07A_EIC_EXTINT7                      (39L)        
#define MUX_PB07A_EIC_EXTINT7                      (0L)        
#define PINMUX_PB07A_EIC_EXTINT7                   ((PIN_PB07A_EIC_EXTINT7 << 16) | MUX_PB07A_EIC_EXTINT7)
#define PORT_PB07A_EIC_EXTINT7                     ((1UL) << 7)
#define PIN_PB07A_EIC_EXTINT_NUM                   _L_(7)       /**< EIC signal: PIN_PB07 External Interrupt Line */

#define PIN_PB23A_EIC_EXTINT7                      (55L)        
#define MUX_PB23A_EIC_EXTINT7                      (0L)        
#define PINMUX_PB23A_EIC_EXTINT7                   ((PIN_PB23A_EIC_EXTINT7 << 16) | MUX_PB23A_EIC_EXTINT7)
#define PORT_PB23A_EIC_EXTINT7                     ((1UL) << 23)
#define PIN_PB23A_EIC_EXTINT_NUM                   _L_(7)       /**< EIC signal: PIN_PB23 External Interrupt Line */

#define PIN_PC23A_EIC_EXTINT7                      (87L)        
#define MUX_PC23A_EIC_EXTINT7                      (0L)        
#define PINMUX_PC23A_EIC_EXTINT7                   ((PIN_PC23A_EIC_EXTINT7 << 16) | MUX_PC23A_EIC_EXTINT7)
#define PORT_PC23A_EIC_EXTINT7                     ((1UL) << 23)
#define PIN_PC23A_EIC_EXTINT_NUM                   _L_(7)       /**< EIC signal: PIN_PC23 External Interrupt Line */

#define PIN_PD12A_EIC_EXTINT7                      (108L)       
#define MUX_PD12A_EIC_EXTINT7                      (0L)        
#define PINMUX_PD12A_EIC_EXTINT7                   ((PIN_PD12A_EIC_EXTINT7 << 16) | MUX_PD12A_EIC_EXTINT7)
#define PORT_PD12A_EIC_EXTINT7                     ((1UL) << 12)
#define PIN_PD12A_EIC_EXTINT_NUM                   _L_(7)       /**< EIC signal: PIN_PD12 External Interrupt Line */

#define PIN_PA24A_EIC_EXTINT8                      (24L)        
#define MUX_PA24A_EIC_EXTINT8                      (0L)        
#define PINMUX_PA24A_EIC_EXTINT8                   ((PIN_PA24A_EIC_EXTINT8 << 16) | MUX_PA24A_EIC_EXTINT8)
#define PORT_PA24A_EIC_EXTINT8                     ((1UL) << 24)
#define PIN_PA24A_EIC_EXTINT_NUM                   _L_(8)       /**< EIC signal: PIN_PA24 External Interrupt Line */

#define PIN_PB08A_EIC_EXTINT8                      (40L)        
#define MUX_PB08A_EIC_EXTINT8                      (0L)        
#define PINMUX_PB08A_EIC_EXTINT8                   ((PIN_PB08A_EIC_EXTINT8 << 16) | MUX_PB08A_EIC_EXTINT8)
#define PORT_PB08A_EIC_EXTINT8                     ((1UL) << 8)
#define PIN_PB08A_EIC_EXTINT_NUM                   _L_(8)       /**< EIC signal: PIN_PB08 External Interrupt Line */

#define PIN_PB24A_EIC_EXTINT8                      (56L)        
#define MUX_PB24A_EIC_EXTINT8                      (0L)        
#define PINMUX_PB24A_EIC_EXTINT8                   ((PIN_PB24A_EIC_EXTINT8 << 16) | MUX_PB24A_EIC_EXTINT8)
#define PORT_PB24A_EIC_EXTINT8                     ((1UL) << 24)
#define PIN_PB24A_EIC_EXTINT_NUM                   _L_(8)       /**< EIC signal: PIN_PB24 External Interrupt Line */

#define PIN_PC24A_EIC_EXTINT8                      (88L)        
#define MUX_PC24A_EIC_EXTINT8                      (0L)        
#define PINMUX_PC24A_EIC_EXTINT8                   ((PIN_PC24A_EIC_EXTINT8 << 16) | MUX_PC24A_EIC_EXTINT8)
#define PORT_PC24A_EIC_EXTINT8                     ((1UL) << 24)
#define PIN_PC24A_EIC_EXTINT_NUM                   _L_(8)       /**< EIC signal: PIN_PC24 External Interrupt Line */

#define PIN_PA09A_EIC_EXTINT9                      (9L)         
#define MUX_PA09A_EIC_EXTINT9                      (0L)        
#define PINMUX_PA09A_EIC_EXTINT9                   ((PIN_PA09A_EIC_EXTINT9 << 16) | MUX_PA09A_EIC_EXTINT9)
#define PORT_PA09A_EIC_EXTINT9                     ((1UL) << 9)
#define PIN_PA09A_EIC_EXTINT_NUM                   _L_(9)       /**< EIC signal: PIN_PA09 External Interrupt Line */

#define PIN_PA25A_EIC_EXTINT9                      (25L)        
#define MUX_PA25A_EIC_EXTINT9                      (0L)        
#define PINMUX_PA25A_EIC_EXTINT9                   ((PIN_PA25A_EIC_EXTINT9 << 16) | MUX_PA25A_EIC_EXTINT9)
#define PORT_PA25A_EIC_EXTINT9                     ((1UL) << 25)
#define PIN_PA25A_EIC_EXTINT_NUM                   _L_(9)       /**< EIC signal: PIN_PA25 External Interrupt Line */

#define PIN_PB09A_EIC_EXTINT9                      (41L)        
#define MUX_PB09A_EIC_EXTINT9                      (0L)        
#define PINMUX_PB09A_EIC_EXTINT9                   ((PIN_PB09A_EIC_EXTINT9 << 16) | MUX_PB09A_EIC_EXTINT9)
#define PORT_PB09A_EIC_EXTINT9                     ((1UL) << 9)
#define PIN_PB09A_EIC_EXTINT_NUM                   _L_(9)       /**< EIC signal: PIN_PB09 External Interrupt Line */

#define PIN_PB25A_EIC_EXTINT9                      (57L)        
#define MUX_PB25A_EIC_EXTINT9                      (0L)        
#define PINMUX_PB25A_EIC_EXTINT9                   ((PIN_PB25A_EIC_EXTINT9 << 16) | MUX_PB25A_EIC_EXTINT9)
#define PORT_PB25A_EIC_EXTINT9                     ((1UL) << 25)
#define PIN_PB25A_EIC_EXTINT_NUM                   _L_(9)       /**< EIC signal: PIN_PB25 External Interrupt Line */

#define PIN_PC07A_EIC_EXTINT9                      (71L)        
#define MUX_PC07A_EIC_EXTINT9                      (0L)        
#define PINMUX_PC07A_EIC_EXTINT9                   ((PIN_PC07A_EIC_EXTINT9 << 16) | MUX_PC07A_EIC_EXTINT9)
#define PORT_PC07A_EIC_EXTINT9                     ((1UL) << 7)
#define PIN_PC07A_EIC_EXTINT_NUM                   _L_(9)       /**< EIC signal: PIN_PC07 External Interrupt Line */

#define PIN_PC25A_EIC_EXTINT9                      (89L)        
#define MUX_PC25A_EIC_EXTINT9                      (0L)        
#define PINMUX_PC25A_EIC_EXTINT9                   ((PIN_PC25A_EIC_EXTINT9 << 16) | MUX_PC25A_EIC_EXTINT9)
#define PORT_PC25A_EIC_EXTINT9                     ((1UL) << 25)
#define PIN_PC25A_EIC_EXTINT_NUM                   _L_(9)       /**< EIC signal: PIN_PC25 External Interrupt Line */

#define PIN_PA10A_EIC_EXTINT10                     (10L)        
#define MUX_PA10A_EIC_EXTINT10                     (0L)        
#define PINMUX_PA10A_EIC_EXTINT10                  ((PIN_PA10A_EIC_EXTINT10 << 16) | MUX_PA10A_EIC_EXTINT10)
#define PORT_PA10A_EIC_EXTINT10                    ((1UL) << 10)
#define PIN_PA10A_EIC_EXTINT_NUM                   _L_(10)      /**< EIC signal: PIN_PA10 External Interrupt Line */

#define PIN_PB10A_EIC_EXTINT10                     (42L)        
#define MUX_PB10A_EIC_EXTINT10                     (0L)        
#define PINMUX_PB10A_EIC_EXTINT10                  ((PIN_PB10A_EIC_EXTINT10 << 16) | MUX_PB10A_EIC_EXTINT10)
#define PORT_PB10A_EIC_EXTINT10                    ((1UL) << 10)
#define PIN_PB10A_EIC_EXTINT_NUM                   _L_(10)      /**< EIC signal: PIN_PB10 External Interrupt Line */

#define PIN_PC10A_EIC_EXTINT10                     (74L)        
#define MUX_PC10A_EIC_EXTINT10                     (0L)        
#define PINMUX_PC10A_EIC_EXTINT10                  ((PIN_PC10A_EIC_EXTINT10 << 16) | MUX_PC10A_EIC_EXTINT10)
#define PORT_PC10A_EIC_EXTINT10                    ((1UL) << 10)
#define PIN_PC10A_EIC_EXTINT_NUM                   _L_(10)      /**< EIC signal: PIN_PC10 External Interrupt Line */

#define PIN_PC26A_EIC_EXTINT10                     (90L)        
#define MUX_PC26A_EIC_EXTINT10                     (0L)        
#define PINMUX_PC26A_EIC_EXTINT10                  ((PIN_PC26A_EIC_EXTINT10 << 16) | MUX_PC26A_EIC_EXTINT10)
#define PORT_PC26A_EIC_EXTINT10                    ((1UL) << 26)
#define PIN_PC26A_EIC_EXTINT_NUM                   _L_(10)      /**< EIC signal: PIN_PC26 External Interrupt Line */

#define PIN_PD20A_EIC_EXTINT10                     (116L)       
#define MUX_PD20A_EIC_EXTINT10                     (0L)        
#define PINMUX_PD20A_EIC_EXTINT10                  ((PIN_PD20A_EIC_EXTINT10 << 16) | MUX_PD20A_EIC_EXTINT10)
#define PORT_PD20A_EIC_EXTINT10                    ((1UL) << 20)
#define PIN_PD20A_EIC_EXTINT_NUM                   _L_(10)      /**< EIC signal: PIN_PD20 External Interrupt Line */

#define PIN_PA11A_EIC_EXTINT11                     (11L)        
#define MUX_PA11A_EIC_EXTINT11                     (0L)        
#define PINMUX_PA11A_EIC_EXTINT11                  ((PIN_PA11A_EIC_EXTINT11 << 16) | MUX_PA11A_EIC_EXTINT11)
#define PORT_PA11A_EIC_EXTINT11                    ((1UL) << 11)
#define PIN_PA11A_EIC_EXTINT_NUM                   _L_(11)      /**< EIC signal: PIN_PA11 External Interrupt Line */

#define PIN_PA27A_EIC_EXTINT11                     (27L)        
#define MUX_PA27A_EIC_EXTINT11                     (0L)        
#define PINMUX_PA27A_EIC_EXTINT11                  ((PIN_PA27A_EIC_EXTINT11 << 16) | MUX_PA27A_EIC_EXTINT11)
#define PORT_PA27A_EIC_EXTINT11                    ((1UL) << 27)
#define PIN_PA27A_EIC_EXTINT_NUM                   _L_(11)      /**< EIC signal: PIN_PA27 External Interrupt Line */

#define PIN_PB11A_EIC_EXTINT11                     (43L)        
#define MUX_PB11A_EIC_EXTINT11                     (0L)        
#define PINMUX_PB11A_EIC_EXTINT11                  ((PIN_PB11A_EIC_EXTINT11 << 16) | MUX_PB11A_EIC_EXTINT11)
#define PORT_PB11A_EIC_EXTINT11                    ((1UL) << 11)
#define PIN_PB11A_EIC_EXTINT_NUM                   _L_(11)      /**< EIC signal: PIN_PB11 External Interrupt Line */

#define PIN_PC11A_EIC_EXTINT11                     (75L)        
#define MUX_PC11A_EIC_EXTINT11                     (0L)        
#define PINMUX_PC11A_EIC_EXTINT11                  ((PIN_PC11A_EIC_EXTINT11 << 16) | MUX_PC11A_EIC_EXTINT11)
#define PORT_PC11A_EIC_EXTINT11                    ((1UL) << 11)
#define PIN_PC11A_EIC_EXTINT_NUM                   _L_(11)      /**< EIC signal: PIN_PC11 External Interrupt Line */

#define PIN_PC27A_EIC_EXTINT11                     (91L)        
#define MUX_PC27A_EIC_EXTINT11                     (0L)        
#define PINMUX_PC27A_EIC_EXTINT11                  ((PIN_PC27A_EIC_EXTINT11 << 16) | MUX_PC27A_EIC_EXTINT11)
#define PORT_PC27A_EIC_EXTINT11                    ((1UL) << 27)
#define PIN_PC27A_EIC_EXTINT_NUM                   _L_(11)      /**< EIC signal: PIN_PC27 External Interrupt Line */

#define PIN_PD21A_EIC_EXTINT11                     (117L)       
#define MUX_PD21A_EIC_EXTINT11                     (0L)        
#define PINMUX_PD21A_EIC_EXTINT11                  ((PIN_PD21A_EIC_EXTINT11 << 16) | MUX_PD21A_EIC_EXTINT11)
#define PORT_PD21A_EIC_EXTINT11                    ((1UL) << 21)
#define PIN_PD21A_EIC_EXTINT_NUM                   _L_(11)      /**< EIC signal: PIN_PD21 External Interrupt Line */

#define PIN_PA12A_EIC_EXTINT12                     (12L)        
#define MUX_PA12A_EIC_EXTINT12                     (0L)        
#define PINMUX_PA12A_EIC_EXTINT12                  ((PIN_PA12A_EIC_EXTINT12 << 16) | MUX_PA12A_EIC_EXTINT12)
#define PORT_PA12A_EIC_EXTINT12                    ((1UL) << 12)
#define PIN_PA12A_EIC_EXTINT_NUM                   _L_(12)      /**< EIC signal: PIN_PA12 External Interrupt Line */

#define PIN_PB12A_EIC_EXTINT12                     (44L)        
#define MUX_PB12A_EIC_EXTINT12                     (0L)        
#define PINMUX_PB12A_EIC_EXTINT12                  ((PIN_PB12A_EIC_EXTINT12 << 16) | MUX_PB12A_EIC_EXTINT12)
#define PORT_PB12A_EIC_EXTINT12                    ((1UL) << 12)
#define PIN_PB12A_EIC_EXTINT_NUM                   _L_(12)      /**< EIC signal: PIN_PB12 External Interrupt Line */

#define PIN_PB26A_EIC_EXTINT12                     (58L)        
#define MUX_PB26A_EIC_EXTINT12                     (0L)        
#define PINMUX_PB26A_EIC_EXTINT12                  ((PIN_PB26A_EIC_EXTINT12 << 16) | MUX_PB26A_EIC_EXTINT12)
#define PORT_PB26A_EIC_EXTINT12                    ((1UL) << 26)
#define PIN_PB26A_EIC_EXTINT_NUM                   _L_(12)      /**< EIC signal: PIN_PB26 External Interrupt Line */

#define PIN_PC12A_EIC_EXTINT12                     (76L)        
#define MUX_PC12A_EIC_EXTINT12                     (0L)        
#define PINMUX_PC12A_EIC_EXTINT12                  ((PIN_PC12A_EIC_EXTINT12 << 16) | MUX_PC12A_EIC_EXTINT12)
#define PORT_PC12A_EIC_EXTINT12                    ((1UL) << 12)
#define PIN_PC12A_EIC_EXTINT_NUM                   _L_(12)      /**< EIC signal: PIN_PC12 External Interrupt Line */

#define PIN_PC28A_EIC_EXTINT12                     (92L)        
#define MUX_PC28A_EIC_EXTINT12                     (0L)        
#define PINMUX_PC28A_EIC_EXTINT12                  ((PIN_PC28A_EIC_EXTINT12 << 16) | MUX_PC28A_EIC_EXTINT12)
#define PORT_PC28A_EIC_EXTINT12                    ((1UL) << 28)
#define PIN_PC28A_EIC_EXTINT_NUM                   _L_(12)      /**< EIC signal: PIN_PC28 External Interrupt Line */

#define PIN_PA13A_EIC_EXTINT13                     (13L)        
#define MUX_PA13A_EIC_EXTINT13                     (0L)        
#define PINMUX_PA13A_EIC_EXTINT13                  ((PIN_PA13A_EIC_EXTINT13 << 16) | MUX_PA13A_EIC_EXTINT13)
#define PORT_PA13A_EIC_EXTINT13                    ((1UL) << 13)
#define PIN_PA13A_EIC_EXTINT_NUM                   _L_(13)      /**< EIC signal: PIN_PA13 External Interrupt Line */

#define PIN_PB13A_EIC_EXTINT13                     (45L)        
#define MUX_PB13A_EIC_EXTINT13                     (0L)        
#define PINMUX_PB13A_EIC_EXTINT13                  ((PIN_PB13A_EIC_EXTINT13 << 16) | MUX_PB13A_EIC_EXTINT13)
#define PORT_PB13A_EIC_EXTINT13                    ((1UL) << 13)
#define PIN_PB13A_EIC_EXTINT_NUM                   _L_(13)      /**< EIC signal: PIN_PB13 External Interrupt Line */

#define PIN_PB27A_EIC_EXTINT13                     (59L)        
#define MUX_PB27A_EIC_EXTINT13                     (0L)        
#define PINMUX_PB27A_EIC_EXTINT13                  ((PIN_PB27A_EIC_EXTINT13 << 16) | MUX_PB27A_EIC_EXTINT13)
#define PORT_PB27A_EIC_EXTINT13                    ((1UL) << 27)
#define PIN_PB27A_EIC_EXTINT_NUM                   _L_(13)      /**< EIC signal: PIN_PB27 External Interrupt Line */

#define PIN_PC13A_EIC_EXTINT13                     (77L)        
#define MUX_PC13A_EIC_EXTINT13                     (0L)        
#define PINMUX_PC13A_EIC_EXTINT13                  ((PIN_PC13A_EIC_EXTINT13 << 16) | MUX_PC13A_EIC_EXTINT13)
#define PORT_PC13A_EIC_EXTINT13                    ((1UL) << 13)
#define PIN_PC13A_EIC_EXTINT_NUM                   _L_(13)      /**< EIC signal: PIN_PC13 External Interrupt Line */

#define PIN_PA30A_EIC_EXTINT14                     (30L)        
#define MUX_PA30A_EIC_EXTINT14                     (0L)        
#define PINMUX_PA30A_EIC_EXTINT14                  ((PIN_PA30A_EIC_EXTINT14 << 16) | MUX_PA30A_EIC_EXTINT14)
#define PORT_PA30A_EIC_EXTINT14                    ((1UL) << 30)
#define PIN_PA30A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PA30 External Interrupt Line */

#define PIN_PB14A_EIC_EXTINT14                     (46L)        
#define MUX_PB14A_EIC_EXTINT14                     (0L)        
#define PINMUX_PB14A_EIC_EXTINT14                  ((PIN_PB14A_EIC_EXTINT14 << 16) | MUX_PB14A_EIC_EXTINT14)
#define PORT_PB14A_EIC_EXTINT14                    ((1UL) << 14)
#define PIN_PB14A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PB14 External Interrupt Line */

#define PIN_PB28A_EIC_EXTINT14                     (60L)        
#define MUX_PB28A_EIC_EXTINT14                     (0L)        
#define PINMUX_PB28A_EIC_EXTINT14                  ((PIN_PB28A_EIC_EXTINT14 << 16) | MUX_PB28A_EIC_EXTINT14)
#define PORT_PB28A_EIC_EXTINT14                    ((1UL) << 28)
#define PIN_PB28A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PB28 External Interrupt Line */

#define PIN_PB30A_EIC_EXTINT14                     (62L)        
#define MUX_PB30A_EIC_EXTINT14                     (0L)        
#define PINMUX_PB30A_EIC_EXTINT14                  ((PIN_PB30A_EIC_EXTINT14 << 16) | MUX_PB30A_EIC_EXTINT14)
#define PORT_PB30A_EIC_EXTINT14                    ((1UL) << 30)
#define PIN_PB30A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PB30 External Interrupt Line */

#define PIN_PC14A_EIC_EXTINT14                     (78L)        
#define MUX_PC14A_EIC_EXTINT14                     (0L)        
#define PINMUX_PC14A_EIC_EXTINT14                  ((PIN_PC14A_EIC_EXTINT14 << 16) | MUX_PC14A_EIC_EXTINT14)
#define PORT_PC14A_EIC_EXTINT14                    ((1UL) << 14)
#define PIN_PC14A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PC14 External Interrupt Line */

#define PIN_PC30A_EIC_EXTINT14                     (94L)        
#define MUX_PC30A_EIC_EXTINT14                     (0L)        
#define PINMUX_PC30A_EIC_EXTINT14                  ((PIN_PC30A_EIC_EXTINT14 << 16) | MUX_PC30A_EIC_EXTINT14)
#define PORT_PC30A_EIC_EXTINT14                    ((1UL) << 30)
#define PIN_PC30A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PC30 External Interrupt Line */

#define PIN_PA14A_EIC_EXTINT14                     (14L)        
#define MUX_PA14A_EIC_EXTINT14                     (0L)        
#define PINMUX_PA14A_EIC_EXTINT14                  ((PIN_PA14A_EIC_EXTINT14 << 16) | MUX_PA14A_EIC_EXTINT14)
#define PORT_PA14A_EIC_EXTINT14                    ((1UL) << 14)
#define PIN_PA14A_EIC_EXTINT_NUM                   _L_(14)      /**< EIC signal: PIN_PA14 External Interrupt Line */

#define PIN_PA15A_EIC_EXTINT15                     (15L)        
#define MUX_PA15A_EIC_EXTINT15                     (0L)        
#define PINMUX_PA15A_EIC_EXTINT15                  ((PIN_PA15A_EIC_EXTINT15 << 16) | MUX_PA15A_EIC_EXTINT15)
#define PORT_PA15A_EIC_EXTINT15                    ((1UL) << 15)
#define PIN_PA15A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PA15 External Interrupt Line */

#define PIN_PA31A_EIC_EXTINT15                     (31L)        
#define MUX_PA31A_EIC_EXTINT15                     (0L)        
#define PINMUX_PA31A_EIC_EXTINT15                  ((PIN_PA31A_EIC_EXTINT15 << 16) | MUX_PA31A_EIC_EXTINT15)
#define PORT_PA31A_EIC_EXTINT15                    ((1UL) << 31)
#define PIN_PA31A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PA31 External Interrupt Line */

#define PIN_PB15A_EIC_EXTINT15                     (47L)        
#define MUX_PB15A_EIC_EXTINT15                     (0L)        
#define PINMUX_PB15A_EIC_EXTINT15                  ((PIN_PB15A_EIC_EXTINT15 << 16) | MUX_PB15A_EIC_EXTINT15)
#define PORT_PB15A_EIC_EXTINT15                    ((1UL) << 15)
#define PIN_PB15A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PB15 External Interrupt Line */

#define PIN_PB29A_EIC_EXTINT15                     (61L)        
#define MUX_PB29A_EIC_EXTINT15                     (0L)        
#define PINMUX_PB29A_EIC_EXTINT15                  ((PIN_PB29A_EIC_EXTINT15 << 16) | MUX_PB29A_EIC_EXTINT15)
#define PORT_PB29A_EIC_EXTINT15                    ((1UL) << 29)
#define PIN_PB29A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PB29 External Interrupt Line */

#define PIN_PB31A_EIC_EXTINT15                     (63L)        
#define MUX_PB31A_EIC_EXTINT15                     (0L)        
#define PINMUX_PB31A_EIC_EXTINT15                  ((PIN_PB31A_EIC_EXTINT15 << 16) | MUX_PB31A_EIC_EXTINT15)
#define PORT_PB31A_EIC_EXTINT15                    ((1UL) << 31)
#define PIN_PB31A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PB31 External Interrupt Line */

#define PIN_PC15A_EIC_EXTINT15                     (79L)        
#define MUX_PC15A_EIC_EXTINT15                     (0L)        
#define PINMUX_PC15A_EIC_EXTINT15                  ((PIN_PC15A_EIC_EXTINT15 << 16) | MUX_PC15A_EIC_EXTINT15)
#define PORT_PC15A_EIC_EXTINT15                    ((1UL) << 15)
#define PIN_PC15A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PC15 External Interrupt Line */

#define PIN_PC31A_EIC_EXTINT15                     (95L)        
#define MUX_PC31A_EIC_EXTINT15                     (0L)        
#define PINMUX_PC31A_EIC_EXTINT15                  ((PIN_PC31A_EIC_EXTINT15 << 16) | MUX_PC31A_EIC_EXTINT15)
#define PORT_PC31A_EIC_EXTINT15                    ((1UL) << 31)
#define PIN_PC31A_EIC_EXTINT_NUM                   _L_(15)      /**< EIC signal: PIN_PC31 External Interrupt Line */

#define PIN_PA08A_EIC_NMI                          (8L)         
#define MUX_PA08A_EIC_NMI                          (0L)        
#define PINMUX_PA08A_EIC_NMI                       ((PIN_PA08A_EIC_NMI << 16) | MUX_PA08A_EIC_NMI)
#define PORT_PA08A_EIC_NMI                         ((1UL) << 8)

/* ========== PORT definition for GCLK peripheral ========== */
#define PIN_PA30M_GCLK_IO0                         (30L)        
#define MUX_PA30M_GCLK_IO0                         (12L)       
#define PINMUX_PA30M_GCLK_IO0                      ((PIN_PA30M_GCLK_IO0 << 16) | MUX_PA30M_GCLK_IO0)
#define PORT_PA30M_GCLK_IO0                        ((1UL) << 30)

#define PIN_PB14M_GCLK_IO0                         (46L)        
#define MUX_PB14M_GCLK_IO0                         (12L)       
#define PINMUX_PB14M_GCLK_IO0                      ((PIN_PB14M_GCLK_IO0 << 16) | MUX_PB14M_GCLK_IO0)
#define PORT_PB14M_GCLK_IO0                        ((1UL) << 14)

#define PIN_PA14M_GCLK_IO0                         (14L)        
#define MUX_PA14M_GCLK_IO0                         (12L)       
#define PINMUX_PA14M_GCLK_IO0                      ((PIN_PA14M_GCLK_IO0 << 16) | MUX_PA14M_GCLK_IO0)
#define PORT_PA14M_GCLK_IO0                        ((1UL) << 14)

#define PIN_PB22M_GCLK_IO0                         (54L)        
#define MUX_PB22M_GCLK_IO0                         (12L)       
#define PINMUX_PB22M_GCLK_IO0                      ((PIN_PB22M_GCLK_IO0 << 16) | MUX_PB22M_GCLK_IO0)
#define PORT_PB22M_GCLK_IO0                        ((1UL) << 22)

#define PIN_PB15M_GCLK_IO1                         (47L)        
#define MUX_PB15M_GCLK_IO1                         (12L)       
#define PINMUX_PB15M_GCLK_IO1                      ((PIN_PB15M_GCLK_IO1 << 16) | MUX_PB15M_GCLK_IO1)
#define PORT_PB15M_GCLK_IO1                        ((1UL) << 15)

#define PIN_PA15M_GCLK_IO1                         (15L)        
#define MUX_PA15M_GCLK_IO1                         (12L)       
#define PINMUX_PA15M_GCLK_IO1                      ((PIN_PA15M_GCLK_IO1 << 16) | MUX_PA15M_GCLK_IO1)
#define PORT_PA15M_GCLK_IO1                        ((1UL) << 15)

#define PIN_PB23M_GCLK_IO1                         (55L)        
#define MUX_PB23M_GCLK_IO1                         (12L)       
#define PINMUX_PB23M_GCLK_IO1                      ((PIN_PB23M_GCLK_IO1 << 16) | MUX_PB23M_GCLK_IO1)
#define PORT_PB23M_GCLK_IO1                        ((1UL) << 23)

#define PIN_PA27M_GCLK_IO1                         (27L)        
#define MUX_PA27M_GCLK_IO1                         (12L)       
#define PINMUX_PA27M_GCLK_IO1                      ((PIN_PA27M_GCLK_IO1 << 16) | MUX_PA27M_GCLK_IO1)
#define PORT_PA27M_GCLK_IO1                        ((1UL) << 27)

#define PIN_PA16M_GCLK_IO2                         (16L)        
#define MUX_PA16M_GCLK_IO2                         (12L)       
#define PINMUX_PA16M_GCLK_IO2                      ((PIN_PA16M_GCLK_IO2 << 16) | MUX_PA16M_GCLK_IO2)
#define PORT_PA16M_GCLK_IO2                        ((1UL) << 16)

#define PIN_PB16M_GCLK_IO2                         (48L)        
#define MUX_PB16M_GCLK_IO2                         (12L)       
#define PINMUX_PB16M_GCLK_IO2                      ((PIN_PB16M_GCLK_IO2 << 16) | MUX_PB16M_GCLK_IO2)
#define PORT_PB16M_GCLK_IO2                        ((1UL) << 16)

#define PIN_PA17M_GCLK_IO3                         (17L)        
#define MUX_PA17M_GCLK_IO3                         (12L)       
#define PINMUX_PA17M_GCLK_IO3                      ((PIN_PA17M_GCLK_IO3 << 16) | MUX_PA17M_GCLK_IO3)
#define PORT_PA17M_GCLK_IO3                        ((1UL) << 17)

#define PIN_PB17M_GCLK_IO3                         (49L)        
#define MUX_PB17M_GCLK_IO3                         (12L)       
#define PINMUX_PB17M_GCLK_IO3                      ((PIN_PB17M_GCLK_IO3 << 16) | MUX_PB17M_GCLK_IO3)
#define PORT_PB17M_GCLK_IO3                        ((1UL) << 17)

#define PIN_PA10M_GCLK_IO4                         (10L)        
#define MUX_PA10M_GCLK_IO4                         (12L)       
#define PINMUX_PA10M_GCLK_IO4                      ((PIN_PA10M_GCLK_IO4 << 16) | MUX_PA10M_GCLK_IO4)
#define PORT_PA10M_GCLK_IO4                        ((1UL) << 10)

#define PIN_PB10M_GCLK_IO4                         (42L)        
#define MUX_PB10M_GCLK_IO4                         (12L)       
#define PINMUX_PB10M_GCLK_IO4                      ((PIN_PB10M_GCLK_IO4 << 16) | MUX_PB10M_GCLK_IO4)
#define PORT_PB10M_GCLK_IO4                        ((1UL) << 10)

#define PIN_PB18M_GCLK_IO4                         (50L)        
#define MUX_PB18M_GCLK_IO4                         (12L)       
#define PINMUX_PB18M_GCLK_IO4                      ((PIN_PB18M_GCLK_IO4 << 16) | MUX_PB18M_GCLK_IO4)
#define PORT_PB18M_GCLK_IO4                        ((1UL) << 18)

#define PIN_PA11M_GCLK_IO5                         (11L)        
#define MUX_PA11M_GCLK_IO5                         (12L)       
#define PINMUX_PA11M_GCLK_IO5                      ((PIN_PA11M_GCLK_IO5 << 16) | MUX_PA11M_GCLK_IO5)
#define PORT_PA11M_GCLK_IO5                        ((1UL) << 11)

#define PIN_PB11M_GCLK_IO5                         (43L)        
#define MUX_PB11M_GCLK_IO5                         (12L)       
#define PINMUX_PB11M_GCLK_IO5                      ((PIN_PB11M_GCLK_IO5 << 16) | MUX_PB11M_GCLK_IO5)
#define PORT_PB11M_GCLK_IO5                        ((1UL) << 11)

#define PIN_PB19M_GCLK_IO5                         (51L)        
#define MUX_PB19M_GCLK_IO5                         (12L)       
#define PINMUX_PB19M_GCLK_IO5                      ((PIN_PB19M_GCLK_IO5 << 16) | MUX_PB19M_GCLK_IO5)
#define PORT_PB19M_GCLK_IO5                        ((1UL) << 19)

#define PIN_PB12M_GCLK_IO6                         (44L)        
#define MUX_PB12M_GCLK_IO6                         (12L)       
#define PINMUX_PB12M_GCLK_IO6                      ((PIN_PB12M_GCLK_IO6 << 16) | MUX_PB12M_GCLK_IO6)
#define PORT_PB12M_GCLK_IO6                        ((1UL) << 12)

#define PIN_PB20M_GCLK_IO6                         (52L)        
#define MUX_PB20M_GCLK_IO6                         (12L)       
#define PINMUX_PB20M_GCLK_IO6                      ((PIN_PB20M_GCLK_IO6 << 16) | MUX_PB20M_GCLK_IO6)
#define PORT_PB20M_GCLK_IO6                        ((1UL) << 20)

#define PIN_PB13M_GCLK_IO7                         (45L)        
#define MUX_PB13M_GCLK_IO7                         (12L)       
#define PINMUX_PB13M_GCLK_IO7                      ((PIN_PB13M_GCLK_IO7 << 16) | MUX_PB13M_GCLK_IO7)
#define PORT_PB13M_GCLK_IO7                        ((1UL) << 13)

#define PIN_PB21M_GCLK_IO7                         (53L)        
#define MUX_PB21M_GCLK_IO7                         (12L)       
#define PINMUX_PB21M_GCLK_IO7                      ((PIN_PB21M_GCLK_IO7 << 16) | MUX_PB21M_GCLK_IO7)
#define PORT_PB21M_GCLK_IO7                        ((1UL) << 21)

/* ========== PORT definition for GMAC peripheral ========== */
#define PIN_PC21L_GMAC_GCOL                        (85L)        
#define MUX_PC21L_GMAC_GCOL                        (11L)       
#define PINMUX_PC21L_GMAC_GCOL                     ((PIN_PC21L_GMAC_GCOL << 16) | MUX_PC21L_GMAC_GCOL)
#define PORT_PC21L_GMAC_GCOL                       ((1UL) << 21)

#define PIN_PA16L_GMAC_GCRS                        (16L)        
#define MUX_PA16L_GMAC_GCRS                        (11L)       
#define PINMUX_PA16L_GMAC_GCRS                     ((PIN_PA16L_GMAC_GCRS << 16) | MUX_PA16L_GMAC_GCRS)
#define PORT_PA16L_GMAC_GCRS                       ((1UL) << 16)

#define PIN_PA20L_GMAC_GMDC                        (20L)        
#define MUX_PA20L_GMAC_GMDC                        (11L)       
#define PINMUX_PA20L_GMAC_GMDC                     ((PIN_PA20L_GMAC_GMDC << 16) | MUX_PA20L_GMAC_GMDC)
#define PORT_PA20L_GMAC_GMDC                       ((1UL) << 20)

#define PIN_PB14L_GMAC_GMDC                        (46L)        
#define MUX_PB14L_GMAC_GMDC                        (11L)       
#define PINMUX_PB14L_GMAC_GMDC                     ((PIN_PB14L_GMAC_GMDC << 16) | MUX_PB14L_GMAC_GMDC)
#define PORT_PB14L_GMAC_GMDC                       ((1UL) << 14)

#define PIN_PC11L_GMAC_GMDC                        (75L)        
#define MUX_PC11L_GMAC_GMDC                        (11L)       
#define PINMUX_PC11L_GMAC_GMDC                     ((PIN_PC11L_GMAC_GMDC << 16) | MUX_PC11L_GMAC_GMDC)
#define PORT_PC11L_GMAC_GMDC                       ((1UL) << 11)

#define PIN_PC22L_GMAC_GMDC                        (86L)        
#define MUX_PC22L_GMAC_GMDC                        (11L)       
#define PINMUX_PC22L_GMAC_GMDC                     ((PIN_PC22L_GMAC_GMDC << 16) | MUX_PC22L_GMAC_GMDC)
#define PORT_PC22L_GMAC_GMDC                       ((1UL) << 22)

#define PIN_PA21L_GMAC_GMDIO                       (21L)        
#define MUX_PA21L_GMAC_GMDIO                       (11L)       
#define PINMUX_PA21L_GMAC_GMDIO                    ((PIN_PA21L_GMAC_GMDIO << 16) | MUX_PA21L_GMAC_GMDIO)
#define PORT_PA21L_GMAC_GMDIO                      ((1UL) << 21)

#define PIN_PB15L_GMAC_GMDIO                       (47L)        
#define MUX_PB15L_GMAC_GMDIO                       (11L)       
#define PINMUX_PB15L_GMAC_GMDIO                    ((PIN_PB15L_GMAC_GMDIO << 16) | MUX_PB15L_GMAC_GMDIO)
#define PORT_PB15L_GMAC_GMDIO                      ((1UL) << 15)

#define PIN_PC12L_GMAC_GMDIO                       (76L)        
#define MUX_PC12L_GMAC_GMDIO                       (11L)       
#define PINMUX_PC12L_GMAC_GMDIO                    ((PIN_PC12L_GMAC_GMDIO << 16) | MUX_PC12L_GMAC_GMDIO)
#define PORT_PC12L_GMAC_GMDIO                      ((1UL) << 12)

#define PIN_PC23L_GMAC_GMDIO                       (87L)        
#define MUX_PC23L_GMAC_GMDIO                       (11L)       
#define PINMUX_PC23L_GMAC_GMDIO                    ((PIN_PC23L_GMAC_GMDIO << 16) | MUX_PC23L_GMAC_GMDIO)
#define PORT_PC23L_GMAC_GMDIO                      ((1UL) << 23)

#define PIN_PA13L_GMAC_GRX0                        (13L)        
#define MUX_PA13L_GMAC_GRX0                        (11L)       
#define PINMUX_PA13L_GMAC_GRX0                     ((PIN_PA13L_GMAC_GRX0 << 16) | MUX_PA13L_GMAC_GRX0)
#define PORT_PA13L_GMAC_GRX0                       ((1UL) << 13)

#define PIN_PA12L_GMAC_GRX1                        (12L)        
#define MUX_PA12L_GMAC_GRX1                        (11L)       
#define PINMUX_PA12L_GMAC_GRX1                     ((PIN_PA12L_GMAC_GRX1 << 16) | MUX_PA12L_GMAC_GRX1)
#define PORT_PA12L_GMAC_GRX1                       ((1UL) << 12)

#define PIN_PC15L_GMAC_GRX2                        (79L)        
#define MUX_PC15L_GMAC_GRX2                        (11L)       
#define PINMUX_PC15L_GMAC_GRX2                     ((PIN_PC15L_GMAC_GRX2 << 16) | MUX_PC15L_GMAC_GRX2)
#define PORT_PC15L_GMAC_GRX2                       ((1UL) << 15)

#define PIN_PC14L_GMAC_GRX3                        (78L)        
#define MUX_PC14L_GMAC_GRX3                        (11L)       
#define PINMUX_PC14L_GMAC_GRX3                     ((PIN_PC14L_GMAC_GRX3 << 16) | MUX_PC14L_GMAC_GRX3)
#define PORT_PC14L_GMAC_GRX3                       ((1UL) << 14)

#define PIN_PC18L_GMAC_GRXCK                       (82L)        
#define MUX_PC18L_GMAC_GRXCK                       (11L)       
#define PINMUX_PC18L_GMAC_GRXCK                    ((PIN_PC18L_GMAC_GRXCK << 16) | MUX_PC18L_GMAC_GRXCK)
#define PORT_PC18L_GMAC_GRXCK                      ((1UL) << 18)

#define PIN_PC20L_GMAC_GRXDV                       (84L)        
#define MUX_PC20L_GMAC_GRXDV                       (11L)       
#define PINMUX_PC20L_GMAC_GRXDV                    ((PIN_PC20L_GMAC_GRXDV << 16) | MUX_PC20L_GMAC_GRXDV)
#define PORT_PC20L_GMAC_GRXDV                      ((1UL) << 20)

#define PIN_PA15L_GMAC_GRXER                       (15L)        
#define MUX_PA15L_GMAC_GRXER                       (11L)       
#define PINMUX_PA15L_GMAC_GRXER                    ((PIN_PA15L_GMAC_GRXER << 16) | MUX_PA15L_GMAC_GRXER)
#define PORT_PA15L_GMAC_GRXER                      ((1UL) << 15)

#define PIN_PA18L_GMAC_GTX0                        (18L)        
#define MUX_PA18L_GMAC_GTX0                        (11L)       
#define PINMUX_PA18L_GMAC_GTX0                     ((PIN_PA18L_GMAC_GTX0 << 16) | MUX_PA18L_GMAC_GTX0)
#define PORT_PA18L_GMAC_GTX0                       ((1UL) << 18)

#define PIN_PA19L_GMAC_GTX1                        (19L)        
#define MUX_PA19L_GMAC_GTX1                        (11L)       
#define PINMUX_PA19L_GMAC_GTX1                     ((PIN_PA19L_GMAC_GTX1 << 16) | MUX_PA19L_GMAC_GTX1)
#define PORT_PA19L_GMAC_GTX1                       ((1UL) << 19)

#define PIN_PC16L_GMAC_GTX2                        (80L)        
#define MUX_PC16L_GMAC_GTX2                        (11L)       
#define PINMUX_PC16L_GMAC_GTX2                     ((PIN_PC16L_GMAC_GTX2 << 16) | MUX_PC16L_GMAC_GTX2)
#define PORT_PC16L_GMAC_GTX2                       ((1UL) << 16)

#define PIN_PC17L_GMAC_GTX3                        (81L)        
#define MUX_PC17L_GMAC_GTX3                        (11L)       
#define PINMUX_PC17L_GMAC_GTX3                     ((PIN_PC17L_GMAC_GTX3 << 16) | MUX_PC17L_GMAC_GTX3)
#define PORT_PC17L_GMAC_GTX3                       ((1UL) << 17)

#define PIN_PA14L_GMAC_GTXCK                       (14L)        
#define MUX_PA14L_GMAC_GTXCK                       (11L)       
#define PINMUX_PA14L_GMAC_GTXCK                    ((PIN_PA14L_GMAC_GTXCK << 16) | MUX_PA14L_GMAC_GTXCK)
#define PORT_PA14L_GMAC_GTXCK                      ((1UL) << 14)

#define PIN_PA17L_GMAC_GTXEN                       (17L)        
#define MUX_PA17L_GMAC_GTXEN                       (11L)       
#define PINMUX_PA17L_GMAC_GTXEN                    ((PIN_PA17L_GMAC_GTXEN << 16) | MUX_PA17L_GMAC_GTXEN)
#define PORT_PA17L_GMAC_GTXEN                      ((1UL) << 17)

#define PIN_PC19L_GMAC_GTXER                       (83L)        
#define MUX_PC19L_GMAC_GTXER                       (11L)       
#define PINMUX_PC19L_GMAC_GTXER                    ((PIN_PC19L_GMAC_GTXER << 16) | MUX_PC19L_GMAC_GTXER)
#define PORT_PC19L_GMAC_GTXER                      ((1UL) << 19)

/* ========== PORT definition for I2S peripheral ========== */
#define PIN_PA09J_I2S_FS0                          (9L)         
#define MUX_PA09J_I2S_FS0                          (9L)        
#define PINMUX_PA09J_I2S_FS0                       ((PIN_PA09J_I2S_FS0 << 16) | MUX_PA09J_I2S_FS0)
#define PORT_PA09J_I2S_FS0                         ((1UL) << 9)

#define PIN_PA20J_I2S_FS0                          (20L)        
#define MUX_PA20J_I2S_FS0                          (9L)        
#define PINMUX_PA20J_I2S_FS0                       ((PIN_PA20J_I2S_FS0 << 16) | MUX_PA20J_I2S_FS0)
#define PORT_PA20J_I2S_FS0                         ((1UL) << 20)

#define PIN_PA23J_I2S_FS1                          (23L)        
#define MUX_PA23J_I2S_FS1                          (9L)        
#define PINMUX_PA23J_I2S_FS1                       ((PIN_PA23J_I2S_FS1 << 16) | MUX_PA23J_I2S_FS1)
#define PORT_PA23J_I2S_FS1                         ((1UL) << 23)

#define PIN_PB11J_I2S_FS1                          (43L)        
#define MUX_PB11J_I2S_FS1                          (9L)        
#define PINMUX_PB11J_I2S_FS1                       ((PIN_PB11J_I2S_FS1 << 16) | MUX_PB11J_I2S_FS1)
#define PORT_PB11J_I2S_FS1                         ((1UL) << 11)

#define PIN_PA08J_I2S_MCK0                         (8L)         
#define MUX_PA08J_I2S_MCK0                         (9L)        
#define PINMUX_PA08J_I2S_MCK0                      ((PIN_PA08J_I2S_MCK0 << 16) | MUX_PA08J_I2S_MCK0)
#define PORT_PA08J_I2S_MCK0                        ((1UL) << 8)

#define PIN_PB17J_I2S_MCK0                         (49L)        
#define MUX_PB17J_I2S_MCK0                         (9L)        
#define PINMUX_PB17J_I2S_MCK0                      ((PIN_PB17J_I2S_MCK0 << 16) | MUX_PB17J_I2S_MCK0)
#define PORT_PB17J_I2S_MCK0                        ((1UL) << 17)

#define PIN_PB29J_I2S_MCK1                         (61L)        
#define MUX_PB29J_I2S_MCK1                         (9L)        
#define PINMUX_PB29J_I2S_MCK1                      ((PIN_PB29J_I2S_MCK1 << 16) | MUX_PB29J_I2S_MCK1)
#define PORT_PB29J_I2S_MCK1                        ((1UL) << 29)

#define PIN_PB13J_I2S_MCK1                         (45L)        
#define MUX_PB13J_I2S_MCK1                         (9L)        
#define PINMUX_PB13J_I2S_MCK1                      ((PIN_PB13J_I2S_MCK1 << 16) | MUX_PB13J_I2S_MCK1)
#define PORT_PB13J_I2S_MCK1                        ((1UL) << 13)

#define PIN_PA10J_I2S_SCK0                         (10L)        
#define MUX_PA10J_I2S_SCK0                         (9L)        
#define PINMUX_PA10J_I2S_SCK0                      ((PIN_PA10J_I2S_SCK0 << 16) | MUX_PA10J_I2S_SCK0)
#define PORT_PA10J_I2S_SCK0                        ((1UL) << 10)

#define PIN_PB16J_I2S_SCK0                         (48L)        
#define MUX_PB16J_I2S_SCK0                         (9L)        
#define PINMUX_PB16J_I2S_SCK0                      ((PIN_PB16J_I2S_SCK0 << 16) | MUX_PB16J_I2S_SCK0)
#define PORT_PB16J_I2S_SCK0                        ((1UL) << 16)

#define PIN_PB28J_I2S_SCK1                         (60L)        
#define MUX_PB28J_I2S_SCK1                         (9L)        
#define PINMUX_PB28J_I2S_SCK1                      ((PIN_PB28J_I2S_SCK1 << 16) | MUX_PB28J_I2S_SCK1)
#define PORT_PB28J_I2S_SCK1                        ((1UL) << 28)

#define PIN_PB12J_I2S_SCK1                         (44L)        
#define MUX_PB12J_I2S_SCK1                         (9L)        
#define PINMUX_PB12J_I2S_SCK1                      ((PIN_PB12J_I2S_SCK1 << 16) | MUX_PB12J_I2S_SCK1)
#define PORT_PB12J_I2S_SCK1                        ((1UL) << 12)

#define PIN_PA22J_I2S_SDI                          (22L)        
#define MUX_PA22J_I2S_SDI                          (9L)        
#define PINMUX_PA22J_I2S_SDI                       ((PIN_PA22J_I2S_SDI << 16) | MUX_PA22J_I2S_SDI)
#define PORT_PA22J_I2S_SDI                         ((1UL) << 22)

#define PIN_PB10J_I2S_SDI                          (42L)        
#define MUX_PB10J_I2S_SDI                          (9L)        
#define PINMUX_PB10J_I2S_SDI                       ((PIN_PB10J_I2S_SDI << 16) | MUX_PB10J_I2S_SDI)
#define PORT_PB10J_I2S_SDI                         ((1UL) << 10)

#define PIN_PA11J_I2S_SDO                          (11L)        
#define MUX_PA11J_I2S_SDO                          (9L)        
#define PINMUX_PA11J_I2S_SDO                       ((PIN_PA11J_I2S_SDO << 16) | MUX_PA11J_I2S_SDO)
#define PORT_PA11J_I2S_SDO                         ((1UL) << 11)

#define PIN_PA21J_I2S_SDO                          (21L)        
#define MUX_PA21J_I2S_SDO                          (9L)        
#define PINMUX_PA21J_I2S_SDO                       ((PIN_PA21J_I2S_SDO << 16) | MUX_PA21J_I2S_SDO)
#define PORT_PA21J_I2S_SDO                         ((1UL) << 21)

/* ========== PORT definition for PCC peripheral ========== */
#define PIN_PA14K_PCC_CLK                          (14L)        
#define MUX_PA14K_PCC_CLK                          (10L)       
#define PINMUX_PA14K_PCC_CLK                       ((PIN_PA14K_PCC_CLK << 16) | MUX_PA14K_PCC_CLK)
#define PORT_PA14K_PCC_CLK                         ((1UL) << 14)

#define PIN_PA16K_PCC_DATA0                        (16L)        
#define MUX_PA16K_PCC_DATA0                        (10L)       
#define PINMUX_PA16K_PCC_DATA0                     ((PIN_PA16K_PCC_DATA0 << 16) | MUX_PA16K_PCC_DATA0)
#define PORT_PA16K_PCC_DATA0                       ((1UL) << 16)

#define PIN_PA17K_PCC_DATA1                        (17L)        
#define MUX_PA17K_PCC_DATA1                        (10L)       
#define PINMUX_PA17K_PCC_DATA1                     ((PIN_PA17K_PCC_DATA1 << 16) | MUX_PA17K_PCC_DATA1)
#define PORT_PA17K_PCC_DATA1                       ((1UL) << 17)

#define PIN_PA18K_PCC_DATA2                        (18L)        
#define MUX_PA18K_PCC_DATA2                        (10L)       
#define PINMUX_PA18K_PCC_DATA2                     ((PIN_PA18K_PCC_DATA2 << 16) | MUX_PA18K_PCC_DATA2)
#define PORT_PA18K_PCC_DATA2                       ((1UL) << 18)

#define PIN_PA19K_PCC_DATA3                        (19L)        
#define MUX_PA19K_PCC_DATA3                        (10L)       
#define PINMUX_PA19K_PCC_DATA3                     ((PIN_PA19K_PCC_DATA3 << 16) | MUX_PA19K_PCC_DATA3)
#define PORT_PA19K_PCC_DATA3                       ((1UL) << 19)

#define PIN_PA20K_PCC_DATA4                        (20L)        
#define MUX_PA20K_PCC_DATA4                        (10L)       
#define PINMUX_PA20K_PCC_DATA4                     ((PIN_PA20K_PCC_DATA4 << 16) | MUX_PA20K_PCC_DATA4)
#define PORT_PA20K_PCC_DATA4                       ((1UL) << 20)

#define PIN_PA21K_PCC_DATA5                        (21L)        
#define MUX_PA21K_PCC_DATA5                        (10L)       
#define PINMUX_PA21K_PCC_DATA5                     ((PIN_PA21K_PCC_DATA5 << 16) | MUX_PA21K_PCC_DATA5)
#define PORT_PA21K_PCC_DATA5                       ((1UL) << 21)

#define PIN_PA22K_PCC_DATA6                        (22L)        
#define MUX_PA22K_PCC_DATA6                        (10L)       
#define PINMUX_PA22K_PCC_DATA6                     ((PIN_PA22K_PCC_DATA6 << 16) | MUX_PA22K_PCC_DATA6)
#define PORT_PA22K_PCC_DATA6                       ((1UL) << 22)

#define PIN_PA23K_PCC_DATA7                        (23L)        
#define MUX_PA23K_PCC_DATA7                        (10L)       
#define PINMUX_PA23K_PCC_DATA7                     ((PIN_PA23K_PCC_DATA7 << 16) | MUX_PA23K_PCC_DATA7)
#define PORT_PA23K_PCC_DATA7                       ((1UL) << 23)

#define PIN_PB14K_PCC_DATA8                        (46L)        
#define MUX_PB14K_PCC_DATA8                        (10L)       
#define PINMUX_PB14K_PCC_DATA8                     ((PIN_PB14K_PCC_DATA8 << 16) | MUX_PB14K_PCC_DATA8)
#define PORT_PB14K_PCC_DATA8                       ((1UL) << 14)

#define PIN_PB15K_PCC_DATA9                        (47L)        
#define MUX_PB15K_PCC_DATA9                        (10L)       
#define PINMUX_PB15K_PCC_DATA9                     ((PIN_PB15K_PCC_DATA9 << 16) | MUX_PB15K_PCC_DATA9)
#define PORT_PB15K_PCC_DATA9                       ((1UL) << 15)

#define PIN_PC12K_PCC_DATA10                       (76L)        
#define MUX_PC12K_PCC_DATA10                       (10L)       
#define PINMUX_PC12K_PCC_DATA10                    ((PIN_PC12K_PCC_DATA10 << 16) | MUX_PC12K_PCC_DATA10)
#define PORT_PC12K_PCC_DATA10                      ((1UL) << 12)

#define PIN_PC13K_PCC_DATA11                       (77L)        
#define MUX_PC13K_PCC_DATA11                       (10L)       
#define PINMUX_PC13K_PCC_DATA11                    ((PIN_PC13K_PCC_DATA11 << 16) | MUX_PC13K_PCC_DATA11)
#define PORT_PC13K_PCC_DATA11                      ((1UL) << 13)

#define PIN_PC14K_PCC_DATA12                       (78L)        
#define MUX_PC14K_PCC_DATA12                       (10L)       
#define PINMUX_PC14K_PCC_DATA12                    ((PIN_PC14K_PCC_DATA12 << 16) | MUX_PC14K_PCC_DATA12)
#define PORT_PC14K_PCC_DATA12                      ((1UL) << 14)

#define PIN_PC15K_PCC_DATA13                       (79L)        
#define MUX_PC15K_PCC_DATA13                       (10L)       
#define PINMUX_PC15K_PCC_DATA13                    ((PIN_PC15K_PCC_DATA13 << 16) | MUX_PC15K_PCC_DATA13)
#define PORT_PC15K_PCC_DATA13                      ((1UL) << 15)

#define PIN_PA12K_PCC_DEN1                         (12L)        
#define MUX_PA12K_PCC_DEN1                         (10L)       
#define PINMUX_PA12K_PCC_DEN1                      ((PIN_PA12K_PCC_DEN1 << 16) | MUX_PA12K_PCC_DEN1)
#define PORT_PA12K_PCC_DEN1                        ((1UL) << 12)

#define PIN_PA13K_PCC_DEN2                         (13L)        
#define MUX_PA13K_PCC_DEN2                         (10L)       
#define PINMUX_PA13K_PCC_DEN2                      ((PIN_PA13K_PCC_DEN2 << 16) | MUX_PA13K_PCC_DEN2)
#define PORT_PA13K_PCC_DEN2                        ((1UL) << 13)

/* ========== PORT definition for PDEC peripheral ========== */
#define PIN_PB18G_PDEC_QDI0                        (50L)        
#define MUX_PB18G_PDEC_QDI0                        (6L)        
#define PINMUX_PB18G_PDEC_QDI0                     ((PIN_PB18G_PDEC_QDI0 << 16) | MUX_PB18G_PDEC_QDI0)
#define PORT_PB18G_PDEC_QDI0                       ((1UL) << 18)

#define PIN_PB23G_PDEC_QDI0                        (55L)        
#define MUX_PB23G_PDEC_QDI0                        (6L)        
#define PINMUX_PB23G_PDEC_QDI0                     ((PIN_PB23G_PDEC_QDI0 << 16) | MUX_PB23G_PDEC_QDI0)
#define PORT_PB23G_PDEC_QDI0                       ((1UL) << 23)

#define PIN_PC16G_PDEC_QDI0                        (80L)        
#define MUX_PC16G_PDEC_QDI0                        (6L)        
#define PINMUX_PC16G_PDEC_QDI0                     ((PIN_PC16G_PDEC_QDI0 << 16) | MUX_PC16G_PDEC_QDI0)
#define PORT_PC16G_PDEC_QDI0                       ((1UL) << 16)

#define PIN_PA24G_PDEC_QDI0                        (24L)        
#define MUX_PA24G_PDEC_QDI0                        (6L)        
#define PINMUX_PA24G_PDEC_QDI0                     ((PIN_PA24G_PDEC_QDI0 << 16) | MUX_PA24G_PDEC_QDI0)
#define PORT_PA24G_PDEC_QDI0                       ((1UL) << 24)

#define PIN_PB19G_PDEC_QDI1                        (51L)        
#define MUX_PB19G_PDEC_QDI1                        (6L)        
#define PINMUX_PB19G_PDEC_QDI1                     ((PIN_PB19G_PDEC_QDI1 << 16) | MUX_PB19G_PDEC_QDI1)
#define PORT_PB19G_PDEC_QDI1                       ((1UL) << 19)

#define PIN_PB24G_PDEC_QDI1                        (56L)        
#define MUX_PB24G_PDEC_QDI1                        (6L)        
#define PINMUX_PB24G_PDEC_QDI1                     ((PIN_PB24G_PDEC_QDI1 << 16) | MUX_PB24G_PDEC_QDI1)
#define PORT_PB24G_PDEC_QDI1                       ((1UL) << 24)

#define PIN_PC17G_PDEC_QDI1                        (81L)        
#define MUX_PC17G_PDEC_QDI1                        (6L)        
#define PINMUX_PC17G_PDEC_QDI1                     ((PIN_PC17G_PDEC_QDI1 << 16) | MUX_PC17G_PDEC_QDI1)
#define PORT_PC17G_PDEC_QDI1                       ((1UL) << 17)

#define PIN_PA25G_PDEC_QDI1                        (25L)        
#define MUX_PA25G_PDEC_QDI1                        (6L)        
#define PINMUX_PA25G_PDEC_QDI1                     ((PIN_PA25G_PDEC_QDI1 << 16) | MUX_PA25G_PDEC_QDI1)
#define PORT_PA25G_PDEC_QDI1                       ((1UL) << 25)

#define PIN_PB20G_PDEC_QDI2                        (52L)        
#define MUX_PB20G_PDEC_QDI2                        (6L)        
#define PINMUX_PB20G_PDEC_QDI2                     ((PIN_PB20G_PDEC_QDI2 << 16) | MUX_PB20G_PDEC_QDI2)
#define PORT_PB20G_PDEC_QDI2                       ((1UL) << 20)

#define PIN_PB25G_PDEC_QDI2                        (57L)        
#define MUX_PB25G_PDEC_QDI2                        (6L)        
#define PINMUX_PB25G_PDEC_QDI2                     ((PIN_PB25G_PDEC_QDI2 << 16) | MUX_PB25G_PDEC_QDI2)
#define PORT_PB25G_PDEC_QDI2                       ((1UL) << 25)

#define PIN_PC18G_PDEC_QDI2                        (82L)        
#define MUX_PC18G_PDEC_QDI2                        (6L)        
#define PINMUX_PC18G_PDEC_QDI2                     ((PIN_PC18G_PDEC_QDI2 << 16) | MUX_PC18G_PDEC_QDI2)
#define PORT_PC18G_PDEC_QDI2                       ((1UL) << 18)

#define PIN_PB22G_PDEC_QDI2                        (54L)        
#define MUX_PB22G_PDEC_QDI2                        (6L)        
#define PINMUX_PB22G_PDEC_QDI2                     ((PIN_PB22G_PDEC_QDI2 << 16) | MUX_PB22G_PDEC_QDI2)
#define PORT_PB22G_PDEC_QDI2                       ((1UL) << 22)

/* ========== PORT definition for QSPI peripheral ========== */
#define PIN_PB11H_QSPI_CS                          (43L)        
#define MUX_PB11H_QSPI_CS                          (7L)        
#define PINMUX_PB11H_QSPI_CS                       ((PIN_PB11H_QSPI_CS << 16) | MUX_PB11H_QSPI_CS)
#define PORT_PB11H_QSPI_CS                         ((1UL) << 11)

#define PIN_PA08H_QSPI_DATA0                       (8L)         
#define MUX_PA08H_QSPI_DATA0                       (7L)        
#define PINMUX_PA08H_QSPI_DATA0                    ((PIN_PA08H_QSPI_DATA0 << 16) | MUX_PA08H_QSPI_DATA0)
#define PORT_PA08H_QSPI_DATA0                      ((1UL) << 8)

#define PIN_PA09H_QSPI_DATA1                       (9L)         
#define MUX_PA09H_QSPI_DATA1                       (7L)        
#define PINMUX_PA09H_QSPI_DATA1                    ((PIN_PA09H_QSPI_DATA1 << 16) | MUX_PA09H_QSPI_DATA1)
#define PORT_PA09H_QSPI_DATA1                      ((1UL) << 9)

#define PIN_PA10H_QSPI_DATA2                       (10L)        
#define MUX_PA10H_QSPI_DATA2                       (7L)        
#define PINMUX_PA10H_QSPI_DATA2                    ((PIN_PA10H_QSPI_DATA2 << 16) | MUX_PA10H_QSPI_DATA2)
#define PORT_PA10H_QSPI_DATA2                      ((1UL) << 10)

#define PIN_PA11H_QSPI_DATA3                       (11L)        
#define MUX_PA11H_QSPI_DATA3                       (7L)        
#define PINMUX_PA11H_QSPI_DATA3                    ((PIN_PA11H_QSPI_DATA3 << 16) | MUX_PA11H_QSPI_DATA3)
#define PORT_PA11H_QSPI_DATA3                      ((1UL) << 11)

#define PIN_PB10H_QSPI_SCK                         (42L)        
#define MUX_PB10H_QSPI_SCK                         (7L)        
#define PINMUX_PB10H_QSPI_SCK                      ((PIN_PB10H_QSPI_SCK << 16) | MUX_PB10H_QSPI_SCK)
#define PORT_PB10H_QSPI_SCK                        ((1UL) << 10)

/* ========== PORT definition for SDHC0 peripheral ========== */
#define PIN_PA06I_SDHC0_SDCD                       (6L)         
#define MUX_PA06I_SDHC0_SDCD                       (8L)        
#define PINMUX_PA06I_SDHC0_SDCD                    ((PIN_PA06I_SDHC0_SDCD << 16) | MUX_PA06I_SDHC0_SDCD)
#define PORT_PA06I_SDHC0_SDCD                      ((1UL) << 6)

#define PIN_PA12I_SDHC0_SDCD                       (12L)        
#define MUX_PA12I_SDHC0_SDCD                       (8L)        
#define PINMUX_PA12I_SDHC0_SDCD                    ((PIN_PA12I_SDHC0_SDCD << 16) | MUX_PA12I_SDHC0_SDCD)
#define PORT_PA12I_SDHC0_SDCD                      ((1UL) << 12)

#define PIN_PB12I_SDHC0_SDCD                       (44L)        
#define MUX_PB12I_SDHC0_SDCD                       (8L)        
#define PINMUX_PB12I_SDHC0_SDCD                    ((PIN_PB12I_SDHC0_SDCD << 16) | MUX_PB12I_SDHC0_SDCD)
#define PORT_PB12I_SDHC0_SDCD                      ((1UL) << 12)

#define PIN_PC06I_SDHC0_SDCD                       (70L)        
#define MUX_PC06I_SDHC0_SDCD                       (8L)        
#define PINMUX_PC06I_SDHC0_SDCD                    ((PIN_PC06I_SDHC0_SDCD << 16) | MUX_PC06I_SDHC0_SDCD)
#define PORT_PC06I_SDHC0_SDCD                      ((1UL) << 6)

#define PIN_PB11I_SDHC0_SDCK                       (43L)        
#define MUX_PB11I_SDHC0_SDCK                       (8L)        
#define PINMUX_PB11I_SDHC0_SDCK                    ((PIN_PB11I_SDHC0_SDCK << 16) | MUX_PB11I_SDHC0_SDCK)
#define PORT_PB11I_SDHC0_SDCK                      ((1UL) << 11)

#define PIN_PA08I_SDHC0_SDCMD                      (8L)         
#define MUX_PA08I_SDHC0_SDCMD                      (8L)        
#define PINMUX_PA08I_SDHC0_SDCMD                   ((PIN_PA08I_SDHC0_SDCMD << 16) | MUX_PA08I_SDHC0_SDCMD)
#define PORT_PA08I_SDHC0_SDCMD                     ((1UL) << 8)

#define PIN_PA09I_SDHC0_SDDAT0                     (9L)         
#define MUX_PA09I_SDHC0_SDDAT0                     (8L)        
#define PINMUX_PA09I_SDHC0_SDDAT0                  ((PIN_PA09I_SDHC0_SDDAT0 << 16) | MUX_PA09I_SDHC0_SDDAT0)
#define PORT_PA09I_SDHC0_SDDAT0                    ((1UL) << 9)

#define PIN_PA10I_SDHC0_SDDAT1                     (10L)        
#define MUX_PA10I_SDHC0_SDDAT1                     (8L)        
#define PINMUX_PA10I_SDHC0_SDDAT1                  ((PIN_PA10I_SDHC0_SDDAT1 << 16) | MUX_PA10I_SDHC0_SDDAT1)
#define PORT_PA10I_SDHC0_SDDAT1                    ((1UL) << 10)

#define PIN_PA11I_SDHC0_SDDAT2                     (11L)        
#define MUX_PA11I_SDHC0_SDDAT2                     (8L)        
#define PINMUX_PA11I_SDHC0_SDDAT2                  ((PIN_PA11I_SDHC0_SDDAT2 << 16) | MUX_PA11I_SDHC0_SDDAT2)
#define PORT_PA11I_SDHC0_SDDAT2                    ((1UL) << 11)

#define PIN_PB10I_SDHC0_SDDAT3                     (42L)        
#define MUX_PB10I_SDHC0_SDDAT3                     (8L)        
#define PINMUX_PB10I_SDHC0_SDDAT3                  ((PIN_PB10I_SDHC0_SDDAT3 << 16) | MUX_PB10I_SDHC0_SDDAT3)
#define PORT_PB10I_SDHC0_SDDAT3                    ((1UL) << 10)

#define PIN_PA07I_SDHC0_SDWP                       (7L)         
#define MUX_PA07I_SDHC0_SDWP                       (8L)        
#define PINMUX_PA07I_SDHC0_SDWP                    ((PIN_PA07I_SDHC0_SDWP << 16) | MUX_PA07I_SDHC0_SDWP)
#define PORT_PA07I_SDHC0_SDWP                      ((1UL) << 7)

#define PIN_PA13I_SDHC0_SDWP                       (13L)        
#define MUX_PA13I_SDHC0_SDWP                       (8L)        
#define PINMUX_PA13I_SDHC0_SDWP                    ((PIN_PA13I_SDHC0_SDWP << 16) | MUX_PA13I_SDHC0_SDWP)
#define PORT_PA13I_SDHC0_SDWP                      ((1UL) << 13)

#define PIN_PB13I_SDHC0_SDWP                       (45L)        
#define MUX_PB13I_SDHC0_SDWP                       (8L)        
#define PINMUX_PB13I_SDHC0_SDWP                    ((PIN_PB13I_SDHC0_SDWP << 16) | MUX_PB13I_SDHC0_SDWP)
#define PORT_PB13I_SDHC0_SDWP                      ((1UL) << 13)

#define PIN_PC07I_SDHC0_SDWP                       (71L)        
#define MUX_PC07I_SDHC0_SDWP                       (8L)        
#define PINMUX_PC07I_SDHC0_SDWP                    ((PIN_PC07I_SDHC0_SDWP << 16) | MUX_PC07I_SDHC0_SDWP)
#define PORT_PC07I_SDHC0_SDWP                      ((1UL) << 7)

/* ========== PORT definition for SDHC1 peripheral ========== */
#define PIN_PB16I_SDHC1_SDCD                       (48L)        
#define MUX_PB16I_SDHC1_SDCD                       (8L)        
#define PINMUX_PB16I_SDHC1_SDCD                    ((PIN_PB16I_SDHC1_SDCD << 16) | MUX_PB16I_SDHC1_SDCD)
#define PORT_PB16I_SDHC1_SDCD                      ((1UL) << 16)

#define PIN_PC20I_SDHC1_SDCD                       (84L)        
#define MUX_PC20I_SDHC1_SDCD                       (8L)        
#define PINMUX_PC20I_SDHC1_SDCD                    ((PIN_PC20I_SDHC1_SDCD << 16) | MUX_PC20I_SDHC1_SDCD)
#define PORT_PC20I_SDHC1_SDCD                      ((1UL) << 20)

#define PIN_PD20I_SDHC1_SDCD                       (116L)       
#define MUX_PD20I_SDHC1_SDCD                       (8L)        
#define PINMUX_PD20I_SDHC1_SDCD                    ((PIN_PD20I_SDHC1_SDCD << 16) | MUX_PD20I_SDHC1_SDCD)
#define PORT_PD20I_SDHC1_SDCD                      ((1UL) << 20)

#define PIN_PA21I_SDHC1_SDCK                       (21L)        
#define MUX_PA21I_SDHC1_SDCK                       (8L)        
#define PINMUX_PA21I_SDHC1_SDCK                    ((PIN_PA21I_SDHC1_SDCK << 16) | MUX_PA21I_SDHC1_SDCK)
#define PORT_PA21I_SDHC1_SDCK                      ((1UL) << 21)

#define PIN_PA20I_SDHC1_SDCMD                      (20L)        
#define MUX_PA20I_SDHC1_SDCMD                      (8L)        
#define PINMUX_PA20I_SDHC1_SDCMD                   ((PIN_PA20I_SDHC1_SDCMD << 16) | MUX_PA20I_SDHC1_SDCMD)
#define PORT_PA20I_SDHC1_SDCMD                     ((1UL) << 20)

#define PIN_PB18I_SDHC1_SDDAT0                     (50L)        
#define MUX_PB18I_SDHC1_SDDAT0                     (8L)        
#define PINMUX_PB18I_SDHC1_SDDAT0                  ((PIN_PB18I_SDHC1_SDDAT0 << 16) | MUX_PB18I_SDHC1_SDDAT0)
#define PORT_PB18I_SDHC1_SDDAT0                    ((1UL) << 18)

#define PIN_PB19I_SDHC1_SDDAT1                     (51L)        
#define MUX_PB19I_SDHC1_SDDAT1                     (8L)        
#define PINMUX_PB19I_SDHC1_SDDAT1                  ((PIN_PB19I_SDHC1_SDDAT1 << 16) | MUX_PB19I_SDHC1_SDDAT1)
#define PORT_PB19I_SDHC1_SDDAT1                    ((1UL) << 19)

#define PIN_PB20I_SDHC1_SDDAT2                     (52L)        
#define MUX_PB20I_SDHC1_SDDAT2                     (8L)        
#define PINMUX_PB20I_SDHC1_SDDAT2                  ((PIN_PB20I_SDHC1_SDDAT2 << 16) | MUX_PB20I_SDHC1_SDDAT2)
#define PORT_PB20I_SDHC1_SDDAT2                    ((1UL) << 20)

#define PIN_PB21I_SDHC1_SDDAT3                     (53L)        
#define MUX_PB21I_SDHC1_SDDAT3                     (8L)        
#define PINMUX_PB21I_SDHC1_SDDAT3                  ((PIN_PB21I_SDHC1_SDDAT3 << 16) | MUX_PB21I_SDHC1_SDDAT3)
#define PORT_PB21I_SDHC1_SDDAT3                    ((1UL) << 21)

#define PIN_PB17I_SDHC1_SDWP                       (49L)        
#define MUX_PB17I_SDHC1_SDWP                       (8L)        
#define PINMUX_PB17I_SDHC1_SDWP                    ((PIN_PB17I_SDHC1_SDWP << 16) | MUX_PB17I_SDHC1_SDWP)
#define PORT_PB17I_SDHC1_SDWP                      ((1UL) << 17)

#define PIN_PC21I_SDHC1_SDWP                       (85L)        
#define MUX_PC21I_SDHC1_SDWP                       (8L)        
#define PINMUX_PC21I_SDHC1_SDWP                    ((PIN_PC21I_SDHC1_SDWP << 16) | MUX_PC21I_SDHC1_SDWP)
#define PORT_PC21I_SDHC1_SDWP                      ((1UL) << 21)

#define PIN_PD21I_SDHC1_SDWP                       (117L)       
#define MUX_PD21I_SDHC1_SDWP                       (8L)        
#define PINMUX_PD21I_SDHC1_SDWP                    ((PIN_PD21I_SDHC1_SDWP << 16) | MUX_PD21I_SDHC1_SDWP)
#define PORT_PD21I_SDHC1_SDWP                      ((1UL) << 21)

/* ========== PORT definition for SERCOM0 peripheral ========== */
#define PIN_PA04D_SERCOM0_PAD0                     (4L)         
#define MUX_PA04D_SERCOM0_PAD0                     (3L)        
#define PINMUX_PA04D_SERCOM0_PAD0                  ((PIN_PA04D_SERCOM0_PAD0 << 16) | MUX_PA04D_SERCOM0_PAD0)
#define PORT_PA04D_SERCOM0_PAD0                    ((1UL) << 4)

#define PIN_PC17D_SERCOM0_PAD0                     (81L)        
#define MUX_PC17D_SERCOM0_PAD0                     (3L)        
#define PINMUX_PC17D_SERCOM0_PAD0                  ((PIN_PC17D_SERCOM0_PAD0 << 16) | MUX_PC17D_SERCOM0_PAD0)
#define PORT_PC17D_SERCOM0_PAD0                    ((1UL) << 17)

#define PIN_PA08C_SERCOM0_PAD0                     (8L)         
#define MUX_PA08C_SERCOM0_PAD0                     (2L)        
#define PINMUX_PA08C_SERCOM0_PAD0                  ((PIN_PA08C_SERCOM0_PAD0 << 16) | MUX_PA08C_SERCOM0_PAD0)
#define PORT_PA08C_SERCOM0_PAD0                    ((1UL) << 8)

#define PIN_PB24C_SERCOM0_PAD0                     (56L)        
#define MUX_PB24C_SERCOM0_PAD0                     (2L)        
#define PINMUX_PB24C_SERCOM0_PAD0                  ((PIN_PB24C_SERCOM0_PAD0 << 16) | MUX_PB24C_SERCOM0_PAD0)
#define PORT_PB24C_SERCOM0_PAD0                    ((1UL) << 24)

#define PIN_PA05D_SERCOM0_PAD1                     (5L)         
#define MUX_PA05D_SERCOM0_PAD1                     (3L)        
#define PINMUX_PA05D_SERCOM0_PAD1                  ((PIN_PA05D_SERCOM0_PAD1 << 16) | MUX_PA05D_SERCOM0_PAD1)
#define PORT_PA05D_SERCOM0_PAD1                    ((1UL) << 5)

#define PIN_PC16D_SERCOM0_PAD1                     (80L)        
#define MUX_PC16D_SERCOM0_PAD1                     (3L)        
#define PINMUX_PC16D_SERCOM0_PAD1                  ((PIN_PC16D_SERCOM0_PAD1 << 16) | MUX_PC16D_SERCOM0_PAD1)
#define PORT_PC16D_SERCOM0_PAD1                    ((1UL) << 16)

#define PIN_PA09C_SERCOM0_PAD1                     (9L)         
#define MUX_PA09C_SERCOM0_PAD1                     (2L)        
#define PINMUX_PA09C_SERCOM0_PAD1                  ((PIN_PA09C_SERCOM0_PAD1 << 16) | MUX_PA09C_SERCOM0_PAD1)
#define PORT_PA09C_SERCOM0_PAD1                    ((1UL) << 9)

#define PIN_PB25C_SERCOM0_PAD1                     (57L)        
#define MUX_PB25C_SERCOM0_PAD1                     (2L)        
#define PINMUX_PB25C_SERCOM0_PAD1                  ((PIN_PB25C_SERCOM0_PAD1 << 16) | MUX_PB25C_SERCOM0_PAD1)
#define PORT_PB25C_SERCOM0_PAD1                    ((1UL) << 25)

#define PIN_PA06D_SERCOM0_PAD2                     (6L)         
#define MUX_PA06D_SERCOM0_PAD2                     (3L)        
#define PINMUX_PA06D_SERCOM0_PAD2                  ((PIN_PA06D_SERCOM0_PAD2 << 16) | MUX_PA06D_SERCOM0_PAD2)
#define PORT_PA06D_SERCOM0_PAD2                    ((1UL) << 6)

#define PIN_PC18D_SERCOM0_PAD2                     (82L)        
#define MUX_PC18D_SERCOM0_PAD2                     (3L)        
#define PINMUX_PC18D_SERCOM0_PAD2                  ((PIN_PC18D_SERCOM0_PAD2 << 16) | MUX_PC18D_SERCOM0_PAD2)
#define PORT_PC18D_SERCOM0_PAD2                    ((1UL) << 18)

#define PIN_PA10C_SERCOM0_PAD2                     (10L)        
#define MUX_PA10C_SERCOM0_PAD2                     (2L)        
#define PINMUX_PA10C_SERCOM0_PAD2                  ((PIN_PA10C_SERCOM0_PAD2 << 16) | MUX_PA10C_SERCOM0_PAD2)
#define PORT_PA10C_SERCOM0_PAD2                    ((1UL) << 10)

#define PIN_PC24C_SERCOM0_PAD2                     (88L)        
#define MUX_PC24C_SERCOM0_PAD2                     (2L)        
#define PINMUX_PC24C_SERCOM0_PAD2                  ((PIN_PC24C_SERCOM0_PAD2 << 16) | MUX_PC24C_SERCOM0_PAD2)
#define PORT_PC24C_SERCOM0_PAD2                    ((1UL) << 24)

#define PIN_PA07D_SERCOM0_PAD3                     (7L)         
#define MUX_PA07D_SERCOM0_PAD3                     (3L)        
#define PINMUX_PA07D_SERCOM0_PAD3                  ((PIN_PA07D_SERCOM0_PAD3 << 16) | MUX_PA07D_SERCOM0_PAD3)
#define PORT_PA07D_SERCOM0_PAD3                    ((1UL) << 7)

#define PIN_PC19D_SERCOM0_PAD3                     (83L)        
#define MUX_PC19D_SERCOM0_PAD3                     (3L)        
#define PINMUX_PC19D_SERCOM0_PAD3                  ((PIN_PC19D_SERCOM0_PAD3 << 16) | MUX_PC19D_SERCOM0_PAD3)
#define PORT_PC19D_SERCOM0_PAD3                    ((1UL) << 19)

#define PIN_PA11C_SERCOM0_PAD3                     (11L)        
#define MUX_PA11C_SERCOM0_PAD3                     (2L)        
#define PINMUX_PA11C_SERCOM0_PAD3                  ((PIN_PA11C_SERCOM0_PAD3 << 16) | MUX_PA11C_SERCOM0_PAD3)
#define PORT_PA11C_SERCOM0_PAD3                    ((1UL) << 11)

#define PIN_PC25C_SERCOM0_PAD3                     (89L)        
#define MUX_PC25C_SERCOM0_PAD3                     (2L)        
#define PINMUX_PC25C_SERCOM0_PAD3                  ((PIN_PC25C_SERCOM0_PAD3 << 16) | MUX_PC25C_SERCOM0_PAD3)
#define PORT_PC25C_SERCOM0_PAD3                    ((1UL) << 25)

/* ========== PORT definition for SERCOM1 peripheral ========== */
#define PIN_PA00D_SERCOM1_PAD0                     (0L)         
#define MUX_PA00D_SERCOM1_PAD0                     (3L)        
#define PINMUX_PA00D_SERCOM1_PAD0                  ((PIN_PA00D_SERCOM1_PAD0 << 16) | MUX_PA00D_SERCOM1_PAD0)
#define PORT_PA00D_SERCOM1_PAD0                    ((1UL) << 0)

#define PIN_PA16C_SERCOM1_PAD0                     (16L)        
#define MUX_PA16C_SERCOM1_PAD0                     (2L)        
#define PINMUX_PA16C_SERCOM1_PAD0                  ((PIN_PA16C_SERCOM1_PAD0 << 16) | MUX_PA16C_SERCOM1_PAD0)
#define PORT_PA16C_SERCOM1_PAD0                    ((1UL) << 16)

#define PIN_PC22C_SERCOM1_PAD0                     (86L)        
#define MUX_PC22C_SERCOM1_PAD0                     (2L)        
#define PINMUX_PC22C_SERCOM1_PAD0                  ((PIN_PC22C_SERCOM1_PAD0 << 16) | MUX_PC22C_SERCOM1_PAD0)
#define PORT_PC22C_SERCOM1_PAD0                    ((1UL) << 22)

#define PIN_PC27C_SERCOM1_PAD0                     (91L)        
#define MUX_PC27C_SERCOM1_PAD0                     (2L)        
#define PINMUX_PC27C_SERCOM1_PAD0                  ((PIN_PC27C_SERCOM1_PAD0 << 16) | MUX_PC27C_SERCOM1_PAD0)
#define PORT_PC27C_SERCOM1_PAD0                    ((1UL) << 27)

#define PIN_PA01D_SERCOM1_PAD1                     (1L)         
#define MUX_PA01D_SERCOM1_PAD1                     (3L)        
#define PINMUX_PA01D_SERCOM1_PAD1                  ((PIN_PA01D_SERCOM1_PAD1 << 16) | MUX_PA01D_SERCOM1_PAD1)
#define PORT_PA01D_SERCOM1_PAD1                    ((1UL) << 1)

#define PIN_PA17C_SERCOM1_PAD1                     (17L)        
#define MUX_PA17C_SERCOM1_PAD1                     (2L)        
#define PINMUX_PA17C_SERCOM1_PAD1                  ((PIN_PA17C_SERCOM1_PAD1 << 16) | MUX_PA17C_SERCOM1_PAD1)
#define PORT_PA17C_SERCOM1_PAD1                    ((1UL) << 17)

#define PIN_PC23C_SERCOM1_PAD1                     (87L)        
#define MUX_PC23C_SERCOM1_PAD1                     (2L)        
#define PINMUX_PC23C_SERCOM1_PAD1                  ((PIN_PC23C_SERCOM1_PAD1 << 16) | MUX_PC23C_SERCOM1_PAD1)
#define PORT_PC23C_SERCOM1_PAD1                    ((1UL) << 23)

#define PIN_PC28C_SERCOM1_PAD1                     (92L)        
#define MUX_PC28C_SERCOM1_PAD1                     (2L)        
#define PINMUX_PC28C_SERCOM1_PAD1                  ((PIN_PC28C_SERCOM1_PAD1 << 16) | MUX_PC28C_SERCOM1_PAD1)
#define PORT_PC28C_SERCOM1_PAD1                    ((1UL) << 28)

#define PIN_PA30D_SERCOM1_PAD2                     (30L)        
#define MUX_PA30D_SERCOM1_PAD2                     (3L)        
#define PINMUX_PA30D_SERCOM1_PAD2                  ((PIN_PA30D_SERCOM1_PAD2 << 16) | MUX_PA30D_SERCOM1_PAD2)
#define PORT_PA30D_SERCOM1_PAD2                    ((1UL) << 30)

#define PIN_PA18C_SERCOM1_PAD2                     (18L)        
#define MUX_PA18C_SERCOM1_PAD2                     (2L)        
#define PINMUX_PA18C_SERCOM1_PAD2                  ((PIN_PA18C_SERCOM1_PAD2 << 16) | MUX_PA18C_SERCOM1_PAD2)
#define PORT_PA18C_SERCOM1_PAD2                    ((1UL) << 18)

#define PIN_PB22C_SERCOM1_PAD2                     (54L)        
#define MUX_PB22C_SERCOM1_PAD2                     (2L)        
#define PINMUX_PB22C_SERCOM1_PAD2                  ((PIN_PB22C_SERCOM1_PAD2 << 16) | MUX_PB22C_SERCOM1_PAD2)
#define PORT_PB22C_SERCOM1_PAD2                    ((1UL) << 22)

#define PIN_PD20C_SERCOM1_PAD2                     (116L)       
#define MUX_PD20C_SERCOM1_PAD2                     (2L)        
#define PINMUX_PD20C_SERCOM1_PAD2                  ((PIN_PD20C_SERCOM1_PAD2 << 16) | MUX_PD20C_SERCOM1_PAD2)
#define PORT_PD20C_SERCOM1_PAD2                    ((1UL) << 20)

#define PIN_PA31D_SERCOM1_PAD3                     (31L)        
#define MUX_PA31D_SERCOM1_PAD3                     (3L)        
#define PINMUX_PA31D_SERCOM1_PAD3                  ((PIN_PA31D_SERCOM1_PAD3 << 16) | MUX_PA31D_SERCOM1_PAD3)
#define PORT_PA31D_SERCOM1_PAD3                    ((1UL) << 31)

#define PIN_PA19C_SERCOM1_PAD3                     (19L)        
#define MUX_PA19C_SERCOM1_PAD3                     (2L)        
#define PINMUX_PA19C_SERCOM1_PAD3                  ((PIN_PA19C_SERCOM1_PAD3 << 16) | MUX_PA19C_SERCOM1_PAD3)
#define PORT_PA19C_SERCOM1_PAD3                    ((1UL) << 19)

#define PIN_PB23C_SERCOM1_PAD3                     (55L)        
#define MUX_PB23C_SERCOM1_PAD3                     (2L)        
#define PINMUX_PB23C_SERCOM1_PAD3                  ((PIN_PB23C_SERCOM1_PAD3 << 16) | MUX_PB23C_SERCOM1_PAD3)
#define PORT_PB23C_SERCOM1_PAD3                    ((1UL) << 23)

#define PIN_PD21C_SERCOM1_PAD3                     (117L)       
#define MUX_PD21C_SERCOM1_PAD3                     (2L)        
#define PINMUX_PD21C_SERCOM1_PAD3                  ((PIN_PD21C_SERCOM1_PAD3 << 16) | MUX_PD21C_SERCOM1_PAD3)
#define PORT_PD21C_SERCOM1_PAD3                    ((1UL) << 21)

/* ========== PORT definition for SERCOM2 peripheral ========== */
#define PIN_PA09D_SERCOM2_PAD0                     (9L)         
#define MUX_PA09D_SERCOM2_PAD0                     (3L)        
#define PINMUX_PA09D_SERCOM2_PAD0                  ((PIN_PA09D_SERCOM2_PAD0 << 16) | MUX_PA09D_SERCOM2_PAD0)
#define PORT_PA09D_SERCOM2_PAD0                    ((1UL) << 9)

#define PIN_PB25D_SERCOM2_PAD0                     (57L)        
#define MUX_PB25D_SERCOM2_PAD0                     (3L)        
#define PINMUX_PB25D_SERCOM2_PAD0                  ((PIN_PB25D_SERCOM2_PAD0 << 16) | MUX_PB25D_SERCOM2_PAD0)
#define PORT_PB25D_SERCOM2_PAD0                    ((1UL) << 25)

#define PIN_PA12C_SERCOM2_PAD0                     (12L)        
#define MUX_PA12C_SERCOM2_PAD0                     (2L)        
#define PINMUX_PA12C_SERCOM2_PAD0                  ((PIN_PA12C_SERCOM2_PAD0 << 16) | MUX_PA12C_SERCOM2_PAD0)
#define PORT_PA12C_SERCOM2_PAD0                    ((1UL) << 12)

#define PIN_PB26C_SERCOM2_PAD0                     (58L)        
#define MUX_PB26C_SERCOM2_PAD0                     (2L)        
#define PINMUX_PB26C_SERCOM2_PAD0                  ((PIN_PB26C_SERCOM2_PAD0 << 16) | MUX_PB26C_SERCOM2_PAD0)
#define PORT_PB26C_SERCOM2_PAD0                    ((1UL) << 26)

#define PIN_PA08D_SERCOM2_PAD1                     (8L)         
#define MUX_PA08D_SERCOM2_PAD1                     (3L)        
#define PINMUX_PA08D_SERCOM2_PAD1                  ((PIN_PA08D_SERCOM2_PAD1 << 16) | MUX_PA08D_SERCOM2_PAD1)
#define PORT_PA08D_SERCOM2_PAD1                    ((1UL) << 8)

#define PIN_PB24D_SERCOM2_PAD1                     (56L)        
#define MUX_PB24D_SERCOM2_PAD1                     (3L)        
#define PINMUX_PB24D_SERCOM2_PAD1                  ((PIN_PB24D_SERCOM2_PAD1 << 16) | MUX_PB24D_SERCOM2_PAD1)
#define PORT_PB24D_SERCOM2_PAD1                    ((1UL) << 24)

#define PIN_PA13C_SERCOM2_PAD1                     (13L)        
#define MUX_PA13C_SERCOM2_PAD1                     (2L)        
#define PINMUX_PA13C_SERCOM2_PAD1                  ((PIN_PA13C_SERCOM2_PAD1 << 16) | MUX_PA13C_SERCOM2_PAD1)
#define PORT_PA13C_SERCOM2_PAD1                    ((1UL) << 13)

#define PIN_PB27C_SERCOM2_PAD1                     (59L)        
#define MUX_PB27C_SERCOM2_PAD1                     (2L)        
#define PINMUX_PB27C_SERCOM2_PAD1                  ((PIN_PB27C_SERCOM2_PAD1 << 16) | MUX_PB27C_SERCOM2_PAD1)
#define PORT_PB27C_SERCOM2_PAD1                    ((1UL) << 27)

#define PIN_PA10D_SERCOM2_PAD2                     (10L)        
#define MUX_PA10D_SERCOM2_PAD2                     (3L)        
#define PINMUX_PA10D_SERCOM2_PAD2                  ((PIN_PA10D_SERCOM2_PAD2 << 16) | MUX_PA10D_SERCOM2_PAD2)
#define PORT_PA10D_SERCOM2_PAD2                    ((1UL) << 10)

#define PIN_PC24D_SERCOM2_PAD2                     (88L)        
#define MUX_PC24D_SERCOM2_PAD2                     (3L)        
#define PINMUX_PC24D_SERCOM2_PAD2                  ((PIN_PC24D_SERCOM2_PAD2 << 16) | MUX_PC24D_SERCOM2_PAD2)
#define PORT_PC24D_SERCOM2_PAD2                    ((1UL) << 24)

#define PIN_PB28C_SERCOM2_PAD2                     (60L)        
#define MUX_PB28C_SERCOM2_PAD2                     (2L)        
#define PINMUX_PB28C_SERCOM2_PAD2                  ((PIN_PB28C_SERCOM2_PAD2 << 16) | MUX_PB28C_SERCOM2_PAD2)
#define PORT_PB28C_SERCOM2_PAD2                    ((1UL) << 28)

#define PIN_PA14C_SERCOM2_PAD2                     (14L)        
#define MUX_PA14C_SERCOM2_PAD2                     (2L)        
#define PINMUX_PA14C_SERCOM2_PAD2                  ((PIN_PA14C_SERCOM2_PAD2 << 16) | MUX_PA14C_SERCOM2_PAD2)
#define PORT_PA14C_SERCOM2_PAD2                    ((1UL) << 14)

#define PIN_PA11D_SERCOM2_PAD3                     (11L)        
#define MUX_PA11D_SERCOM2_PAD3                     (3L)        
#define PINMUX_PA11D_SERCOM2_PAD3                  ((PIN_PA11D_SERCOM2_PAD3 << 16) | MUX_PA11D_SERCOM2_PAD3)
#define PORT_PA11D_SERCOM2_PAD3                    ((1UL) << 11)

#define PIN_PC25D_SERCOM2_PAD3                     (89L)        
#define MUX_PC25D_SERCOM2_PAD3                     (3L)        
#define PINMUX_PC25D_SERCOM2_PAD3                  ((PIN_PC25D_SERCOM2_PAD3 << 16) | MUX_PC25D_SERCOM2_PAD3)
#define PORT_PC25D_SERCOM2_PAD3                    ((1UL) << 25)

#define PIN_PB29C_SERCOM2_PAD3                     (61L)        
#define MUX_PB29C_SERCOM2_PAD3                     (2L)        
#define PINMUX_PB29C_SERCOM2_PAD3                  ((PIN_PB29C_SERCOM2_PAD3 << 16) | MUX_PB29C_SERCOM2_PAD3)
#define PORT_PB29C_SERCOM2_PAD3                    ((1UL) << 29)

#define PIN_PA15C_SERCOM2_PAD3                     (15L)        
#define MUX_PA15C_SERCOM2_PAD3                     (2L)        
#define PINMUX_PA15C_SERCOM2_PAD3                  ((PIN_PA15C_SERCOM2_PAD3 << 16) | MUX_PA15C_SERCOM2_PAD3)
#define PORT_PA15C_SERCOM2_PAD3                    ((1UL) << 15)

/* ========== PORT definition for SERCOM3 peripheral ========== */
#define PIN_PA17D_SERCOM3_PAD0                     (17L)        
#define MUX_PA17D_SERCOM3_PAD0                     (3L)        
#define PINMUX_PA17D_SERCOM3_PAD0                  ((PIN_PA17D_SERCOM3_PAD0 << 16) | MUX_PA17D_SERCOM3_PAD0)
#define PORT_PA17D_SERCOM3_PAD0                    ((1UL) << 17)

#define PIN_PC23D_SERCOM3_PAD0                     (87L)        
#define MUX_PC23D_SERCOM3_PAD0                     (3L)        
#define PINMUX_PC23D_SERCOM3_PAD0                  ((PIN_PC23D_SERCOM3_PAD0 << 16) | MUX_PC23D_SERCOM3_PAD0)
#define PORT_PC23D_SERCOM3_PAD0                    ((1UL) << 23)

#define PIN_PA22C_SERCOM3_PAD0                     (22L)        
#define MUX_PA22C_SERCOM3_PAD0                     (2L)        
#define PINMUX_PA22C_SERCOM3_PAD0                  ((PIN_PA22C_SERCOM3_PAD0 << 16) | MUX_PA22C_SERCOM3_PAD0)
#define PORT_PA22C_SERCOM3_PAD0                    ((1UL) << 22)

#define PIN_PB20C_SERCOM3_PAD0                     (52L)        
#define MUX_PB20C_SERCOM3_PAD0                     (2L)        
#define PINMUX_PB20C_SERCOM3_PAD0                  ((PIN_PB20C_SERCOM3_PAD0 << 16) | MUX_PB20C_SERCOM3_PAD0)
#define PORT_PB20C_SERCOM3_PAD0                    ((1UL) << 20)

#define PIN_PA16D_SERCOM3_PAD1                     (16L)        
#define MUX_PA16D_SERCOM3_PAD1                     (3L)        
#define PINMUX_PA16D_SERCOM3_PAD1                  ((PIN_PA16D_SERCOM3_PAD1 << 16) | MUX_PA16D_SERCOM3_PAD1)
#define PORT_PA16D_SERCOM3_PAD1                    ((1UL) << 16)

#define PIN_PC22D_SERCOM3_PAD1                     (86L)        
#define MUX_PC22D_SERCOM3_PAD1                     (3L)        
#define PINMUX_PC22D_SERCOM3_PAD1                  ((PIN_PC22D_SERCOM3_PAD1 << 16) | MUX_PC22D_SERCOM3_PAD1)
#define PORT_PC22D_SERCOM3_PAD1                    ((1UL) << 22)

#define PIN_PA23C_SERCOM3_PAD1                     (23L)        
#define MUX_PA23C_SERCOM3_PAD1                     (2L)        
#define PINMUX_PA23C_SERCOM3_PAD1                  ((PIN_PA23C_SERCOM3_PAD1 << 16) | MUX_PA23C_SERCOM3_PAD1)
#define PORT_PA23C_SERCOM3_PAD1                    ((1UL) << 23)

#define PIN_PB21C_SERCOM3_PAD1                     (53L)        
#define MUX_PB21C_SERCOM3_PAD1                     (2L)        
#define PINMUX_PB21C_SERCOM3_PAD1                  ((PIN_PB21C_SERCOM3_PAD1 << 16) | MUX_PB21C_SERCOM3_PAD1)
#define PORT_PB21C_SERCOM3_PAD1                    ((1UL) << 21)

#define PIN_PA18D_SERCOM3_PAD2                     (18L)        
#define MUX_PA18D_SERCOM3_PAD2                     (3L)        
#define PINMUX_PA18D_SERCOM3_PAD2                  ((PIN_PA18D_SERCOM3_PAD2 << 16) | MUX_PA18D_SERCOM3_PAD2)
#define PORT_PA18D_SERCOM3_PAD2                    ((1UL) << 18)

#define PIN_PA20D_SERCOM3_PAD2                     (20L)        
#define MUX_PA20D_SERCOM3_PAD2                     (3L)        
#define PINMUX_PA20D_SERCOM3_PAD2                  ((PIN_PA20D_SERCOM3_PAD2 << 16) | MUX_PA20D_SERCOM3_PAD2)
#define PORT_PA20D_SERCOM3_PAD2                    ((1UL) << 20)

#define PIN_PD20D_SERCOM3_PAD2                     (116L)       
#define MUX_PD20D_SERCOM3_PAD2                     (3L)        
#define PINMUX_PD20D_SERCOM3_PAD2                  ((PIN_PD20D_SERCOM3_PAD2 << 16) | MUX_PD20D_SERCOM3_PAD2)
#define PORT_PD20D_SERCOM3_PAD2                    ((1UL) << 20)

#define PIN_PA24C_SERCOM3_PAD2                     (24L)        
#define MUX_PA24C_SERCOM3_PAD2                     (2L)        
#define PINMUX_PA24C_SERCOM3_PAD2                  ((PIN_PA24C_SERCOM3_PAD2 << 16) | MUX_PA24C_SERCOM3_PAD2)
#define PORT_PA24C_SERCOM3_PAD2                    ((1UL) << 24)

#define PIN_PA19D_SERCOM3_PAD3                     (19L)        
#define MUX_PA19D_SERCOM3_PAD3                     (3L)        
#define PINMUX_PA19D_SERCOM3_PAD3                  ((PIN_PA19D_SERCOM3_PAD3 << 16) | MUX_PA19D_SERCOM3_PAD3)
#define PORT_PA19D_SERCOM3_PAD3                    ((1UL) << 19)

#define PIN_PA21D_SERCOM3_PAD3                     (21L)        
#define MUX_PA21D_SERCOM3_PAD3                     (3L)        
#define PINMUX_PA21D_SERCOM3_PAD3                  ((PIN_PA21D_SERCOM3_PAD3 << 16) | MUX_PA21D_SERCOM3_PAD3)
#define PORT_PA21D_SERCOM3_PAD3                    ((1UL) << 21)

#define PIN_PD21D_SERCOM3_PAD3                     (117L)       
#define MUX_PD21D_SERCOM3_PAD3                     (3L)        
#define PINMUX_PD21D_SERCOM3_PAD3                  ((PIN_PD21D_SERCOM3_PAD3 << 16) | MUX_PD21D_SERCOM3_PAD3)
#define PORT_PD21D_SERCOM3_PAD3                    ((1UL) << 21)

#define PIN_PA25C_SERCOM3_PAD3                     (25L)        
#define MUX_PA25C_SERCOM3_PAD3                     (2L)        
#define PINMUX_PA25C_SERCOM3_PAD3                  ((PIN_PA25C_SERCOM3_PAD3 << 16) | MUX_PA25C_SERCOM3_PAD3)
#define PORT_PA25C_SERCOM3_PAD3                    ((1UL) << 25)

/* ========== PORT definition for SERCOM4 peripheral ========== */
#define PIN_PA13D_SERCOM4_PAD0                     (13L)        
#define MUX_PA13D_SERCOM4_PAD0                     (3L)        
#define PINMUX_PA13D_SERCOM4_PAD0                  ((PIN_PA13D_SERCOM4_PAD0 << 16) | MUX_PA13D_SERCOM4_PAD0)
#define PORT_PA13D_SERCOM4_PAD0                    ((1UL) << 13)

#define PIN_PB08D_SERCOM4_PAD0                     (40L)        
#define MUX_PB08D_SERCOM4_PAD0                     (3L)        
#define PINMUX_PB08D_SERCOM4_PAD0                  ((PIN_PB08D_SERCOM4_PAD0 << 16) | MUX_PB08D_SERCOM4_PAD0)
#define PORT_PB08D_SERCOM4_PAD0                    ((1UL) << 8)

#define PIN_PB27D_SERCOM4_PAD0                     (59L)        
#define MUX_PB27D_SERCOM4_PAD0                     (3L)        
#define PINMUX_PB27D_SERCOM4_PAD0                  ((PIN_PB27D_SERCOM4_PAD0 << 16) | MUX_PB27D_SERCOM4_PAD0)
#define PORT_PB27D_SERCOM4_PAD0                    ((1UL) << 27)

#define PIN_PB12C_SERCOM4_PAD0                     (44L)        
#define MUX_PB12C_SERCOM4_PAD0                     (2L)        
#define PINMUX_PB12C_SERCOM4_PAD0                  ((PIN_PB12C_SERCOM4_PAD0 << 16) | MUX_PB12C_SERCOM4_PAD0)
#define PORT_PB12C_SERCOM4_PAD0                    ((1UL) << 12)

#define PIN_PA12D_SERCOM4_PAD1                     (12L)        
#define MUX_PA12D_SERCOM4_PAD1                     (3L)        
#define PINMUX_PA12D_SERCOM4_PAD1                  ((PIN_PA12D_SERCOM4_PAD1 << 16) | MUX_PA12D_SERCOM4_PAD1)
#define PORT_PA12D_SERCOM4_PAD1                    ((1UL) << 12)

#define PIN_PB09D_SERCOM4_PAD1                     (41L)        
#define MUX_PB09D_SERCOM4_PAD1                     (3L)        
#define PINMUX_PB09D_SERCOM4_PAD1                  ((PIN_PB09D_SERCOM4_PAD1 << 16) | MUX_PB09D_SERCOM4_PAD1)
#define PORT_PB09D_SERCOM4_PAD1                    ((1UL) << 9)

#define PIN_PB26D_SERCOM4_PAD1                     (58L)        
#define MUX_PB26D_SERCOM4_PAD1                     (3L)        
#define PINMUX_PB26D_SERCOM4_PAD1                  ((PIN_PB26D_SERCOM4_PAD1 << 16) | MUX_PB26D_SERCOM4_PAD1)
#define PORT_PB26D_SERCOM4_PAD1                    ((1UL) << 26)

#define PIN_PB13C_SERCOM4_PAD1                     (45L)        
#define MUX_PB13C_SERCOM4_PAD1                     (2L)        
#define PINMUX_PB13C_SERCOM4_PAD1                  ((PIN_PB13C_SERCOM4_PAD1 << 16) | MUX_PB13C_SERCOM4_PAD1)
#define PORT_PB13C_SERCOM4_PAD1                    ((1UL) << 13)

#define PIN_PA14D_SERCOM4_PAD2                     (14L)        
#define MUX_PA14D_SERCOM4_PAD2                     (3L)        
#define PINMUX_PA14D_SERCOM4_PAD2                  ((PIN_PA14D_SERCOM4_PAD2 << 16) | MUX_PA14D_SERCOM4_PAD2)
#define PORT_PA14D_SERCOM4_PAD2                    ((1UL) << 14)

#define PIN_PB10D_SERCOM4_PAD2                     (42L)        
#define MUX_PB10D_SERCOM4_PAD2                     (3L)        
#define PINMUX_PB10D_SERCOM4_PAD2                  ((PIN_PB10D_SERCOM4_PAD2 << 16) | MUX_PB10D_SERCOM4_PAD2)
#define PORT_PB10D_SERCOM4_PAD2                    ((1UL) << 10)

#define PIN_PB28D_SERCOM4_PAD2                     (60L)        
#define MUX_PB28D_SERCOM4_PAD2                     (3L)        
#define PINMUX_PB28D_SERCOM4_PAD2                  ((PIN_PB28D_SERCOM4_PAD2 << 16) | MUX_PB28D_SERCOM4_PAD2)
#define PORT_PB28D_SERCOM4_PAD2                    ((1UL) << 28)

#define PIN_PB14C_SERCOM4_PAD2                     (46L)        
#define MUX_PB14C_SERCOM4_PAD2                     (2L)        
#define PINMUX_PB14C_SERCOM4_PAD2                  ((PIN_PB14C_SERCOM4_PAD2 << 16) | MUX_PB14C_SERCOM4_PAD2)
#define PORT_PB14C_SERCOM4_PAD2                    ((1UL) << 14)

#define PIN_PB11D_SERCOM4_PAD3                     (43L)        
#define MUX_PB11D_SERCOM4_PAD3                     (3L)        
#define PINMUX_PB11D_SERCOM4_PAD3                  ((PIN_PB11D_SERCOM4_PAD3 << 16) | MUX_PB11D_SERCOM4_PAD3)
#define PORT_PB11D_SERCOM4_PAD3                    ((1UL) << 11)

#define PIN_PB29D_SERCOM4_PAD3                     (61L)        
#define MUX_PB29D_SERCOM4_PAD3                     (3L)        
#define PINMUX_PB29D_SERCOM4_PAD3                  ((PIN_PB29D_SERCOM4_PAD3 << 16) | MUX_PB29D_SERCOM4_PAD3)
#define PORT_PB29D_SERCOM4_PAD3                    ((1UL) << 29)

#define PIN_PA15D_SERCOM4_PAD3                     (15L)        
#define MUX_PA15D_SERCOM4_PAD3                     (3L)        
#define PINMUX_PA15D_SERCOM4_PAD3                  ((PIN_PA15D_SERCOM4_PAD3 << 16) | MUX_PA15D_SERCOM4_PAD3)
#define PORT_PA15D_SERCOM4_PAD3                    ((1UL) << 15)

#define PIN_PB15C_SERCOM4_PAD3                     (47L)        
#define MUX_PB15C_SERCOM4_PAD3                     (2L)        
#define PINMUX_PB15C_SERCOM4_PAD3                  ((PIN_PB15C_SERCOM4_PAD3 << 16) | MUX_PB15C_SERCOM4_PAD3)
#define PORT_PB15C_SERCOM4_PAD3                    ((1UL) << 15)

/* ========== PORT definition for SERCOM5 peripheral ========== */
#define PIN_PA23D_SERCOM5_PAD0                     (23L)        
#define MUX_PA23D_SERCOM5_PAD0                     (3L)        
#define PINMUX_PA23D_SERCOM5_PAD0                  ((PIN_PA23D_SERCOM5_PAD0 << 16) | MUX_PA23D_SERCOM5_PAD0)
#define PORT_PA23D_SERCOM5_PAD0                    ((1UL) << 23)

#define PIN_PB02D_SERCOM5_PAD0                     (34L)        
#define MUX_PB02D_SERCOM5_PAD0                     (3L)        
#define PINMUX_PB02D_SERCOM5_PAD0                  ((PIN_PB02D_SERCOM5_PAD0 << 16) | MUX_PB02D_SERCOM5_PAD0)
#define PORT_PB02D_SERCOM5_PAD0                    ((1UL) << 2)

#define PIN_PB31D_SERCOM5_PAD0                     (63L)        
#define MUX_PB31D_SERCOM5_PAD0                     (3L)        
#define PINMUX_PB31D_SERCOM5_PAD0                  ((PIN_PB31D_SERCOM5_PAD0 << 16) | MUX_PB31D_SERCOM5_PAD0)
#define PORT_PB31D_SERCOM5_PAD0                    ((1UL) << 31)

#define PIN_PB16C_SERCOM5_PAD0                     (48L)        
#define MUX_PB16C_SERCOM5_PAD0                     (2L)        
#define PINMUX_PB16C_SERCOM5_PAD0                  ((PIN_PB16C_SERCOM5_PAD0 << 16) | MUX_PB16C_SERCOM5_PAD0)
#define PORT_PB16C_SERCOM5_PAD0                    ((1UL) << 16)

#define PIN_PA22D_SERCOM5_PAD1                     (22L)        
#define MUX_PA22D_SERCOM5_PAD1                     (3L)        
#define PINMUX_PA22D_SERCOM5_PAD1                  ((PIN_PA22D_SERCOM5_PAD1 << 16) | MUX_PA22D_SERCOM5_PAD1)
#define PORT_PA22D_SERCOM5_PAD1                    ((1UL) << 22)

#define PIN_PB03D_SERCOM5_PAD1                     (35L)        
#define MUX_PB03D_SERCOM5_PAD1                     (3L)        
#define PINMUX_PB03D_SERCOM5_PAD1                  ((PIN_PB03D_SERCOM5_PAD1 << 16) | MUX_PB03D_SERCOM5_PAD1)
#define PORT_PB03D_SERCOM5_PAD1                    ((1UL) << 3)

#define PIN_PB30D_SERCOM5_PAD1                     (62L)        
#define MUX_PB30D_SERCOM5_PAD1                     (3L)        
#define PINMUX_PB30D_SERCOM5_PAD1                  ((PIN_PB30D_SERCOM5_PAD1 << 16) | MUX_PB30D_SERCOM5_PAD1)
#define PORT_PB30D_SERCOM5_PAD1                    ((1UL) << 30)

#define PIN_PB17C_SERCOM5_PAD1                     (49L)        
#define MUX_PB17C_SERCOM5_PAD1                     (2L)        
#define PINMUX_PB17C_SERCOM5_PAD1                  ((PIN_PB17C_SERCOM5_PAD1 << 16) | MUX_PB17C_SERCOM5_PAD1)
#define PORT_PB17C_SERCOM5_PAD1                    ((1UL) << 17)

#define PIN_PA24D_SERCOM5_PAD2                     (24L)        
#define MUX_PA24D_SERCOM5_PAD2                     (3L)        
#define PINMUX_PA24D_SERCOM5_PAD2                  ((PIN_PA24D_SERCOM5_PAD2 << 16) | MUX_PA24D_SERCOM5_PAD2)
#define PORT_PA24D_SERCOM5_PAD2                    ((1UL) << 24)

#define PIN_PB00D_SERCOM5_PAD2                     (32L)        
#define MUX_PB00D_SERCOM5_PAD2                     (3L)        
#define PINMUX_PB00D_SERCOM5_PAD2                  ((PIN_PB00D_SERCOM5_PAD2 << 16) | MUX_PB00D_SERCOM5_PAD2)
#define PORT_PB00D_SERCOM5_PAD2                    ((1UL) << 0)

#define PIN_PB22D_SERCOM5_PAD2                     (54L)        
#define MUX_PB22D_SERCOM5_PAD2                     (3L)        
#define PINMUX_PB22D_SERCOM5_PAD2                  ((PIN_PB22D_SERCOM5_PAD2 << 16) | MUX_PB22D_SERCOM5_PAD2)
#define PORT_PB22D_SERCOM5_PAD2                    ((1UL) << 22)

#define PIN_PA20C_SERCOM5_PAD2                     (20L)        
#define MUX_PA20C_SERCOM5_PAD2                     (2L)        
#define PINMUX_PA20C_SERCOM5_PAD2                  ((PIN_PA20C_SERCOM5_PAD2 << 16) | MUX_PA20C_SERCOM5_PAD2)
#define PORT_PA20C_SERCOM5_PAD2                    ((1UL) << 20)

#define PIN_PB18C_SERCOM5_PAD2                     (50L)        
#define MUX_PB18C_SERCOM5_PAD2                     (2L)        
#define PINMUX_PB18C_SERCOM5_PAD2                  ((PIN_PB18C_SERCOM5_PAD2 << 16) | MUX_PB18C_SERCOM5_PAD2)
#define PORT_PB18C_SERCOM5_PAD2                    ((1UL) << 18)

#define PIN_PA25D_SERCOM5_PAD3                     (25L)        
#define MUX_PA25D_SERCOM5_PAD3                     (3L)        
#define PINMUX_PA25D_SERCOM5_PAD3                  ((PIN_PA25D_SERCOM5_PAD3 << 16) | MUX_PA25D_SERCOM5_PAD3)
#define PORT_PA25D_SERCOM5_PAD3                    ((1UL) << 25)

#define PIN_PB01D_SERCOM5_PAD3                     (33L)        
#define MUX_PB01D_SERCOM5_PAD3                     (3L)        
#define PINMUX_PB01D_SERCOM5_PAD3                  ((PIN_PB01D_SERCOM5_PAD3 << 16) | MUX_PB01D_SERCOM5_PAD3)
#define PORT_PB01D_SERCOM5_PAD3                    ((1UL) << 1)

#define PIN_PB23D_SERCOM5_PAD3                     (55L)        
#define MUX_PB23D_SERCOM5_PAD3                     (3L)        
#define PINMUX_PB23D_SERCOM5_PAD3                  ((PIN_PB23D_SERCOM5_PAD3 << 16) | MUX_PB23D_SERCOM5_PAD3)
#define PORT_PB23D_SERCOM5_PAD3                    ((1UL) << 23)

#define PIN_PA21C_SERCOM5_PAD3                     (21L)        
#define MUX_PA21C_SERCOM5_PAD3                     (2L)        
#define PINMUX_PA21C_SERCOM5_PAD3                  ((PIN_PA21C_SERCOM5_PAD3 << 16) | MUX_PA21C_SERCOM5_PAD3)
#define PORT_PA21C_SERCOM5_PAD3                    ((1UL) << 21)

#define PIN_PB19C_SERCOM5_PAD3                     (51L)        
#define MUX_PB19C_SERCOM5_PAD3                     (2L)        
#define PINMUX_PB19C_SERCOM5_PAD3                  ((PIN_PB19C_SERCOM5_PAD3 << 16) | MUX_PB19C_SERCOM5_PAD3)
#define PORT_PB19C_SERCOM5_PAD3                    ((1UL) << 19)

/* ========== PORT definition for SERCOM6 peripheral ========== */
#define PIN_PD09D_SERCOM6_PAD0                     (105L)       
#define MUX_PD09D_SERCOM6_PAD0                     (3L)        
#define PINMUX_PD09D_SERCOM6_PAD0                  ((PIN_PD09D_SERCOM6_PAD0 << 16) | MUX_PD09D_SERCOM6_PAD0)
#define PORT_PD09D_SERCOM6_PAD0                    ((1UL) << 9)

#define PIN_PC13D_SERCOM6_PAD0                     (77L)        
#define MUX_PC13D_SERCOM6_PAD0                     (3L)        
#define PINMUX_PC13D_SERCOM6_PAD0                  ((PIN_PC13D_SERCOM6_PAD0 << 16) | MUX_PC13D_SERCOM6_PAD0)
#define PORT_PC13D_SERCOM6_PAD0                    ((1UL) << 13)

#define PIN_PC04C_SERCOM6_PAD0                     (68L)        
#define MUX_PC04C_SERCOM6_PAD0                     (2L)        
#define PINMUX_PC04C_SERCOM6_PAD0                  ((PIN_PC04C_SERCOM6_PAD0 << 16) | MUX_PC04C_SERCOM6_PAD0)
#define PORT_PC04C_SERCOM6_PAD0                    ((1UL) << 4)

#define PIN_PC16C_SERCOM6_PAD0                     (80L)        
#define MUX_PC16C_SERCOM6_PAD0                     (2L)        
#define PINMUX_PC16C_SERCOM6_PAD0                  ((PIN_PC16C_SERCOM6_PAD0 << 16) | MUX_PC16C_SERCOM6_PAD0)
#define PORT_PC16C_SERCOM6_PAD0                    ((1UL) << 16)

#define PIN_PD08D_SERCOM6_PAD1                     (104L)       
#define MUX_PD08D_SERCOM6_PAD1                     (3L)        
#define PINMUX_PD08D_SERCOM6_PAD1                  ((PIN_PD08D_SERCOM6_PAD1 << 16) | MUX_PD08D_SERCOM6_PAD1)
#define PORT_PD08D_SERCOM6_PAD1                    ((1UL) << 8)

#define PIN_PC12D_SERCOM6_PAD1                     (76L)        
#define MUX_PC12D_SERCOM6_PAD1                     (3L)        
#define PINMUX_PC12D_SERCOM6_PAD1                  ((PIN_PC12D_SERCOM6_PAD1 << 16) | MUX_PC12D_SERCOM6_PAD1)
#define PORT_PC12D_SERCOM6_PAD1                    ((1UL) << 12)

#define PIN_PC05C_SERCOM6_PAD1                     (69L)        
#define MUX_PC05C_SERCOM6_PAD1                     (2L)        
#define PINMUX_PC05C_SERCOM6_PAD1                  ((PIN_PC05C_SERCOM6_PAD1 << 16) | MUX_PC05C_SERCOM6_PAD1)
#define PORT_PC05C_SERCOM6_PAD1                    ((1UL) << 5)

#define PIN_PC17C_SERCOM6_PAD1                     (81L)        
#define MUX_PC17C_SERCOM6_PAD1                     (2L)        
#define PINMUX_PC17C_SERCOM6_PAD1                  ((PIN_PC17C_SERCOM6_PAD1 << 16) | MUX_PC17C_SERCOM6_PAD1)
#define PORT_PC17C_SERCOM6_PAD1                    ((1UL) << 17)

#define PIN_PC14D_SERCOM6_PAD2                     (78L)        
#define MUX_PC14D_SERCOM6_PAD2                     (3L)        
#define PINMUX_PC14D_SERCOM6_PAD2                  ((PIN_PC14D_SERCOM6_PAD2 << 16) | MUX_PC14D_SERCOM6_PAD2)
#define PORT_PC14D_SERCOM6_PAD2                    ((1UL) << 14)

#define PIN_PD10D_SERCOM6_PAD2                     (106L)       
#define MUX_PD10D_SERCOM6_PAD2                     (3L)        
#define PINMUX_PD10D_SERCOM6_PAD2                  ((PIN_PD10D_SERCOM6_PAD2 << 16) | MUX_PD10D_SERCOM6_PAD2)
#define PORT_PD10D_SERCOM6_PAD2                    ((1UL) << 10)

#define PIN_PC06C_SERCOM6_PAD2                     (70L)        
#define MUX_PC06C_SERCOM6_PAD2                     (2L)        
#define PINMUX_PC06C_SERCOM6_PAD2                  ((PIN_PC06C_SERCOM6_PAD2 << 16) | MUX_PC06C_SERCOM6_PAD2)
#define PORT_PC06C_SERCOM6_PAD2                    ((1UL) << 6)

#define PIN_PC10C_SERCOM6_PAD2                     (74L)        
#define MUX_PC10C_SERCOM6_PAD2                     (2L)        
#define PINMUX_PC10C_SERCOM6_PAD2                  ((PIN_PC10C_SERCOM6_PAD2 << 16) | MUX_PC10C_SERCOM6_PAD2)
#define PORT_PC10C_SERCOM6_PAD2                    ((1UL) << 10)

#define PIN_PC18C_SERCOM6_PAD2                     (82L)        
#define MUX_PC18C_SERCOM6_PAD2                     (2L)        
#define PINMUX_PC18C_SERCOM6_PAD2                  ((PIN_PC18C_SERCOM6_PAD2 << 16) | MUX_PC18C_SERCOM6_PAD2)
#define PORT_PC18C_SERCOM6_PAD2                    ((1UL) << 18)

#define PIN_PC15D_SERCOM6_PAD3                     (79L)        
#define MUX_PC15D_SERCOM6_PAD3                     (3L)        
#define PINMUX_PC15D_SERCOM6_PAD3                  ((PIN_PC15D_SERCOM6_PAD3 << 16) | MUX_PC15D_SERCOM6_PAD3)
#define PORT_PC15D_SERCOM6_PAD3                    ((1UL) << 15)

#define PIN_PD11D_SERCOM6_PAD3                     (107L)       
#define MUX_PD11D_SERCOM6_PAD3                     (3L)        
#define PINMUX_PD11D_SERCOM6_PAD3                  ((PIN_PD11D_SERCOM6_PAD3 << 16) | MUX_PD11D_SERCOM6_PAD3)
#define PORT_PD11D_SERCOM6_PAD3                    ((1UL) << 11)

#define PIN_PC07C_SERCOM6_PAD3                     (71L)        
#define MUX_PC07C_SERCOM6_PAD3                     (2L)        
#define PINMUX_PC07C_SERCOM6_PAD3                  ((PIN_PC07C_SERCOM6_PAD3 << 16) | MUX_PC07C_SERCOM6_PAD3)
#define PORT_PC07C_SERCOM6_PAD3                    ((1UL) << 7)

#define PIN_PC11C_SERCOM6_PAD3                     (75L)        
#define MUX_PC11C_SERCOM6_PAD3                     (2L)        
#define PINMUX_PC11C_SERCOM6_PAD3                  ((PIN_PC11C_SERCOM6_PAD3 << 16) | MUX_PC11C_SERCOM6_PAD3)
#define PORT_PC11C_SERCOM6_PAD3                    ((1UL) << 11)

#define PIN_PC19C_SERCOM6_PAD3                     (83L)        
#define MUX_PC19C_SERCOM6_PAD3                     (2L)        
#define PINMUX_PC19C_SERCOM6_PAD3                  ((PIN_PC19C_SERCOM6_PAD3 << 16) | MUX_PC19C_SERCOM6_PAD3)
#define PORT_PC19C_SERCOM6_PAD3                    ((1UL) << 19)

/* ========== PORT definition for SERCOM7 peripheral ========== */
#define PIN_PB21D_SERCOM7_PAD0                     (53L)        
#define MUX_PB21D_SERCOM7_PAD0                     (3L)        
#define PINMUX_PB21D_SERCOM7_PAD0                  ((PIN_PB21D_SERCOM7_PAD0 << 16) | MUX_PB21D_SERCOM7_PAD0)
#define PORT_PB21D_SERCOM7_PAD0                    ((1UL) << 21)

#define PIN_PD08C_SERCOM7_PAD0                     (104L)       
#define MUX_PD08C_SERCOM7_PAD0                     (2L)        
#define PINMUX_PD08C_SERCOM7_PAD0                  ((PIN_PD08C_SERCOM7_PAD0 << 16) | MUX_PD08C_SERCOM7_PAD0)
#define PORT_PD08C_SERCOM7_PAD0                    ((1UL) << 8)

#define PIN_PB30C_SERCOM7_PAD0                     (62L)        
#define MUX_PB30C_SERCOM7_PAD0                     (2L)        
#define PINMUX_PB30C_SERCOM7_PAD0                  ((PIN_PB30C_SERCOM7_PAD0 << 16) | MUX_PB30C_SERCOM7_PAD0)
#define PORT_PB30C_SERCOM7_PAD0                    ((1UL) << 30)

#define PIN_PC12C_SERCOM7_PAD0                     (76L)        
#define MUX_PC12C_SERCOM7_PAD0                     (2L)        
#define PINMUX_PC12C_SERCOM7_PAD0                  ((PIN_PC12C_SERCOM7_PAD0 << 16) | MUX_PC12C_SERCOM7_PAD0)
#define PORT_PC12C_SERCOM7_PAD0                    ((1UL) << 12)

#define PIN_PB20D_SERCOM7_PAD1                     (52L)        
#define MUX_PB20D_SERCOM7_PAD1                     (3L)        
#define PINMUX_PB20D_SERCOM7_PAD1                  ((PIN_PB20D_SERCOM7_PAD1 << 16) | MUX_PB20D_SERCOM7_PAD1)
#define PORT_PB20D_SERCOM7_PAD1                    ((1UL) << 20)

#define PIN_PD09C_SERCOM7_PAD1                     (105L)       
#define MUX_PD09C_SERCOM7_PAD1                     (2L)        
#define PINMUX_PD09C_SERCOM7_PAD1                  ((PIN_PD09C_SERCOM7_PAD1 << 16) | MUX_PD09C_SERCOM7_PAD1)
#define PORT_PD09C_SERCOM7_PAD1                    ((1UL) << 9)

#define PIN_PB31C_SERCOM7_PAD1                     (63L)        
#define MUX_PB31C_SERCOM7_PAD1                     (2L)        
#define PINMUX_PB31C_SERCOM7_PAD1                  ((PIN_PB31C_SERCOM7_PAD1 << 16) | MUX_PB31C_SERCOM7_PAD1)
#define PORT_PB31C_SERCOM7_PAD1                    ((1UL) << 31)

#define PIN_PC13C_SERCOM7_PAD1                     (77L)        
#define MUX_PC13C_SERCOM7_PAD1                     (2L)        
#define PINMUX_PC13C_SERCOM7_PAD1                  ((PIN_PC13C_SERCOM7_PAD1 << 16) | MUX_PC13C_SERCOM7_PAD1)
#define PORT_PC13C_SERCOM7_PAD1                    ((1UL) << 13)

#define PIN_PB18D_SERCOM7_PAD2                     (50L)        
#define MUX_PB18D_SERCOM7_PAD2                     (3L)        
#define PINMUX_PB18D_SERCOM7_PAD2                  ((PIN_PB18D_SERCOM7_PAD2 << 16) | MUX_PB18D_SERCOM7_PAD2)
#define PORT_PB18D_SERCOM7_PAD2                    ((1UL) << 18)

#define PIN_PC10D_SERCOM7_PAD2                     (74L)        
#define MUX_PC10D_SERCOM7_PAD2                     (3L)        
#define PINMUX_PC10D_SERCOM7_PAD2                  ((PIN_PC10D_SERCOM7_PAD2 << 16) | MUX_PC10D_SERCOM7_PAD2)
#define PORT_PC10D_SERCOM7_PAD2                    ((1UL) << 10)

#define PIN_PC14C_SERCOM7_PAD2                     (78L)        
#define MUX_PC14C_SERCOM7_PAD2                     (2L)        
#define PINMUX_PC14C_SERCOM7_PAD2                  ((PIN_PC14C_SERCOM7_PAD2 << 16) | MUX_PC14C_SERCOM7_PAD2)
#define PORT_PC14C_SERCOM7_PAD2                    ((1UL) << 14)

#define PIN_PD10C_SERCOM7_PAD2                     (106L)       
#define MUX_PD10C_SERCOM7_PAD2                     (2L)        
#define PINMUX_PD10C_SERCOM7_PAD2                  ((PIN_PD10C_SERCOM7_PAD2 << 16) | MUX_PD10C_SERCOM7_PAD2)
#define PORT_PD10C_SERCOM7_PAD2                    ((1UL) << 10)

#define PIN_PA30C_SERCOM7_PAD2                     (30L)        
#define MUX_PA30C_SERCOM7_PAD2                     (2L)        
#define PINMUX_PA30C_SERCOM7_PAD2                  ((PIN_PA30C_SERCOM7_PAD2 << 16) | MUX_PA30C_SERCOM7_PAD2)
#define PORT_PA30C_SERCOM7_PAD2                    ((1UL) << 30)

#define PIN_PB19D_SERCOM7_PAD3                     (51L)        
#define MUX_PB19D_SERCOM7_PAD3                     (3L)        
#define PINMUX_PB19D_SERCOM7_PAD3                  ((PIN_PB19D_SERCOM7_PAD3 << 16) | MUX_PB19D_SERCOM7_PAD3)
#define PORT_PB19D_SERCOM7_PAD3                    ((1UL) << 19)

#define PIN_PC11D_SERCOM7_PAD3                     (75L)        
#define MUX_PC11D_SERCOM7_PAD3                     (3L)        
#define PINMUX_PC11D_SERCOM7_PAD3                  ((PIN_PC11D_SERCOM7_PAD3 << 16) | MUX_PC11D_SERCOM7_PAD3)
#define PORT_PC11D_SERCOM7_PAD3                    ((1UL) << 11)

#define PIN_PC15C_SERCOM7_PAD3                     (79L)        
#define MUX_PC15C_SERCOM7_PAD3                     (2L)        
#define PINMUX_PC15C_SERCOM7_PAD3                  ((PIN_PC15C_SERCOM7_PAD3 << 16) | MUX_PC15C_SERCOM7_PAD3)
#define PORT_PC15C_SERCOM7_PAD3                    ((1UL) << 15)

#define PIN_PD11C_SERCOM7_PAD3                     (107L)       
#define MUX_PD11C_SERCOM7_PAD3                     (2L)        
#define PINMUX_PD11C_SERCOM7_PAD3                  ((PIN_PD11C_SERCOM7_PAD3 << 16) | MUX_PD11C_SERCOM7_PAD3)
#define PORT_PD11C_SERCOM7_PAD3                    ((1UL) << 11)

#define PIN_PA31C_SERCOM7_PAD3                     (31L)        
#define MUX_PA31C_SERCOM7_PAD3                     (2L)        
#define PINMUX_PA31C_SERCOM7_PAD3                  ((PIN_PA31C_SERCOM7_PAD3 << 16) | MUX_PA31C_SERCOM7_PAD3)
#define PORT_PA31C_SERCOM7_PAD3                    ((1UL) << 31)

/* ========== PORT definition for TC0 peripheral ========== */
#define PIN_PA04E_TC0_WO0                          (4L)         
#define MUX_PA04E_TC0_WO0                          (4L)        
#define PINMUX_PA04E_TC0_WO0                       ((PIN_PA04E_TC0_WO0 << 16) | MUX_PA04E_TC0_WO0)
#define PORT_PA04E_TC0_WO0                         ((1UL) << 4)

#define PIN_PA08E_TC0_WO0                          (8L)         
#define MUX_PA08E_TC0_WO0                          (4L)        
#define PINMUX_PA08E_TC0_WO0                       ((PIN_PA08E_TC0_WO0 << 16) | MUX_PA08E_TC0_WO0)
#define PORT_PA08E_TC0_WO0                         ((1UL) << 8)

#define PIN_PB30E_TC0_WO0                          (62L)        
#define MUX_PB30E_TC0_WO0                          (4L)        
#define PINMUX_PB30E_TC0_WO0                       ((PIN_PB30E_TC0_WO0 << 16) | MUX_PB30E_TC0_WO0)
#define PORT_PB30E_TC0_WO0                         ((1UL) << 30)

#define PIN_PA05E_TC0_WO1                          (5L)         
#define MUX_PA05E_TC0_WO1                          (4L)        
#define PINMUX_PA05E_TC0_WO1                       ((PIN_PA05E_TC0_WO1 << 16) | MUX_PA05E_TC0_WO1)
#define PORT_PA05E_TC0_WO1                         ((1UL) << 5)

#define PIN_PA09E_TC0_WO1                          (9L)         
#define MUX_PA09E_TC0_WO1                          (4L)        
#define PINMUX_PA09E_TC0_WO1                       ((PIN_PA09E_TC0_WO1 << 16) | MUX_PA09E_TC0_WO1)
#define PORT_PA09E_TC0_WO1                         ((1UL) << 9)

#define PIN_PB31E_TC0_WO1                          (63L)        
#define MUX_PB31E_TC0_WO1                          (4L)        
#define PINMUX_PB31E_TC0_WO1                       ((PIN_PB31E_TC0_WO1 << 16) | MUX_PB31E_TC0_WO1)
#define PORT_PB31E_TC0_WO1                         ((1UL) << 31)

/* ========== PORT definition for TC1 peripheral ========== */
#define PIN_PA06E_TC1_WO0                          (6L)         
#define MUX_PA06E_TC1_WO0                          (4L)        
#define PINMUX_PA06E_TC1_WO0                       ((PIN_PA06E_TC1_WO0 << 16) | MUX_PA06E_TC1_WO0)
#define PORT_PA06E_TC1_WO0                         ((1UL) << 6)

#define PIN_PA10E_TC1_WO0                          (10L)        
#define MUX_PA10E_TC1_WO0                          (4L)        
#define PINMUX_PA10E_TC1_WO0                       ((PIN_PA10E_TC1_WO0 << 16) | MUX_PA10E_TC1_WO0)
#define PORT_PA10E_TC1_WO0                         ((1UL) << 10)

#define PIN_PA07E_TC1_WO1                          (7L)         
#define MUX_PA07E_TC1_WO1                          (4L)        
#define PINMUX_PA07E_TC1_WO1                       ((PIN_PA07E_TC1_WO1 << 16) | MUX_PA07E_TC1_WO1)
#define PORT_PA07E_TC1_WO1                         ((1UL) << 7)

#define PIN_PA11E_TC1_WO1                          (11L)        
#define MUX_PA11E_TC1_WO1                          (4L)        
#define PINMUX_PA11E_TC1_WO1                       ((PIN_PA11E_TC1_WO1 << 16) | MUX_PA11E_TC1_WO1)
#define PORT_PA11E_TC1_WO1                         ((1UL) << 11)

/* ========== PORT definition for TC2 peripheral ========== */
#define PIN_PA12E_TC2_WO0                          (12L)        
#define MUX_PA12E_TC2_WO0                          (4L)        
#define PINMUX_PA12E_TC2_WO0                       ((PIN_PA12E_TC2_WO0 << 16) | MUX_PA12E_TC2_WO0)
#define PORT_PA12E_TC2_WO0                         ((1UL) << 12)

#define PIN_PA16E_TC2_WO0                          (16L)        
#define MUX_PA16E_TC2_WO0                          (4L)        
#define PINMUX_PA16E_TC2_WO0                       ((PIN_PA16E_TC2_WO0 << 16) | MUX_PA16E_TC2_WO0)
#define PORT_PA16E_TC2_WO0                         ((1UL) << 16)

#define PIN_PA00E_TC2_WO0                          (0L)         
#define MUX_PA00E_TC2_WO0                          (4L)        
#define PINMUX_PA00E_TC2_WO0                       ((PIN_PA00E_TC2_WO0 << 16) | MUX_PA00E_TC2_WO0)
#define PORT_PA00E_TC2_WO0                         ((1UL) << 0)

#define PIN_PA01E_TC2_WO1                          (1L)         
#define MUX_PA01E_TC2_WO1                          (4L)        
#define PINMUX_PA01E_TC2_WO1                       ((PIN_PA01E_TC2_WO1 << 16) | MUX_PA01E_TC2_WO1)
#define PORT_PA01E_TC2_WO1                         ((1UL) << 1)

#define PIN_PA13E_TC2_WO1                          (13L)        
#define MUX_PA13E_TC2_WO1                          (4L)        
#define PINMUX_PA13E_TC2_WO1                       ((PIN_PA13E_TC2_WO1 << 16) | MUX_PA13E_TC2_WO1)
#define PORT_PA13E_TC2_WO1                         ((1UL) << 13)

#define PIN_PA17E_TC2_WO1                          (17L)        
#define MUX_PA17E_TC2_WO1                          (4L)        
#define PINMUX_PA17E_TC2_WO1                       ((PIN_PA17E_TC2_WO1 << 16) | MUX_PA17E_TC2_WO1)
#define PORT_PA17E_TC2_WO1                         ((1UL) << 17)

/* ========== PORT definition for TC3 peripheral ========== */
#define PIN_PA18E_TC3_WO0                          (18L)        
#define MUX_PA18E_TC3_WO0                          (4L)        
#define PINMUX_PA18E_TC3_WO0                       ((PIN_PA18E_TC3_WO0 << 16) | MUX_PA18E_TC3_WO0)
#define PORT_PA18E_TC3_WO0                         ((1UL) << 18)

#define PIN_PA14E_TC3_WO0                          (14L)        
#define MUX_PA14E_TC3_WO0                          (4L)        
#define PINMUX_PA14E_TC3_WO0                       ((PIN_PA14E_TC3_WO0 << 16) | MUX_PA14E_TC3_WO0)
#define PORT_PA14E_TC3_WO0                         ((1UL) << 14)

#define PIN_PA15E_TC3_WO1                          (15L)        
#define MUX_PA15E_TC3_WO1                          (4L)        
#define PINMUX_PA15E_TC3_WO1                       ((PIN_PA15E_TC3_WO1 << 16) | MUX_PA15E_TC3_WO1)
#define PORT_PA15E_TC3_WO1                         ((1UL) << 15)

#define PIN_PA19E_TC3_WO1                          (19L)        
#define MUX_PA19E_TC3_WO1                          (4L)        
#define PINMUX_PA19E_TC3_WO1                       ((PIN_PA19E_TC3_WO1 << 16) | MUX_PA19E_TC3_WO1)
#define PORT_PA19E_TC3_WO1                         ((1UL) << 19)

/* ========== PORT definition for TC4 peripheral ========== */
#define PIN_PA22E_TC4_WO0                          (22L)        
#define MUX_PA22E_TC4_WO0                          (4L)        
#define PINMUX_PA22E_TC4_WO0                       ((PIN_PA22E_TC4_WO0 << 16) | MUX_PA22E_TC4_WO0)
#define PORT_PA22E_TC4_WO0                         ((1UL) << 22)

#define PIN_PB08E_TC4_WO0                          (40L)        
#define MUX_PB08E_TC4_WO0                          (4L)        
#define PINMUX_PB08E_TC4_WO0                       ((PIN_PB08E_TC4_WO0 << 16) | MUX_PB08E_TC4_WO0)
#define PORT_PB08E_TC4_WO0                         ((1UL) << 8)

#define PIN_PB12E_TC4_WO0                          (44L)        
#define MUX_PB12E_TC4_WO0                          (4L)        
#define PINMUX_PB12E_TC4_WO0                       ((PIN_PB12E_TC4_WO0 << 16) | MUX_PB12E_TC4_WO0)
#define PORT_PB12E_TC4_WO0                         ((1UL) << 12)

#define PIN_PA23E_TC4_WO1                          (23L)        
#define MUX_PA23E_TC4_WO1                          (4L)        
#define PINMUX_PA23E_TC4_WO1                       ((PIN_PA23E_TC4_WO1 << 16) | MUX_PA23E_TC4_WO1)
#define PORT_PA23E_TC4_WO1                         ((1UL) << 23)

#define PIN_PB09E_TC4_WO1                          (41L)        
#define MUX_PB09E_TC4_WO1                          (4L)        
#define PINMUX_PB09E_TC4_WO1                       ((PIN_PB09E_TC4_WO1 << 16) | MUX_PB09E_TC4_WO1)
#define PORT_PB09E_TC4_WO1                         ((1UL) << 9)

#define PIN_PB13E_TC4_WO1                          (45L)        
#define MUX_PB13E_TC4_WO1                          (4L)        
#define PINMUX_PB13E_TC4_WO1                       ((PIN_PB13E_TC4_WO1 << 16) | MUX_PB13E_TC4_WO1)
#define PORT_PB13E_TC4_WO1                         ((1UL) << 13)

/* ========== PORT definition for TC5 peripheral ========== */
#define PIN_PA24E_TC5_WO0                          (24L)        
#define MUX_PA24E_TC5_WO0                          (4L)        
#define PINMUX_PA24E_TC5_WO0                       ((PIN_PA24E_TC5_WO0 << 16) | MUX_PA24E_TC5_WO0)
#define PORT_PA24E_TC5_WO0                         ((1UL) << 24)

#define PIN_PB10E_TC5_WO0                          (42L)        
#define MUX_PB10E_TC5_WO0                          (4L)        
#define PINMUX_PB10E_TC5_WO0                       ((PIN_PB10E_TC5_WO0 << 16) | MUX_PB10E_TC5_WO0)
#define PORT_PB10E_TC5_WO0                         ((1UL) << 10)

#define PIN_PB14E_TC5_WO0                          (46L)        
#define MUX_PB14E_TC5_WO0                          (4L)        
#define PINMUX_PB14E_TC5_WO0                       ((PIN_PB14E_TC5_WO0 << 16) | MUX_PB14E_TC5_WO0)
#define PORT_PB14E_TC5_WO0                         ((1UL) << 14)

#define PIN_PA25E_TC5_WO1                          (25L)        
#define MUX_PA25E_TC5_WO1                          (4L)        
#define PINMUX_PA25E_TC5_WO1                       ((PIN_PA25E_TC5_WO1 << 16) | MUX_PA25E_TC5_WO1)
#define PORT_PA25E_TC5_WO1                         ((1UL) << 25)

#define PIN_PB11E_TC5_WO1                          (43L)        
#define MUX_PB11E_TC5_WO1                          (4L)        
#define PINMUX_PB11E_TC5_WO1                       ((PIN_PB11E_TC5_WO1 << 16) | MUX_PB11E_TC5_WO1)
#define PORT_PB11E_TC5_WO1                         ((1UL) << 11)

#define PIN_PB15E_TC5_WO1                          (47L)        
#define MUX_PB15E_TC5_WO1                          (4L)        
#define PINMUX_PB15E_TC5_WO1                       ((PIN_PB15E_TC5_WO1 << 16) | MUX_PB15E_TC5_WO1)
#define PORT_PB15E_TC5_WO1                         ((1UL) << 15)

/* ========== PORT definition for TC6 peripheral ========== */
#define PIN_PA30E_TC6_WO0                          (30L)        
#define MUX_PA30E_TC6_WO0                          (4L)        
#define PINMUX_PA30E_TC6_WO0                       ((PIN_PA30E_TC6_WO0 << 16) | MUX_PA30E_TC6_WO0)
#define PORT_PA30E_TC6_WO0                         ((1UL) << 30)

#define PIN_PB02E_TC6_WO0                          (34L)        
#define MUX_PB02E_TC6_WO0                          (4L)        
#define PINMUX_PB02E_TC6_WO0                       ((PIN_PB02E_TC6_WO0 << 16) | MUX_PB02E_TC6_WO0)
#define PORT_PB02E_TC6_WO0                         ((1UL) << 2)

#define PIN_PB16E_TC6_WO0                          (48L)        
#define MUX_PB16E_TC6_WO0                          (4L)        
#define PINMUX_PB16E_TC6_WO0                       ((PIN_PB16E_TC6_WO0 << 16) | MUX_PB16E_TC6_WO0)
#define PORT_PB16E_TC6_WO0                         ((1UL) << 16)

#define PIN_PA31E_TC6_WO1                          (31L)        
#define MUX_PA31E_TC6_WO1                          (4L)        
#define PINMUX_PA31E_TC6_WO1                       ((PIN_PA31E_TC6_WO1 << 16) | MUX_PA31E_TC6_WO1)
#define PORT_PA31E_TC6_WO1                         ((1UL) << 31)

#define PIN_PB03E_TC6_WO1                          (35L)        
#define MUX_PB03E_TC6_WO1                          (4L)        
#define PINMUX_PB03E_TC6_WO1                       ((PIN_PB03E_TC6_WO1 << 16) | MUX_PB03E_TC6_WO1)
#define PORT_PB03E_TC6_WO1                         ((1UL) << 3)

#define PIN_PB17E_TC6_WO1                          (49L)        
#define MUX_PB17E_TC6_WO1                          (4L)        
#define PINMUX_PB17E_TC6_WO1                       ((PIN_PB17E_TC6_WO1 << 16) | MUX_PB17E_TC6_WO1)
#define PORT_PB17E_TC6_WO1                         ((1UL) << 17)

/* ========== PORT definition for TC7 peripheral ========== */
#define PIN_PA20E_TC7_WO0                          (20L)        
#define MUX_PA20E_TC7_WO0                          (4L)        
#define PINMUX_PA20E_TC7_WO0                       ((PIN_PA20E_TC7_WO0 << 16) | MUX_PA20E_TC7_WO0)
#define PORT_PA20E_TC7_WO0                         ((1UL) << 20)

#define PIN_PB00E_TC7_WO0                          (32L)        
#define MUX_PB00E_TC7_WO0                          (4L)        
#define PINMUX_PB00E_TC7_WO0                       ((PIN_PB00E_TC7_WO0 << 16) | MUX_PB00E_TC7_WO0)
#define PORT_PB00E_TC7_WO0                         ((1UL) << 0)

#define PIN_PB22E_TC7_WO0                          (54L)        
#define MUX_PB22E_TC7_WO0                          (4L)        
#define PINMUX_PB22E_TC7_WO0                       ((PIN_PB22E_TC7_WO0 << 16) | MUX_PB22E_TC7_WO0)
#define PORT_PB22E_TC7_WO0                         ((1UL) << 22)

#define PIN_PA21E_TC7_WO1                          (21L)        
#define MUX_PA21E_TC7_WO1                          (4L)        
#define PINMUX_PA21E_TC7_WO1                       ((PIN_PA21E_TC7_WO1 << 16) | MUX_PA21E_TC7_WO1)
#define PORT_PA21E_TC7_WO1                         ((1UL) << 21)

#define PIN_PB01E_TC7_WO1                          (33L)        
#define MUX_PB01E_TC7_WO1                          (4L)        
#define PINMUX_PB01E_TC7_WO1                       ((PIN_PB01E_TC7_WO1 << 16) | MUX_PB01E_TC7_WO1)
#define PORT_PB01E_TC7_WO1                         ((1UL) << 1)

#define PIN_PB23E_TC7_WO1                          (55L)        
#define MUX_PB23E_TC7_WO1                          (4L)        
#define PINMUX_PB23E_TC7_WO1                       ((PIN_PB23E_TC7_WO1 << 16) | MUX_PB23E_TC7_WO1)
#define PORT_PB23E_TC7_WO1                         ((1UL) << 23)

/* ========== PORT definition for TCC0 peripheral ========== */
#define PIN_PA20G_TCC0_WO0                         (20L)        
#define MUX_PA20G_TCC0_WO0                         (6L)        
#define PINMUX_PA20G_TCC0_WO0                      ((PIN_PA20G_TCC0_WO0 << 16) | MUX_PA20G_TCC0_WO0)
#define PORT_PA20G_TCC0_WO0                        ((1UL) << 20)

#define PIN_PB12G_TCC0_WO0                         (44L)        
#define MUX_PB12G_TCC0_WO0                         (6L)        
#define PINMUX_PB12G_TCC0_WO0                      ((PIN_PB12G_TCC0_WO0 << 16) | MUX_PB12G_TCC0_WO0)
#define PORT_PB12G_TCC0_WO0                        ((1UL) << 12)

#define PIN_PA08F_TCC0_WO0                         (8L)         
#define MUX_PA08F_TCC0_WO0                         (5L)        
#define PINMUX_PA08F_TCC0_WO0                      ((PIN_PA08F_TCC0_WO0 << 16) | MUX_PA08F_TCC0_WO0)
#define PORT_PA08F_TCC0_WO0                        ((1UL) << 8)

#define PIN_PC04F_TCC0_WO0                         (68L)        
#define MUX_PC04F_TCC0_WO0                         (5L)        
#define PINMUX_PC04F_TCC0_WO0                      ((PIN_PC04F_TCC0_WO0 << 16) | MUX_PC04F_TCC0_WO0)
#define PORT_PC04F_TCC0_WO0                        ((1UL) << 4)

#define PIN_PC10F_TCC0_WO0                         (74L)        
#define MUX_PC10F_TCC0_WO0                         (5L)        
#define PINMUX_PC10F_TCC0_WO0                      ((PIN_PC10F_TCC0_WO0 << 16) | MUX_PC10F_TCC0_WO0)
#define PORT_PC10F_TCC0_WO0                        ((1UL) << 10)

#define PIN_PC16F_TCC0_WO0                         (80L)        
#define MUX_PC16F_TCC0_WO0                         (5L)        
#define PINMUX_PC16F_TCC0_WO0                      ((PIN_PC16F_TCC0_WO0 << 16) | MUX_PC16F_TCC0_WO0)
#define PORT_PC16F_TCC0_WO0                        ((1UL) << 16)

#define PIN_PA21G_TCC0_WO1                         (21L)        
#define MUX_PA21G_TCC0_WO1                         (6L)        
#define PINMUX_PA21G_TCC0_WO1                      ((PIN_PA21G_TCC0_WO1 << 16) | MUX_PA21G_TCC0_WO1)
#define PORT_PA21G_TCC0_WO1                        ((1UL) << 21)

#define PIN_PB13G_TCC0_WO1                         (45L)        
#define MUX_PB13G_TCC0_WO1                         (6L)        
#define PINMUX_PB13G_TCC0_WO1                      ((PIN_PB13G_TCC0_WO1 << 16) | MUX_PB13G_TCC0_WO1)
#define PORT_PB13G_TCC0_WO1                        ((1UL) << 13)

#define PIN_PA09F_TCC0_WO1                         (9L)         
#define MUX_PA09F_TCC0_WO1                         (5L)        
#define PINMUX_PA09F_TCC0_WO1                      ((PIN_PA09F_TCC0_WO1 << 16) | MUX_PA09F_TCC0_WO1)
#define PORT_PA09F_TCC0_WO1                        ((1UL) << 9)

#define PIN_PC11F_TCC0_WO1                         (75L)        
#define MUX_PC11F_TCC0_WO1                         (5L)        
#define PINMUX_PC11F_TCC0_WO1                      ((PIN_PC11F_TCC0_WO1 << 16) | MUX_PC11F_TCC0_WO1)
#define PORT_PC11F_TCC0_WO1                        ((1UL) << 11)

#define PIN_PC17F_TCC0_WO1                         (81L)        
#define MUX_PC17F_TCC0_WO1                         (5L)        
#define PINMUX_PC17F_TCC0_WO1                      ((PIN_PC17F_TCC0_WO1 << 16) | MUX_PC17F_TCC0_WO1)
#define PORT_PC17F_TCC0_WO1                        ((1UL) << 17)

#define PIN_PD08F_TCC0_WO1                         (104L)       
#define MUX_PD08F_TCC0_WO1                         (5L)        
#define PINMUX_PD08F_TCC0_WO1                      ((PIN_PD08F_TCC0_WO1 << 16) | MUX_PD08F_TCC0_WO1)
#define PORT_PD08F_TCC0_WO1                        ((1UL) << 8)

#define PIN_PA22G_TCC0_WO2                         (22L)        
#define MUX_PA22G_TCC0_WO2                         (6L)        
#define PINMUX_PA22G_TCC0_WO2                      ((PIN_PA22G_TCC0_WO2 << 16) | MUX_PA22G_TCC0_WO2)
#define PORT_PA22G_TCC0_WO2                        ((1UL) << 22)

#define PIN_PB14G_TCC0_WO2                         (46L)        
#define MUX_PB14G_TCC0_WO2                         (6L)        
#define PINMUX_PB14G_TCC0_WO2                      ((PIN_PB14G_TCC0_WO2 << 16) | MUX_PB14G_TCC0_WO2)
#define PORT_PB14G_TCC0_WO2                        ((1UL) << 14)

#define PIN_PA10F_TCC0_WO2                         (10L)        
#define MUX_PA10F_TCC0_WO2                         (5L)        
#define PINMUX_PA10F_TCC0_WO2                      ((PIN_PA10F_TCC0_WO2 << 16) | MUX_PA10F_TCC0_WO2)
#define PORT_PA10F_TCC0_WO2                        ((1UL) << 10)

#define PIN_PC12F_TCC0_WO2                         (76L)        
#define MUX_PC12F_TCC0_WO2                         (5L)        
#define PINMUX_PC12F_TCC0_WO2                      ((PIN_PC12F_TCC0_WO2 << 16) | MUX_PC12F_TCC0_WO2)
#define PORT_PC12F_TCC0_WO2                        ((1UL) << 12)

#define PIN_PC18F_TCC0_WO2                         (82L)        
#define MUX_PC18F_TCC0_WO2                         (5L)        
#define PINMUX_PC18F_TCC0_WO2                      ((PIN_PC18F_TCC0_WO2 << 16) | MUX_PC18F_TCC0_WO2)
#define PORT_PC18F_TCC0_WO2                        ((1UL) << 18)

#define PIN_PD09F_TCC0_WO2                         (105L)       
#define MUX_PD09F_TCC0_WO2                         (5L)        
#define PINMUX_PD09F_TCC0_WO2                      ((PIN_PD09F_TCC0_WO2 << 16) | MUX_PD09F_TCC0_WO2)
#define PORT_PD09F_TCC0_WO2                        ((1UL) << 9)

#define PIN_PA23G_TCC0_WO3                         (23L)        
#define MUX_PA23G_TCC0_WO3                         (6L)        
#define PINMUX_PA23G_TCC0_WO3                      ((PIN_PA23G_TCC0_WO3 << 16) | MUX_PA23G_TCC0_WO3)
#define PORT_PA23G_TCC0_WO3                        ((1UL) << 23)

#define PIN_PB15G_TCC0_WO3                         (47L)        
#define MUX_PB15G_TCC0_WO3                         (6L)        
#define PINMUX_PB15G_TCC0_WO3                      ((PIN_PB15G_TCC0_WO3 << 16) | MUX_PB15G_TCC0_WO3)
#define PORT_PB15G_TCC0_WO3                        ((1UL) << 15)

#define PIN_PA11F_TCC0_WO3                         (11L)        
#define MUX_PA11F_TCC0_WO3                         (5L)        
#define PINMUX_PA11F_TCC0_WO3                      ((PIN_PA11F_TCC0_WO3 << 16) | MUX_PA11F_TCC0_WO3)
#define PORT_PA11F_TCC0_WO3                        ((1UL) << 11)

#define PIN_PC13F_TCC0_WO3                         (77L)        
#define MUX_PC13F_TCC0_WO3                         (5L)        
#define PINMUX_PC13F_TCC0_WO3                      ((PIN_PC13F_TCC0_WO3 << 16) | MUX_PC13F_TCC0_WO3)
#define PORT_PC13F_TCC0_WO3                        ((1UL) << 13)

#define PIN_PC19F_TCC0_WO3                         (83L)        
#define MUX_PC19F_TCC0_WO3                         (5L)        
#define PINMUX_PC19F_TCC0_WO3                      ((PIN_PC19F_TCC0_WO3 << 16) | MUX_PC19F_TCC0_WO3)
#define PORT_PC19F_TCC0_WO3                        ((1UL) << 19)

#define PIN_PD10F_TCC0_WO3                         (106L)       
#define MUX_PD10F_TCC0_WO3                         (5L)        
#define PINMUX_PD10F_TCC0_WO3                      ((PIN_PD10F_TCC0_WO3 << 16) | MUX_PD10F_TCC0_WO3)
#define PORT_PD10F_TCC0_WO3                        ((1UL) << 10)

#define PIN_PA16G_TCC0_WO4                         (16L)        
#define MUX_PA16G_TCC0_WO4                         (6L)        
#define PINMUX_PA16G_TCC0_WO4                      ((PIN_PA16G_TCC0_WO4 << 16) | MUX_PA16G_TCC0_WO4)
#define PORT_PA16G_TCC0_WO4                        ((1UL) << 16)

#define PIN_PB16G_TCC0_WO4                         (48L)        
#define MUX_PB16G_TCC0_WO4                         (6L)        
#define PINMUX_PB16G_TCC0_WO4                      ((PIN_PB16G_TCC0_WO4 << 16) | MUX_PB16G_TCC0_WO4)
#define PORT_PB16G_TCC0_WO4                        ((1UL) << 16)

#define PIN_PB10F_TCC0_WO4                         (42L)        
#define MUX_PB10F_TCC0_WO4                         (5L)        
#define PINMUX_PB10F_TCC0_WO4                      ((PIN_PB10F_TCC0_WO4 << 16) | MUX_PB10F_TCC0_WO4)
#define PORT_PB10F_TCC0_WO4                        ((1UL) << 10)

#define PIN_PC14F_TCC0_WO4                         (78L)        
#define MUX_PC14F_TCC0_WO4                         (5L)        
#define PINMUX_PC14F_TCC0_WO4                      ((PIN_PC14F_TCC0_WO4 << 16) | MUX_PC14F_TCC0_WO4)
#define PORT_PC14F_TCC0_WO4                        ((1UL) << 14)

#define PIN_PC20F_TCC0_WO4                         (84L)        
#define MUX_PC20F_TCC0_WO4                         (5L)        
#define PINMUX_PC20F_TCC0_WO4                      ((PIN_PC20F_TCC0_WO4 << 16) | MUX_PC20F_TCC0_WO4)
#define PORT_PC20F_TCC0_WO4                        ((1UL) << 20)

#define PIN_PD11F_TCC0_WO4                         (107L)       
#define MUX_PD11F_TCC0_WO4                         (5L)        
#define PINMUX_PD11F_TCC0_WO4                      ((PIN_PD11F_TCC0_WO4 << 16) | MUX_PD11F_TCC0_WO4)
#define PORT_PD11F_TCC0_WO4                        ((1UL) << 11)

#define PIN_PA17G_TCC0_WO5                         (17L)        
#define MUX_PA17G_TCC0_WO5                         (6L)        
#define PINMUX_PA17G_TCC0_WO5                      ((PIN_PA17G_TCC0_WO5 << 16) | MUX_PA17G_TCC0_WO5)
#define PORT_PA17G_TCC0_WO5                        ((1UL) << 17)

#define PIN_PB17G_TCC0_WO5                         (49L)        
#define MUX_PB17G_TCC0_WO5                         (6L)        
#define PINMUX_PB17G_TCC0_WO5                      ((PIN_PB17G_TCC0_WO5 << 16) | MUX_PB17G_TCC0_WO5)
#define PORT_PB17G_TCC0_WO5                        ((1UL) << 17)

#define PIN_PB11F_TCC0_WO5                         (43L)        
#define MUX_PB11F_TCC0_WO5                         (5L)        
#define PINMUX_PB11F_TCC0_WO5                      ((PIN_PB11F_TCC0_WO5 << 16) | MUX_PB11F_TCC0_WO5)
#define PORT_PB11F_TCC0_WO5                        ((1UL) << 11)

#define PIN_PC15F_TCC0_WO5                         (79L)        
#define MUX_PC15F_TCC0_WO5                         (5L)        
#define PINMUX_PC15F_TCC0_WO5                      ((PIN_PC15F_TCC0_WO5 << 16) | MUX_PC15F_TCC0_WO5)
#define PORT_PC15F_TCC0_WO5                        ((1UL) << 15)

#define PIN_PC21F_TCC0_WO5                         (85L)        
#define MUX_PC21F_TCC0_WO5                         (5L)        
#define PINMUX_PC21F_TCC0_WO5                      ((PIN_PC21F_TCC0_WO5 << 16) | MUX_PC21F_TCC0_WO5)
#define PORT_PC21F_TCC0_WO5                        ((1UL) << 21)

#define PIN_PD12F_TCC0_WO5                         (108L)       
#define MUX_PD12F_TCC0_WO5                         (5L)        
#define PINMUX_PD12F_TCC0_WO5                      ((PIN_PD12F_TCC0_WO5 << 16) | MUX_PD12F_TCC0_WO5)
#define PORT_PD12F_TCC0_WO5                        ((1UL) << 12)

#define PIN_PA18G_TCC0_WO6                         (18L)        
#define MUX_PA18G_TCC0_WO6                         (6L)        
#define PINMUX_PA18G_TCC0_WO6                      ((PIN_PA18G_TCC0_WO6 << 16) | MUX_PA18G_TCC0_WO6)
#define PORT_PA18G_TCC0_WO6                        ((1UL) << 18)

#define PIN_PB30G_TCC0_WO6                         (62L)        
#define MUX_PB30G_TCC0_WO6                         (6L)        
#define PINMUX_PB30G_TCC0_WO6                      ((PIN_PB30G_TCC0_WO6 << 16) | MUX_PB30G_TCC0_WO6)
#define PORT_PB30G_TCC0_WO6                        ((1UL) << 30)

#define PIN_PA12F_TCC0_WO6                         (12L)        
#define MUX_PA12F_TCC0_WO6                         (5L)        
#define PINMUX_PA12F_TCC0_WO6                      ((PIN_PA12F_TCC0_WO6 << 16) | MUX_PA12F_TCC0_WO6)
#define PORT_PA12F_TCC0_WO6                        ((1UL) << 12)

#define PIN_PC22F_TCC0_WO6                         (86L)        
#define MUX_PC22F_TCC0_WO6                         (5L)        
#define PINMUX_PC22F_TCC0_WO6                      ((PIN_PC22F_TCC0_WO6 << 16) | MUX_PC22F_TCC0_WO6)
#define PORT_PC22F_TCC0_WO6                        ((1UL) << 22)

#define PIN_PA19G_TCC0_WO7                         (19L)        
#define MUX_PA19G_TCC0_WO7                         (6L)        
#define PINMUX_PA19G_TCC0_WO7                      ((PIN_PA19G_TCC0_WO7 << 16) | MUX_PA19G_TCC0_WO7)
#define PORT_PA19G_TCC0_WO7                        ((1UL) << 19)

#define PIN_PB31G_TCC0_WO7                         (63L)        
#define MUX_PB31G_TCC0_WO7                         (6L)        
#define PINMUX_PB31G_TCC0_WO7                      ((PIN_PB31G_TCC0_WO7 << 16) | MUX_PB31G_TCC0_WO7)
#define PORT_PB31G_TCC0_WO7                        ((1UL) << 31)

#define PIN_PA13F_TCC0_WO7                         (13L)        
#define MUX_PA13F_TCC0_WO7                         (5L)        
#define PINMUX_PA13F_TCC0_WO7                      ((PIN_PA13F_TCC0_WO7 << 16) | MUX_PA13F_TCC0_WO7)
#define PORT_PA13F_TCC0_WO7                        ((1UL) << 13)

#define PIN_PC23F_TCC0_WO7                         (87L)        
#define MUX_PC23F_TCC0_WO7                         (5L)        
#define PINMUX_PC23F_TCC0_WO7                      ((PIN_PC23F_TCC0_WO7 << 16) | MUX_PC23F_TCC0_WO7)
#define PORT_PC23F_TCC0_WO7                        ((1UL) << 23)

/* ========== PORT definition for TCC1 peripheral ========== */
#define PIN_PB10G_TCC1_WO0                         (42L)        
#define MUX_PB10G_TCC1_WO0                         (6L)        
#define PINMUX_PB10G_TCC1_WO0                      ((PIN_PB10G_TCC1_WO0 << 16) | MUX_PB10G_TCC1_WO0)
#define PORT_PB10G_TCC1_WO0                        ((1UL) << 10)

#define PIN_PC14G_TCC1_WO0                         (78L)        
#define MUX_PC14G_TCC1_WO0                         (6L)        
#define PINMUX_PC14G_TCC1_WO0                      ((PIN_PC14G_TCC1_WO0 << 16) | MUX_PC14G_TCC1_WO0)
#define PORT_PC14G_TCC1_WO0                        ((1UL) << 14)

#define PIN_PA16F_TCC1_WO0                         (16L)        
#define MUX_PA16F_TCC1_WO0                         (5L)        
#define PINMUX_PA16F_TCC1_WO0                      ((PIN_PA16F_TCC1_WO0 << 16) | MUX_PA16F_TCC1_WO0)
#define PORT_PA16F_TCC1_WO0                        ((1UL) << 16)

#define PIN_PB18F_TCC1_WO0                         (50L)        
#define MUX_PB18F_TCC1_WO0                         (5L)        
#define PINMUX_PB18F_TCC1_WO0                      ((PIN_PB18F_TCC1_WO0 << 16) | MUX_PB18F_TCC1_WO0)
#define PORT_PB18F_TCC1_WO0                        ((1UL) << 18)

#define PIN_PD20F_TCC1_WO0                         (116L)       
#define MUX_PD20F_TCC1_WO0                         (5L)        
#define PINMUX_PD20F_TCC1_WO0                      ((PIN_PD20F_TCC1_WO0 << 16) | MUX_PD20F_TCC1_WO0)
#define PORT_PD20F_TCC1_WO0                        ((1UL) << 20)

#define PIN_PB11G_TCC1_WO1                         (43L)        
#define MUX_PB11G_TCC1_WO1                         (6L)        
#define PINMUX_PB11G_TCC1_WO1                      ((PIN_PB11G_TCC1_WO1 << 16) | MUX_PB11G_TCC1_WO1)
#define PORT_PB11G_TCC1_WO1                        ((1UL) << 11)

#define PIN_PC15G_TCC1_WO1                         (79L)        
#define MUX_PC15G_TCC1_WO1                         (6L)        
#define PINMUX_PC15G_TCC1_WO1                      ((PIN_PC15G_TCC1_WO1 << 16) | MUX_PC15G_TCC1_WO1)
#define PORT_PC15G_TCC1_WO1                        ((1UL) << 15)

#define PIN_PA17F_TCC1_WO1                         (17L)        
#define MUX_PA17F_TCC1_WO1                         (5L)        
#define PINMUX_PA17F_TCC1_WO1                      ((PIN_PA17F_TCC1_WO1 << 16) | MUX_PA17F_TCC1_WO1)
#define PORT_PA17F_TCC1_WO1                        ((1UL) << 17)

#define PIN_PB19F_TCC1_WO1                         (51L)        
#define MUX_PB19F_TCC1_WO1                         (5L)        
#define PINMUX_PB19F_TCC1_WO1                      ((PIN_PB19F_TCC1_WO1 << 16) | MUX_PB19F_TCC1_WO1)
#define PORT_PB19F_TCC1_WO1                        ((1UL) << 19)

#define PIN_PD21F_TCC1_WO1                         (117L)       
#define MUX_PD21F_TCC1_WO1                         (5L)        
#define PINMUX_PD21F_TCC1_WO1                      ((PIN_PD21F_TCC1_WO1 << 16) | MUX_PD21F_TCC1_WO1)
#define PORT_PD21F_TCC1_WO1                        ((1UL) << 21)

#define PIN_PA12G_TCC1_WO2                         (12L)        
#define MUX_PA12G_TCC1_WO2                         (6L)        
#define PINMUX_PA12G_TCC1_WO2                      ((PIN_PA12G_TCC1_WO2 << 16) | MUX_PA12G_TCC1_WO2)
#define PORT_PA12G_TCC1_WO2                        ((1UL) << 12)

#define PIN_PA14G_TCC1_WO2                         (14L)        
#define MUX_PA14G_TCC1_WO2                         (6L)        
#define PINMUX_PA14G_TCC1_WO2                      ((PIN_PA14G_TCC1_WO2 << 16) | MUX_PA14G_TCC1_WO2)
#define PORT_PA14G_TCC1_WO2                        ((1UL) << 14)

#define PIN_PA18F_TCC1_WO2                         (18L)        
#define MUX_PA18F_TCC1_WO2                         (5L)        
#define PINMUX_PA18F_TCC1_WO2                      ((PIN_PA18F_TCC1_WO2 << 16) | MUX_PA18F_TCC1_WO2)
#define PORT_PA18F_TCC1_WO2                        ((1UL) << 18)

#define PIN_PB20F_TCC1_WO2                         (52L)        
#define MUX_PB20F_TCC1_WO2                         (5L)        
#define PINMUX_PB20F_TCC1_WO2                      ((PIN_PB20F_TCC1_WO2 << 16) | MUX_PB20F_TCC1_WO2)
#define PORT_PB20F_TCC1_WO2                        ((1UL) << 20)

#define PIN_PB26F_TCC1_WO2                         (58L)        
#define MUX_PB26F_TCC1_WO2                         (5L)        
#define PINMUX_PB26F_TCC1_WO2                      ((PIN_PB26F_TCC1_WO2 << 16) | MUX_PB26F_TCC1_WO2)
#define PORT_PB26F_TCC1_WO2                        ((1UL) << 26)

#define PIN_PA13G_TCC1_WO3                         (13L)        
#define MUX_PA13G_TCC1_WO3                         (6L)        
#define PINMUX_PA13G_TCC1_WO3                      ((PIN_PA13G_TCC1_WO3 << 16) | MUX_PA13G_TCC1_WO3)
#define PORT_PA13G_TCC1_WO3                        ((1UL) << 13)

#define PIN_PA15G_TCC1_WO3                         (15L)        
#define MUX_PA15G_TCC1_WO3                         (6L)        
#define PINMUX_PA15G_TCC1_WO3                      ((PIN_PA15G_TCC1_WO3 << 16) | MUX_PA15G_TCC1_WO3)
#define PORT_PA15G_TCC1_WO3                        ((1UL) << 15)

#define PIN_PA19F_TCC1_WO3                         (19L)        
#define MUX_PA19F_TCC1_WO3                         (5L)        
#define PINMUX_PA19F_TCC1_WO3                      ((PIN_PA19F_TCC1_WO3 << 16) | MUX_PA19F_TCC1_WO3)
#define PORT_PA19F_TCC1_WO3                        ((1UL) << 19)

#define PIN_PB21F_TCC1_WO3                         (53L)        
#define MUX_PB21F_TCC1_WO3                         (5L)        
#define PINMUX_PB21F_TCC1_WO3                      ((PIN_PB21F_TCC1_WO3 << 16) | MUX_PB21F_TCC1_WO3)
#define PORT_PB21F_TCC1_WO3                        ((1UL) << 21)

#define PIN_PB27F_TCC1_WO3                         (59L)        
#define MUX_PB27F_TCC1_WO3                         (5L)        
#define PINMUX_PB27F_TCC1_WO3                      ((PIN_PB27F_TCC1_WO3 << 16) | MUX_PB27F_TCC1_WO3)
#define PORT_PB27F_TCC1_WO3                        ((1UL) << 27)

#define PIN_PA08G_TCC1_WO4                         (8L)         
#define MUX_PA08G_TCC1_WO4                         (6L)        
#define PINMUX_PA08G_TCC1_WO4                      ((PIN_PA08G_TCC1_WO4 << 16) | MUX_PA08G_TCC1_WO4)
#define PORT_PA08G_TCC1_WO4                        ((1UL) << 8)

#define PIN_PC10G_TCC1_WO4                         (74L)        
#define MUX_PC10G_TCC1_WO4                         (6L)        
#define PINMUX_PC10G_TCC1_WO4                      ((PIN_PC10G_TCC1_WO4 << 16) | MUX_PC10G_TCC1_WO4)
#define PORT_PC10G_TCC1_WO4                        ((1UL) << 10)

#define PIN_PA20F_TCC1_WO4                         (20L)        
#define MUX_PA20F_TCC1_WO4                         (5L)        
#define PINMUX_PA20F_TCC1_WO4                      ((PIN_PA20F_TCC1_WO4 << 16) | MUX_PA20F_TCC1_WO4)
#define PORT_PA20F_TCC1_WO4                        ((1UL) << 20)

#define PIN_PB28F_TCC1_WO4                         (60L)        
#define MUX_PB28F_TCC1_WO4                         (5L)        
#define PINMUX_PB28F_TCC1_WO4                      ((PIN_PB28F_TCC1_WO4 << 16) | MUX_PB28F_TCC1_WO4)
#define PORT_PB28F_TCC1_WO4                        ((1UL) << 28)

#define PIN_PA09G_TCC1_WO5                         (9L)         
#define MUX_PA09G_TCC1_WO5                         (6L)        
#define PINMUX_PA09G_TCC1_WO5                      ((PIN_PA09G_TCC1_WO5 << 16) | MUX_PA09G_TCC1_WO5)
#define PORT_PA09G_TCC1_WO5                        ((1UL) << 9)

#define PIN_PC11G_TCC1_WO5                         (75L)        
#define MUX_PC11G_TCC1_WO5                         (6L)        
#define PINMUX_PC11G_TCC1_WO5                      ((PIN_PC11G_TCC1_WO5 << 16) | MUX_PC11G_TCC1_WO5)
#define PORT_PC11G_TCC1_WO5                        ((1UL) << 11)

#define PIN_PA21F_TCC1_WO5                         (21L)        
#define MUX_PA21F_TCC1_WO5                         (5L)        
#define PINMUX_PA21F_TCC1_WO5                      ((PIN_PA21F_TCC1_WO5 << 16) | MUX_PA21F_TCC1_WO5)
#define PORT_PA21F_TCC1_WO5                        ((1UL) << 21)

#define PIN_PB29F_TCC1_WO5                         (61L)        
#define MUX_PB29F_TCC1_WO5                         (5L)        
#define PINMUX_PB29F_TCC1_WO5                      ((PIN_PB29F_TCC1_WO5 << 16) | MUX_PB29F_TCC1_WO5)
#define PORT_PB29F_TCC1_WO5                        ((1UL) << 29)

#define PIN_PA10G_TCC1_WO6                         (10L)        
#define MUX_PA10G_TCC1_WO6                         (6L)        
#define PINMUX_PA10G_TCC1_WO6                      ((PIN_PA10G_TCC1_WO6 << 16) | MUX_PA10G_TCC1_WO6)
#define PORT_PA10G_TCC1_WO6                        ((1UL) << 10)

#define PIN_PC12G_TCC1_WO6                         (76L)        
#define MUX_PC12G_TCC1_WO6                         (6L)        
#define PINMUX_PC12G_TCC1_WO6                      ((PIN_PC12G_TCC1_WO6 << 16) | MUX_PC12G_TCC1_WO6)
#define PORT_PC12G_TCC1_WO6                        ((1UL) << 12)

#define PIN_PA22F_TCC1_WO6                         (22L)        
#define MUX_PA22F_TCC1_WO6                         (5L)        
#define PINMUX_PA22F_TCC1_WO6                      ((PIN_PA22F_TCC1_WO6 << 16) | MUX_PA22F_TCC1_WO6)
#define PORT_PA22F_TCC1_WO6                        ((1UL) << 22)

#define PIN_PA11G_TCC1_WO7                         (11L)        
#define MUX_PA11G_TCC1_WO7                         (6L)        
#define PINMUX_PA11G_TCC1_WO7                      ((PIN_PA11G_TCC1_WO7 << 16) | MUX_PA11G_TCC1_WO7)
#define PORT_PA11G_TCC1_WO7                        ((1UL) << 11)

#define PIN_PC13G_TCC1_WO7                         (77L)        
#define MUX_PC13G_TCC1_WO7                         (6L)        
#define PINMUX_PC13G_TCC1_WO7                      ((PIN_PC13G_TCC1_WO7 << 16) | MUX_PC13G_TCC1_WO7)
#define PORT_PC13G_TCC1_WO7                        ((1UL) << 13)

#define PIN_PA23F_TCC1_WO7                         (23L)        
#define MUX_PA23F_TCC1_WO7                         (5L)        
#define PINMUX_PA23F_TCC1_WO7                      ((PIN_PA23F_TCC1_WO7 << 16) | MUX_PA23F_TCC1_WO7)
#define PORT_PA23F_TCC1_WO7                        ((1UL) << 23)

/* ========== PORT definition for TCC2 peripheral ========== */
#define PIN_PA14F_TCC2_WO0                         (14L)        
#define MUX_PA14F_TCC2_WO0                         (5L)        
#define PINMUX_PA14F_TCC2_WO0                      ((PIN_PA14F_TCC2_WO0 << 16) | MUX_PA14F_TCC2_WO0)
#define PORT_PA14F_TCC2_WO0                        ((1UL) << 14)

#define PIN_PA30F_TCC2_WO0                         (30L)        
#define MUX_PA30F_TCC2_WO0                         (5L)        
#define PINMUX_PA30F_TCC2_WO0                      ((PIN_PA30F_TCC2_WO0 << 16) | MUX_PA30F_TCC2_WO0)
#define PORT_PA30F_TCC2_WO0                        ((1UL) << 30)

#define PIN_PA15F_TCC2_WO1                         (15L)        
#define MUX_PA15F_TCC2_WO1                         (5L)        
#define PINMUX_PA15F_TCC2_WO1                      ((PIN_PA15F_TCC2_WO1 << 16) | MUX_PA15F_TCC2_WO1)
#define PORT_PA15F_TCC2_WO1                        ((1UL) << 15)

#define PIN_PA31F_TCC2_WO1                         (31L)        
#define MUX_PA31F_TCC2_WO1                         (5L)        
#define PINMUX_PA31F_TCC2_WO1                      ((PIN_PA31F_TCC2_WO1 << 16) | MUX_PA31F_TCC2_WO1)
#define PORT_PA31F_TCC2_WO1                        ((1UL) << 31)

#define PIN_PA24F_TCC2_WO2                         (24L)        
#define MUX_PA24F_TCC2_WO2                         (5L)        
#define PINMUX_PA24F_TCC2_WO2                      ((PIN_PA24F_TCC2_WO2 << 16) | MUX_PA24F_TCC2_WO2)
#define PORT_PA24F_TCC2_WO2                        ((1UL) << 24)

#define PIN_PB02F_TCC2_WO2                         (34L)        
#define MUX_PB02F_TCC2_WO2                         (5L)        
#define PINMUX_PB02F_TCC2_WO2                      ((PIN_PB02F_TCC2_WO2 << 16) | MUX_PB02F_TCC2_WO2)
#define PORT_PB02F_TCC2_WO2                        ((1UL) << 2)

/* ========== PORT definition for TCC3 peripheral ========== */
#define PIN_PB12F_TCC3_WO0                         (44L)        
#define MUX_PB12F_TCC3_WO0                         (5L)        
#define PINMUX_PB12F_TCC3_WO0                      ((PIN_PB12F_TCC3_WO0 << 16) | MUX_PB12F_TCC3_WO0)
#define PORT_PB12F_TCC3_WO0                        ((1UL) << 12)

#define PIN_PB16F_TCC3_WO0                         (48L)        
#define MUX_PB16F_TCC3_WO0                         (5L)        
#define PINMUX_PB16F_TCC3_WO0                      ((PIN_PB16F_TCC3_WO0 << 16) | MUX_PB16F_TCC3_WO0)
#define PORT_PB16F_TCC3_WO0                        ((1UL) << 16)

#define PIN_PB13F_TCC3_WO1                         (45L)        
#define MUX_PB13F_TCC3_WO1                         (5L)        
#define PINMUX_PB13F_TCC3_WO1                      ((PIN_PB13F_TCC3_WO1 << 16) | MUX_PB13F_TCC3_WO1)
#define PORT_PB13F_TCC3_WO1                        ((1UL) << 13)

#define PIN_PB17F_TCC3_WO1                         (49L)        
#define MUX_PB17F_TCC3_WO1                         (5L)        
#define PINMUX_PB17F_TCC3_WO1                      ((PIN_PB17F_TCC3_WO1 << 16) | MUX_PB17F_TCC3_WO1)
#define PORT_PB17F_TCC3_WO1                        ((1UL) << 17)

/* ========== PORT definition for TCC4 peripheral ========== */
#define PIN_PB14F_TCC4_WO0                         (46L)        
#define MUX_PB14F_TCC4_WO0                         (5L)        
#define PINMUX_PB14F_TCC4_WO0                      ((PIN_PB14F_TCC4_WO0 << 16) | MUX_PB14F_TCC4_WO0)
#define PORT_PB14F_TCC4_WO0                        ((1UL) << 14)

#define PIN_PB30F_TCC4_WO0                         (62L)        
#define MUX_PB30F_TCC4_WO0                         (5L)        
#define PINMUX_PB30F_TCC4_WO0                      ((PIN_PB30F_TCC4_WO0 << 16) | MUX_PB30F_TCC4_WO0)
#define PORT_PB30F_TCC4_WO0                        ((1UL) << 30)

#define PIN_PB15F_TCC4_WO1                         (47L)        
#define MUX_PB15F_TCC4_WO1                         (5L)        
#define PINMUX_PB15F_TCC4_WO1                      ((PIN_PB15F_TCC4_WO1 << 16) | MUX_PB15F_TCC4_WO1)
#define PORT_PB15F_TCC4_WO1                        ((1UL) << 15)

#define PIN_PB31F_TCC4_WO1                         (63L)        
#define MUX_PB31F_TCC4_WO1                         (5L)        
#define PINMUX_PB31F_TCC4_WO1                      ((PIN_PB31F_TCC4_WO1 << 16) | MUX_PB31F_TCC4_WO1)
#define PORT_PB31F_TCC4_WO1                        ((1UL) << 31)

/* ========== PORT definition for USB peripheral ========== */
#define PIN_PA24H_USB_DM                           (24L)        
#define MUX_PA24H_USB_DM                           (7L)        
#define PINMUX_PA24H_USB_DM                        ((PIN_PA24H_USB_DM << 16) | MUX_PA24H_USB_DM)
#define PORT_PA24H_USB_DM                          ((1UL) << 24)

#define PIN_PA25H_USB_DP                           (25L)        
#define MUX_PA25H_USB_DP                           (7L)        
#define PINMUX_PA25H_USB_DP                        ((PIN_PA25H_USB_DP << 16) | MUX_PA25H_USB_DP)
#define PORT_PA25H_USB_DP                          ((1UL) << 25)

#define PIN_PA23H_USB_SOF_1KHZ                     (23L)        
#define MUX_PA23H_USB_SOF_1KHZ                     (7L)        
#define PINMUX_PA23H_USB_SOF_1KHZ                  ((PIN_PA23H_USB_SOF_1KHZ << 16) | MUX_PA23H_USB_SOF_1KHZ)
#define PORT_PA23H_USB_SOF_1KHZ                    ((1UL) << 23)

#define PIN_PB22H_USB_SOF_1KHZ                     (54L)        
#define MUX_PB22H_USB_SOF_1KHZ                     (7L)        
#define PINMUX_PB22H_USB_SOF_1KHZ                  ((PIN_PB22H_USB_SOF_1KHZ << 16) | MUX_PB22H_USB_SOF_1KHZ)
#define PORT_PB22H_USB_SOF_1KHZ                    ((1UL) << 22)



#endif /* _SAME54P20A_GPIO_H_ */

