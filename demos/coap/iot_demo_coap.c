/*
 * /*
 *  Created on: Nov 10, 2020
 *  Authors: Mohammed Abdelmaksoud & Hatim Jamali
 *  1NCE GmbH
 *
 */

/**
 * @file iot_demo_coap.c
 * @brief Demonstrates usage of lobaro COAP library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"
#include "platform/iot_network.h"

/* COAP include. */
#include "coap_main.h"


/*-----------------------------------------------------------*/



CoAP_RespHandler_fn_t CoAP_Resp_handler( CoAP_Message_t * pRespMsg,
                                         NetEp_t * Sender )
{
    configPRINTF(
        ( "MESSAGE Payload : %s \r\n", pRespMsg->Payload ) );
    PrintEndpoint( Sender );
}

/* Declaration of demo function. */
int RuncoapDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the COAP demo, called by the demo runner.
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int RuncoapDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface )
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_FAILURE;

    SocketsSockaddr_t ServerAddress;
    NetPacket_t pPacket;

    configPRINTF( ( "Connecting to CoAP server\r\n" ) );
    ServerAddress.usPort = SOCKETS_htons( configCOAP_PORT );
    ServerAddress.ulAddress = SOCKETS_inet_addr_quick( configCOAP_SERVER_ADDR0,
                                                       configCOAP_SERVER_ADDR1, configCOAP_SERVER_ADDR2,
                                                       configCOAP_SERVER_ADDR3 );
    const NetEp_t ServerEp =
    {
        .NetType = IPV4, .NetPort =
            configCOAP_PORT, .NetAddr= { .IPv4 = { .u8 =
                               {
                                   configCOAP_SERVER_ADDR0, configCOAP_SERVER_ADDR1,
                                   configCOAP_SERVER_ADDR2, configCOAP_SERVER_ADDR3
                               } } }
    };


    /* Create UDP Socket */
    Socket_t udp = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_DGRAM,
                                   SOCKETS_IPPROTO_UDP );
    CoAP_Socket_t * socket = AllocSocket();

    /* Connect to CoAP server */
    if( SOCKETS_Connect( udp, &ServerAddress, sizeof( ServerAddress ) ) == 0 )
    {
        configPRINTF( ( "Connected to CoAP server \r\n" ) );

        /* Add payload */
        char pcTransmittedString[] = "hello from CoAP";

        /* Add socket and Network Interface configuration */
        socket->Handle = udp;
        socket->Tx = CoAP_Send_Wifi;

        /* Create confirmable CoAP POST packet with URI Path option */
        CoAP_Message_t * pReqMsg = CoAP_CreateMessage( CON, REQ_POST,
                                                       CoAP_GetNextMid(), CoAP_GenerateToken() );

        CoAP_addNewPayloadToMessage( pReqMsg, pcTransmittedString,
                                     strlen( pcTransmittedString ) );
        CoAP_AddOption( pReqMsg, OPT_NUM_URI_PATH, configCOAP_URI_PATH, strlen( configCOAP_URI_PATH ) );


        /* Create CoAP Client Interaction to send a confirmable POST Request  */
        CoAP_StartNewClientInteraction( pReqMsg, socket->Handle, &ServerEp, CoAP_Resp_handler );
        CoAP_Interaction_t * pIA = CoAP_GetLongestPendingInteraction();


        /* Execute the Interaction list  */

        while( pIA != NULL )
        {
            CoAP_doWork();

            if( pIA->State == COAP_STATE_WAITING_RESPONSE )
            {
                CoAP_Recv_Wifi( socket->Handle, &pPacket, ServerEp );
            }

            pIA = CoAP_GetLongestPendingInteraction();

            if( pIA->State == COAP_STATE_FINISHED )
            {
                status = EXIT_SUCCESS;
            }
        }
    }

    return status;
}

/*-----------------------------------------------------------*/
