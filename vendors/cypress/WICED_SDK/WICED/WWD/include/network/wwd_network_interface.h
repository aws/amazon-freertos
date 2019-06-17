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

/** @file
 *  Defines the WICED Network Interface.
 *
 *  Provides prototypes for functions that allow WICED to communicate
 *  with a network stack in an abstract way.
 */

#ifndef HEADER_WWD_NETWORK_INTERFACE_H_INCLUDED
#define HEADER_WWD_NETWORK_INTERFACE_H_INCLUDED

#include "wwd_buffer.h"
#include "wwd_constants.h"

#ifdef __cplusplus
extern "C"
{
#endif



/* @addtogroup netif Network Interface
 * Allows WICED to communicate with a network stack in an abstract way.
 *  @{
 */

/******************************************************
 *       Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef void (*wwd_network_filter_ethernet_packet_t)(wiced_buffer_t buffer, void* userdata);

/******************************************************
 *             Function declarations
 ******************************************************/

/*
 * Called by WICED to pass received data to the network stack
 *
 * Implemented in 'network adapter driver' which is specific to the
 * network stack in use.
 * Packets received from the Wi-Fi network by WICED are forwarded to this function which
 * must be implemented in the network interface. Ethernet headers
 * are present at the start of these packet buffers.
 *
 * This function is called asynchronously in the context of the
 * WICED thread whenever new data has arrived.
 * Packet buffers are allocated within WICED, and ownership is transferred
 * to the network stack. The network stack or application is thus
 * responsible for releasing the packet buffers.
 * Most packet buffering systems have a pointer to the 'current point' within
 * the packet buffer. When this function is called, the pointer points
 * to the start of the Ethernet header. There is other inconsequential data
 * before the Ethernet header.
 *
 * It is preferable that the @ref host_network_process_ethernet_data function simply puts
 * the received packet on a queue for processing by another thread. This avoids the
 * WICED thread being unnecessarily tied up which would delay other packets
 * being transmitted or received.
 *
 * @param buffer : Handle of the packet which has just been received. Responsibility for
 *                 releasing this buffer is transferred from WICED at this point.
 * @param interface : The interface (AP or STA) on which the packet was received.
 *
 */
/*@external@*/ extern void host_network_process_ethernet_data( /*@only@*/ wiced_buffer_t buffer, wwd_interface_t interface ); /* Network stack assumes responsibility for freeing buffer */

/* Functions provided by WICED that may be called by Network Stack */

/*
 * Called by the Network Stack to send an ethernet frame
 *
 * Implemented in 'network adapter driver' which is specific to the
 * network stack in use.
 * This function takes Ethernet data from the network stack and queues it for transmission over the wireless network.
 * The function can be called from any thread context as it is thread safe, however
 * it must not be called from interrupt context since it can block while waiting
 * for a lock on the transmit queue.
 *
 * This function returns immediately after the packet has been queued for transmit,
 * NOT after it has been transmitted.  Packet buffers passed to the WICED core
 * are released inside the WICED core once they have been transmitted.
 *
 * Some network stacks assume the driver send function blocks until the packet has been physically sent. This
 * type of stack typically releases the packet buffer immediately after the driver send function returns.
 * In this case, and assuming the buffering system can count references to packet buffers, the driver send function
 * can take an additional reference to the packet buffer. This enables the network stack and the WICED core driver
 * to independently release their own packet buffer references.
 *
 * @param buffer : Handle of the packet buffer to be sent.
 * @param interface : the interface over which to send the packet (AP or STA)
 *
 */
extern void wwd_network_send_ethernet_data( /*@only@*/ wiced_buffer_t buffer, wwd_interface_t interface ); /* Returns immediately - Wiced_buffer_tx_completed will be called once the transmission has finished */

/*
 * Called by a WICED application to filter packets of the specified ethertype before they are passed to the network stack.
 *
 * NOTE: The packet passed to the filter callback will be processed normally by the network stack when the callback returns.
 *
 * @param ethertype                : Ethertype to filter.
 * @param interface                : The interface (AP or STA) on which the packet is received.
 * @param ethernet_packet_callback : Callback routine to invoke when packet with matching ethertype is received.
 * @param userdata                 : Opaque data pointer to pass back to application in callback.
 */
extern void host_network_set_ethertype_filter( uint16_t ethertype, wwd_interface_t interface, wwd_network_filter_ethernet_packet_t ethernet_packet_callback, void* userdata );

/*  @} */


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef HEADER_WWD_NETWORK_INTERFACE_H_INCLUDED */
