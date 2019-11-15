/*******************************************************************************
  TCP/IP notifications Header file

  Company:
    Microchip Technology Inc.

  File Name:
   tcpip_notify.h

  Summary:
   TCPIP notifications mechanism header file

  Description:
    This source file contains the internal notifications API
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef __TCPIP_NOTIFY_H_
#define __TCPIP_NOTIFY_H_

#include <stdint.h>
#include <stdbool.h>


// *****************************************************************************
// *****************************************************************************
// Section: API definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
Function:
    bool      TCPIP_Notification_Initialize(PROTECTED_SINGLE_LIST* notifyList)

  Summary:
    Initializes a notification list

  Description:
    Initializes a notification list and makes it ready for use.

  Precondition:
    notifyList is a valid list

  Parameters:
    notifyList  - address of the list to be initialized    

  Returns:
        On Success - true
        On Failure - false

  Example:
     None

  Remarks:
    This call creates the protection object (semaphire) associated with this list.
*/
bool        TCPIP_Notification_Initialize(PROTECTED_SINGLE_LIST* notifyList);



// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_Deinitialize(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)

  Summary:
    Deinitializes a notification list

  Description:
    Deinitializes a notification list and frees associated resources

  Precondition:
    notifyList is a valid initialized list

  Parameters:
    notifyList  - address of the list to be deinitialized    
    heapH       - Heap to be used for freeing node resources

  Returns:
        None

  Example:
     None

  Remarks:
    This call deletes the protection object (semaphire) associated with this list.

    heapH has to match the one that was used for adding nodes
*/
void        TCPIP_Notification_Deinitialize(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH);




// *****************************************************************************
/*
Function:
    SGL_LIST_NODE*      TCPIP_Notification_Add(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)

  Summary:
    Adds a new notification

  Description:
    Tries to create a new SGL_LIST_NODE and add it to the tail of the notifyList.

  Precondition:
    the module notifyList should have been initialized

  Parameters:
    notifyList  - address of the list where the new entry is to be added    
    heapH       - Heap to be used for adding the new node.
                  This could be module specific.
    nBytes      - size of the entry - module specific

  Returns:
    SGL_LIST_NODE pointer to the created node 
        On Success - Returns a valid pointer
        On Failure - null pointer if memory call failed

  Example:
     None

  Remarks:
    It is up to each module to set the specific data associated with this entry.
    This function only creates a new node and inserts it properly in the notification list.
*/
SGL_LIST_NODE*      TCPIP_Notification_Add(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);


// *****************************************************************************
/*
Function:
    bool      TCPIP_Notification_Remove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)

  Summary:
    Removes a notification 

  Description:
    Tries to remove a SGL_LIST_NODE from the notifyList.

  Precondition:
    the node should have been added to the notifyList with TCPIP_Notification_Add()

  Parameters:
    node        - node to be deregistered
    heapH       - Heap to be used for freeing up memory
                  This could be module specific.
    notifyList  - address of the list from where the new entry is to be removed    

  Returns:
        true  - for success
        false - if failure

  Example:
     None

  Remarks:
    It is up to each module to remove/free the specific data associated with this entry.
    This function only removes the node from the notification list and then frees the associated memory
*/
bool      TCPIP_Notification_Remove(SGL_LIST_NODE* node, PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH);

// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_RemoveAll(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH)

  Summary:
    Removes all notifications from a list 

  Description:
    Tries to remove all notifications from the notifyList.

  Precondition:
    the nodes should have been added to the notifyList with TCPIP_Notification_Add()

  Parameters:
    heapH       - Heap to be used for freeing up memory
                  This could be module specific.
    notifyList  - address of the list from where the new entry is to be removed    

  Returns:
        None

  Example:
     None

  Remarks:
    It is up to each module to remove/free the specific data associated with this entry.
    This function only removes the node from the notification list and then frees the associated memory
*/
void      TCPIP_Notification_RemoveAll(PROTECTED_SINGLE_LIST* notifyList, TCPIP_STACK_HEAP_HANDLE heapH);


// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_Lock(PROTECTED_SINGLE_LIST* notifyList)

  Summary:
    Locks access to a notification list 

  Description:
    Locks access to a notification list 
    The list can be traversed safely.

  Precondition:
    The notifyList should have been properly initialized

  Parameters:
    notifyList  - list to lock 

  Returns:
        None

  Example:
     None

  Remarks:
    None
*/
static __inline__ void __attribute__((always_inline)) TCPIP_Notification_Lock(PROTECTED_SINGLE_LIST* notifyList)
{
    TCPIP_Helper_ProtectedSingleListLock(notifyList);
}

// *****************************************************************************
/*
Function:
    void      TCPIP_Notification_Unlock(PROTECTED_SINGLE_LIST* notifyList)

  Summary:
    Unlocks access to a notification list 

  Description:
    Unlocks access to a notification list 

  Precondition:
    The notifyList should have been properly initialized and locked
    before this call

  Parameters:
    notifyList  - list to unlock 

  Returns:
        None

  Example:
     None

  Remarks:
    None
*/
static __inline__ void __attribute__((always_inline)) TCPIP_Notification_Unlock(PROTECTED_SINGLE_LIST* notifyList)
{
    TCPIP_Helper_ProtectedSingleListUnlock(notifyList);
}



#endif  // __TCPIP_NOTIFY_H_



