/*******************************************************************************
  MIIM Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_config.h

  Summary:
    MIIM driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2016-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

//DOM-IGNORE-END

#ifndef _DRV_MIIM_CONFIG_H
#define _DRV_MIIM_CONFIG_H


// *****************************************************************************
// *****************************************************************************
// Section: MIIM Configuration
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MIIM hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver.

  Description:
    This definition selects the maximum number of hardware instances that can be
    supported by the dynamic driver.
    Usually set to 1.

  Remarks:
    None.
*/

#define DRV_MIIM_INSTANCES_NUMBER                1

// *****************************************************************************
/* MIIM instance operations

  Summary:
    Selects the maximum number of simultaneous operations for an instance.

  Description:
    This definition selects the maximum number of simultaneous operations
    that can be supported by this driver.
    Note that this represents operations for all clients

  Remarks:
    None.
*/

#define DRV_MIIM_INSTANCE_OPERATIONS        4


// *****************************************************************************
/* MIIM number of clients

  Summary:
    Selects the maximum number of clients.

  Description:
    This definition select the MIIM Maximum Number of Clients per instance.

  Remarks:
    By default the 1st MIIM client is the DRV_ETHPHY.
    An extra client is allowed.
  *************************************************************************/

#define DRV_MIIM_INSTANCE_CLIENTS                2

// *****************************************************************************
/* MIIM client Operation Protection

  Summary:
    Enables/Disables Client Operation Protection feature.

  Description:
    Because of the recirculation of the operation handles and client handles
    the possibility exists that a misbehaved client inadvertently gets the results
    of a previous completed operations that now belongs to a different client.
    When this feature is enabled, extra protection is added for an operation handle
    to uniquely identify a client that has started the operation and extra check is done
    that operation belongs to the client that asks for the result.


  Remarks:
    Set the value to 1 to enable, 0 to disable the feature.

    Enabling this feature requires a small overhead in code and data size.

  *************************************************************************/

#define DRV_MIIM_CLIENT_OP_PROTECTION       0

// *****************************************************************************
/* MIIM PHY Commands

  Summary:
    Enables/Disables MIIM commands feature.

  Description:
    Adds a MIIM command to the TCP/IP command menu allowing to read/write
    a PHY register.

  Remarks:
    Set the value to 1 to enable, 0 to disable the feature.

    Currently the MIIM commands are integrated in the TCP/IP commands.
    To have the MIIM commands available the TCP/IP commands need to be enabled.

  *************************************************************************/

#define DRV_MIIM_COMMANDS       0


#endif // #ifndef _DRV_MIIM_CONFIG_H

/*******************************************************************************
 End of File
*/

