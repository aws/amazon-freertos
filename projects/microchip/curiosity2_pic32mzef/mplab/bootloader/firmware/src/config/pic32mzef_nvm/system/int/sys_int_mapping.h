/*******************************************************************************
  Interrupt System Service Mapping File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int_mapping.h

  Summary:
    Interrupt System Service mapping file.

  Description:
    This header file contains the mapping of the APIs defined in the API header
    to either the function implementations or macro implementation or the
    specific variant implementation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

#ifndef SYS_INT_MAPPING_H
#define SYS_INT_MAPPING_H

// *****************************************************************************
// *****************************************************************************
// Section: Interrupt System Service Mapping
// *****************************************************************************
// *****************************************************************************

#define SYS_INT_IsEnabled()                 ((bool)(_CP0_GET_STATUS() & 0x01))
#define SYS_INT_SourceEnable( source )      EVIC_SourceEnable( source )
#define SYS_INT_SourceIsEnabled( source )   EVIC_SourceIsEnabled( source )
#define SYS_INT_SourceStatusGet( source )   EVIC_SourceStatusGet( source )
#define SYS_INT_SourceStatusSet( source )   EVIC_SourceStatusSet( source )
#define SYS_INT_SourceStatusClear( source ) EVIC_SourceStatusClear( source )

#endif // SYS_INT_MAPPING_H
