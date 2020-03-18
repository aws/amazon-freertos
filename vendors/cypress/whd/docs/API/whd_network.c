/*
 * Sample code used for reference only
 */

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param buffer : the incoming ethernet packet
 */
void host_network_process_ethernet_data(whd_interface_t ifp, /*@only@*/ whd_buffer_t buffer)
{
    struct eth_hdr *ethernet_header;
    struct netif *tmp_netif;
    u8_t result;
    uint16_t ethertype;
    struct pbuf *lwip_buffer = (struct pbuf *)buffer;

    if (buffer == NULL)
        return;

    /* points to packet payload, which starts with an Ethernet header */
    ethernet_header = (struct eth_hdr *)lwip_buffer->payload;

    ethertype = htons(ethernet_header->type);

    /* Check if this is an 802.1Q VLAN tagged packet */
    if (ethertype == WHD_ETHERTYPE_8021Q)
    {
        /* Need to remove the 4 octet VLAN Tag, by moving src and dest addresses 4 octets to the right,
        * and then read the actual ethertype. The VLAN ID and priority fields are currently ignored. */
        uint8_t temp_buffer[12];
        memcpy(temp_buffer, lwip_buffer->payload, 12);
        memcpy( ( (uint8_t *)lwip_buffer->payload ) + 4, temp_buffer, 12 );

        lwip_buffer->payload = ( (uint8_t *)lwip_buffer->payload ) + 4;
        lwip_buffer->len = (u16_t)(lwip_buffer->len - 4);

        ethernet_header = (struct eth_hdr *)lwip_buffer->payload;
        ethertype = htons(ethernet_header->type);
    }

    switch (ethertype)
    {
        case WHD_ETHERTYPE_IPv4:
            /* Find the netif object matching the provided interface */
            for (tmp_netif = netif_list; (tmp_netif != NULL) && (tmp_netif->state != (void *)ifp);
                 tmp_netif = tmp_netif->next)
            {
            }

            if (tmp_netif == NULL)
            {
                /* Received a packet for a network interface is not initialised Cannot do anything with packet - just drop it. */
                result = pbuf_free(lwip_buffer);
                LWIP_ASSERT("Failed to release packet buffer", (result != (u8_t)0) );
                lwip_buffer = NULL;
                return;
            }

            /* Send to packet to tcpip_thread to process */
            if (tcpip_input(lwip_buffer, tmp_netif) != ERR_OK)
            {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n") );

                /* Stop lint warning - packet has not been released in this case */ /*@-usereleased@*/
                result = pbuf_free(lwip_buffer);
                /*@+usereleased@*/
                LWIP_ASSERT("Failed to release packet buffer", (result != (u8_t)0) );
                lwip_buffer = NULL;
            }
            break;

        case WHD_ETHERTYPE_EAPOL:
            /* Process EAPOL packet */
            break;

        default:
            result = pbuf_free(lwip_buffer);
            LWIP_ASSERT("Failed to release packet buffer", (result != (u8_t)0) );
            lwip_buffer = NULL;
            break;
    }
}

whd_netif_funcs_t netif_ops =
{
    .whd_network_process_ethernet_data = host_network_process_ethernet_data,
};

