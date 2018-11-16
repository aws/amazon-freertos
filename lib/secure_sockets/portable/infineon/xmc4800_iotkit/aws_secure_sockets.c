/*
 * Amazon FreeRTOS Secure Sockets for Infineon XMC4800 IoT Connectivity Kit V1.0.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright (c) 2018, Infineon Technologies AG
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 */

/**
 * @file aws_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* TLS includes. */
#include "aws_tls.h"
#include "aws_pkcs11.h"

/* Socket and WiFi interface includes. */
#include "aws_wifi.h"
#include "aws_secure_sockets.h"
#include "esp/esp_netconn.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/**
 * @brief A Flag to indicate whether or not a socket is
 * secure i.e. it uses TLS or not.
 */
#define securesocketsSOCKET_SECURE_FLAG                 ( 1UL << 0 )

/**
 * @brief A flag to indicate whether or not a socket is closed
 * for receive.
 */
#define securesocketsSOCKET_READ_CLOSED_FLAG            ( 1UL << 1 )

/**
 * @brief A flag to indicate whether or not a socket is closed
 * for send.
 */
#define securesocketsSOCKET_WRITE_CLOSED_FLAG           ( 1UL << 2 )

/**
 * @brief A flag to indicate whether or not a non-default server
 * certificate has been bound to the socket.
 */
#define securesocketsSOCKET_TRUSTED_SERVER_CERT_FLAG    ( 1UL << 3 )

/**
 * @brief A flag to indicate whether or not the socket is connected.
 *
 */
#define securesocketsSOCKET_IS_CONNECTED                ( 1UL << 4 )

/**
 * @brief Represents a secure socket.
 */
typedef struct SSocketContext_t
{
  esp_netconn_p xSocket;
  uint32_t ulFlags;                   /**< Various properties of the socket (secured etc.). */
  char * pcDestination;               /**< Destination URL. Set using SOCKETS_SO_SERVER_NAME_INDICATION option in SOCKETS_SetSockOpt function. */
  void * pvTLSContext;                /**< The TLS Context. */
  char * pcServerCertificate;         /**< Server certificate. Set using SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE option in SOCKETS_SetSockOpt function. */
  uint32_t ulServerCertificateLength; /**< Length of the server certificate. */
  char ** ppcAlpnProtocols;
  uint32_t ulAlpnProtocolsCount;
  uint8_t ucInUse;                    /**< Tracks whether the socket is in use or not. */
  esp_pbuf_p pbuf;
  BaseType_t available;
  size_t offset;
} SSocketContext_t, * SSocketContextPtr_t;

/**
 * @brief Secure socket objects.
 *
 * An index in this array is returned to the user from SOCKETS_Socket
 * function.
 */
static SSocketContext_t xSockets[ESP_CFG_MAX_CONNS];

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * SSocketContext.ucInUse flag at one time.
 */
static SemaphoreHandle_t xUcInUse = NULL;
static SemaphoreHandle_t xTLSConnect = NULL;
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 60000UL );

static uint32_t prvGetFreeSocket( void )
{
  uint32_t ulSocketNumber;

  /* Obtain the socketInUse mutex. */
  if (xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime) == pdTRUE)
  {
    /* Iterate over xSockets array to see if any free socket
     * is available. */
    for (ulSocketNumber = 0 ; ulSocketNumber < ( uint32_t ) ESP_CFG_MAX_CONNS ; ulSocketNumber++)
    {
      if (xSockets[ ulSocketNumber ].ucInUse == 0U)
      {
        /* Mark the socket as "in-use". */
        xSockets[ ulSocketNumber ].ucInUse = 1;

        /* We have found a free socket, so stop. */
        break;
      }
    }

    /* Give back the socketInUse mutex. */
    xSemaphoreGive(xUcInUse);
  }
  else
  {
	ulSocketNumber = ESP_CFG_MAX_CONNS;
  }

  /* Did we find a free socket? */
  if (ulSocketNumber == (uint32_t) ESP_CFG_MAX_CONNS)
  {
    /* Return SOCKETS_INVALID_SOCKET if we fail to
     * find a free socket. */
	ulSocketNumber = (uint32_t) SOCKETS_INVALID_SOCKET;
  }

  return ulSocketNumber;
}
/*-----------------------------------------------------------*/

static BaseType_t prvReturnSocket( SSocketContextPtr_t pvContext )
{
  BaseType_t xResult = pdFAIL;

  /* Since multiple tasks can be accessing this simultaneously,
   * this has to be in critical section. */
  /* Obtain the socketInUse mutex. */
  if (xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime) == pdTRUE)
  {
    /* Mark the socket as free. */
    pvContext->ucInUse = 0;

    xResult = pdTRUE;

    /* Give back the socketInUse mutex. */
    xSemaphoreGive(xUcInUse);
  }

  return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvIsValidSocket( uint32_t ulSocketNumber )
{
  BaseType_t xValid = pdFALSE;

  /* Check that the provided socket number is within the valid index range. */
  if (ulSocketNumber < ( uint32_t ) ESP_CFG_MAX_CONNS )
  {
	/* Obtain the socketInUse mutex. */
	if( xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime ) == pdTRUE )
	{
      /* Check that this socket is in use. */
      if (xSockets[ ulSocketNumber ].ucInUse == 1U )
      {
        /* This is a valid socket number. */
        xValid = pdTRUE;
      }

      /* Give back the socketInUse mutex. */
      xSemaphoreGive(xUcInUse);
	}
  }

  return xValid;
}
/*-----------------------------------------------------------*/

static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
  BaseType_t lRetVal = 0;
  SSocketContextPtr_t pxContext = (SSocketContextPtr_t) pvContext;

  if ((pucData != NULL) && (pvContext != NULL))
  {
    espr_t res = esp_netconn_write(pxContext->xSocket, pucData, xDataLength);

    if (res == espOK)
    {
      res = esp_netconn_flush(pxContext->xSocket);    /* Flush data to output */
      if (res == espOK)					            /* Were data sent? */
      {
        lRetVal = xDataLength;
      }
    }
  }

  return lRetVal;
}

/*-----------------------------------------------------------*/

static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveLength )
{

  BaseType_t lRetVal = 0;
  SSocketContextPtr_t pxContext = (SSocketContextPtr_t) pvContext;

  if ((pucReceiveBuffer != NULL) && (pvContext != NULL))
  {
  	if (pxContext->available == 0)
    {
      espr_t res = esp_netconn_receive(pxContext->xSocket, &(pxContext->pbuf));
      if ((res == espCLOSED) || (res == espTIMEOUT))
      {
     	if (pxContext->pbuf != NULL)
       	{
          esp_pbuf_free(pxContext->pbuf);    /* Free the memory after usage */
          pxContext->pbuf = NULL;
      	}
      }
      else if ((res == espOK) && (pxContext->pbuf != NULL))
      {
    	pxContext->available = esp_pbuf_length(pxContext->pbuf, 1);
    	pxContext->offset = 0;
      }
      else
      {
    	//
      }
    }

  	if (pxContext->available > 0)
    {
  	  lRetVal = esp_pbuf_copy(pxContext->pbuf, pucReceiveBuffer, xReceiveLength, pxContext->offset);
  	  pxContext->offset += lRetVal;
  	  pxContext->available -= lRetVal;
      if (pxContext->available == 0)
      {
        esp_pbuf_free(pxContext->pbuf);    /* Free the memory after usage */
        pxContext->pbuf = NULL;
      }
    }
  }

  return lRetVal;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
  uint32_t ulSocketNumber;

  /* Ensure that only supported values are supplied. */
  configASSERT( lDomain == SOCKETS_AF_INET );
  configASSERT( lType == SOCKETS_SOCK_STREAM );
  configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

  ulSocketNumber = prvGetFreeSocket();

  /* If we get a free socket, set its attributes. */
  if(ulSocketNumber != (uint32_t)SOCKETS_INVALID_SOCKET)
  {
	SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

	pxContext->ulFlags = 0;
	pxContext->pcDestination = NULL;
	pxContext->pvTLSContext = NULL;
	pxContext->pcServerCertificate = NULL;
	pxContext->ulServerCertificateLength = 0;
	pxContext->ppcAlpnProtocols = NULL;
	pxContext->ulAlpnProtocolsCount = 0;
	pxContext->pbuf = NULL;
	pxContext->available = 0;
	pxContext->offset = 0;

	pxContext->xSocket = esp_netconn_new(ESP_NETCONN_TYPE_TCP);

	if (pxContext->xSocket != NULL)
    {
      esp_netconn_set_receive_timeout(pxContext->xSocket, 0);
    }
    else
    {
      return SOCKETS_INVALID_SOCKET;
    }
  }

  /* If we fail to get a free socket, we return SOCKETS_INVALID_SOCKET. */
  return (Socket_t)ulSocketNumber;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Accept( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t * pxAddressLength )
{
  return SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
  uint32_t ulSocketNumber = ( uint32_t ) xSocket;
  int32_t lRetVal = SOCKETS_SOCKET_ERROR;

  (void)xAddressLength;

  if ((prvIsValidSocket( ulSocketNumber ) == pdTRUE) && (pxAddress != NULL) && (pxAddress->usPort != 0))
  {
    char host[20];
	SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

    /* Check that the socket is not already connected. */
    if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) == 0UL)
    {
      SOCKETS_inet_ntoa(pxAddress->ulAddress, host);

      if (esp_sta_has_ip())
      {
    	espr_t conn_status = esp_netconn_connect((esp_netconn_p)(pxContext->xSocket), host, SOCKETS_ntohs(pxAddress->usPort));
        if (conn_status == espOK)
        {
          pxContext->ulFlags |= securesocketsSOCKET_IS_CONNECTED;
          lRetVal = SOCKETS_ERROR_NONE;
        }
      }

      /* Negotiate TLS if requested. */
      if ((lRetVal == SOCKETS_ERROR_NONE) && ((pxContext->ulFlags & securesocketsSOCKET_SECURE_FLAG) != 0))
      {
        TLSParams_t xTLSParams = { 0 };
        xTLSParams.ulSize = sizeof( xTLSParams );
        xTLSParams.pcDestination = pxContext->pcDestination;
        xTLSParams.pcServerCertificate = pxContext->pcServerCertificate;
        xTLSParams.ulServerCertificateLength = pxContext->ulServerCertificateLength;
        xTLSParams.ppcAlpnProtocols = ( const char ** ) pxContext->ppcAlpnProtocols;
        xTLSParams.ulAlpnProtocolsCount = pxContext->ulAlpnProtocolsCount;
        xTLSParams.pvCallerContext = pxContext;
        xTLSParams.pxNetworkRecv = prvNetworkRecv;
        xTLSParams.pxNetworkSend = prvNetworkSend;

        /* Initialize TLS. */
        if (TLS_Init(&(pxContext->pvTLSContext), &(xTLSParams)) == pdFREERTOS_ERRNO_NONE)
        {
          if (xSemaphoreTake(xTLSConnect, xMaxSemaphoreBlockTime) == pdTRUE)
          {
            /* Initiate TLS handshake. */
            if (TLS_Connect(pxContext->pvTLSContext) != pdFREERTOS_ERRNO_NONE)
            {
              /* TLS handshake failed. */
              esp_netconn_close(pxContext->xSocket);
              pxContext->ulFlags &= ~securesocketsSOCKET_IS_CONNECTED;
              TLS_Cleanup(pxContext->pvTLSContext);
              pxContext->pvTLSContext = NULL;
              lRetVal = SOCKETS_TLS_HANDSHAKE_ERROR;
            }
            /* Release semaphore */
            xSemaphoreGive(xTLSConnect);
          }
        }
        else
        {
          /* TLS Initialization failed. */
          lRetVal = SOCKETS_TLS_INIT_ERROR;
        }
      }
    }
    else
    {
      lRetVal = SOCKETS_EISCONN;
    }
  }
  else
  {
    lRetVal = SOCKETS_EINVAL;
  }

  return lRetVal;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
  int32_t lReceivedBytes = 0;
  uint32_t ulSocketNumber = (uint32_t)xSocket;

  /* Remove warning about unused parameters. */
  (void)ulFlags;

  /* Ensure that a valid socket was passed and the
   * passed buffer is not NULL. */
  if ((prvIsValidSocket(ulSocketNumber) == pdTRUE) && (pvBuffer != NULL))
  {
	SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

	if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) != 0UL)
	{
      /* Check that receive is allowed on the socket. */
      if ((pxContext->ulFlags & securesocketsSOCKET_READ_CLOSED_FLAG ) == 0UL)
      {
        if ((pxContext->ulFlags & securesocketsSOCKET_SECURE_FLAG) != 0UL)
        {
          /* Receive through TLS pipe, if negotiated. */
          lReceivedBytes = TLS_Recv(pxContext->pvTLSContext, pvBuffer, xBufferLength);

          /* Convert the error code. */
          if (lReceivedBytes < 0)
          {
            /* TLS_Recv failed. */
            return SOCKETS_TLS_RECV_ERROR;
          }
        }
        else
        {
          /* Receive un-encrypted. */
          lReceivedBytes = prvNetworkRecv(pxContext, pvBuffer, xBufferLength);
        }
      }
      else
      {
        /* The socket has been closed for read. */
        return SOCKETS_ECLOSED;
      }
	}
    else
    {
      /* The socket has been closed for read. */
      return SOCKETS_ECLOSED;
    }
  }
  else
  {
    return SOCKETS_EINVAL;
  }

  return lReceivedBytes;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
  int32_t lSentBytes = 0;
  uint32_t ulSocketNumber = (uint32_t)xSocket;

  /* Remove warning about unused parameters. */
  (void)ulFlags;

  if ((prvIsValidSocket(ulSocketNumber) == pdTRUE) && (pvBuffer != NULL))
  {
	SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

	if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) != 0UL)
	{
      /* Check that send is allowed on the socket. */
      if ((pxContext->ulFlags & securesocketsSOCKET_WRITE_CLOSED_FLAG ) == 0UL)
      {
        if ((pxContext->ulFlags & securesocketsSOCKET_SECURE_FLAG) != 0UL)
        {
          /* Send through TLS pipe, if negotiated. */
          lSentBytes = TLS_Send(pxContext->pvTLSContext, pvBuffer, xDataLength);

          /* Convert the error code. */
          if (lSentBytes < 0)
          {
            /* TLS_Recv failed. */
            return SOCKETS_TLS_SEND_ERROR;
          }
        }
        else
        {
          /* Send unencrypted. */
          lSentBytes = prvNetworkSend(pxContext, pvBuffer, xDataLength);
        }
      }
      else
      {
        /* The socket has been closed for write. */
        return SOCKETS_ECLOSED;
      }
	}
    else
    {
      /* The supplied socket is closed. */
      return SOCKETS_ECLOSED;
    }
  }
  else
  {
    return SOCKETS_EINVAL;
  }

  return lSentBytes;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
  uint32_t ulSocketNumber = ( uint32_t ) xSocket;

  /* Ensure that a valid socket was passed. */
  if (prvIsValidSocket(ulSocketNumber) == pdTRUE)
  {
    SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

    switch( ulHow )
    {
      case SOCKETS_SHUT_RD:
        /* Further receive calls on this socket should return error. */
    	pxContext->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
        break;

      case SOCKETS_SHUT_WR:
        /* Further send calls on this socket should return error. */
    	pxContext->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;
        break;

      case SOCKETS_SHUT_RDWR:
        /* Further send or receive calls on this socket should return error. */
    	pxContext->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG | securesocketsSOCKET_WRITE_CLOSED_FLAG;
        break;

      default:
        /* An invalid value was passed for ulHow. */
        return SOCKETS_EINVAL;
        break;
    }
  }
  else
  {
    return SOCKETS_EINVAL;
  }

  return SOCKETS_ERROR_NONE;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Close( Socket_t xSocket )
{
  int32_t lRetVal = SOCKETS_SOCKET_ERROR;
  uint32_t ulSocketNumber = (uint32_t)xSocket;

  /* Ensure that a valid socket was passed. */
  if (prvIsValidSocket(ulSocketNumber) == pdTRUE)
  {
	SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

	pxContext->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
	pxContext->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;

    /* Clean-up destination string. */
    if (pxContext->pcDestination != NULL)
    {
      vPortFree(pxContext->pcDestination);
      pxContext->pcDestination = NULL;
    }

    /* Clean-up server certificate. */
    if (pxContext->pcServerCertificate != NULL)
    {
      vPortFree(pxContext->pcServerCertificate);
      pxContext->pcServerCertificate = NULL;
    }

    /* Clean-up application protocol array. */
    if (pxContext->ppcAlpnProtocols != NULL)
    {
      for (uint32_t ulProtocol = 0; ulProtocol < pxContext->ulAlpnProtocolsCount; ulProtocol++)
      {
        if (pxContext->ppcAlpnProtocols[ ulProtocol ] != NULL)
        {
          vPortFree( pxContext->ppcAlpnProtocols[ ulProtocol ] );
          pxContext->ppcAlpnProtocols[ ulProtocol ] = NULL;
        }
      }

      vPortFree(pxContext->ppcAlpnProtocols);
      pxContext->ppcAlpnProtocols = NULL;
    }

    /* Clean-up TLS context. */
    if ((pxContext->ulFlags & securesocketsSOCKET_SECURE_FLAG) != 0UL)
    {
      TLS_Cleanup(pxContext->pvTLSContext);
      pxContext->pvTLSContext = NULL;
    }

 	if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) != 0UL)
	{
      if (esp_netconn_close(pxContext->xSocket) == espOK)
      {
        pxContext->ulFlags &= ~securesocketsSOCKET_IS_CONNECTED;
        lRetVal = SOCKETS_ERROR_NONE;
      }
    }
 	else
 	{
      lRetVal = SOCKETS_ERROR_NONE;
 	}

    esp_netconn_delete(pxContext->xSocket);

    /* Return the socket back to the free socket pool. */
    if (prvReturnSocket(pxContext) != pdTRUE)
    {
      lRetVal = SOCKETS_SOCKET_ERROR;
    }

  }
  else
  {
	lRetVal = SOCKETS_EINVAL;
  }

  return lRetVal;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
  int32_t lRetCode = SOCKETS_ERROR_NONE;
  uint32_t ulSocketNumber = ( uint32_t ) xSocket;
  TickType_t xTimeout;

  /* Remove compiler warnings about unused parameters. */
  (void)lLevel;

  /* Ensure that a valid socket was passed. */
  if (prvIsValidSocket(ulSocketNumber) == pdTRUE)
  {
	 SSocketContextPtr_t pxContext = &xSockets[ulSocketNumber];

	 switch( lOptionName )
     {
       case SOCKETS_SO_SERVER_NAME_INDICATION:
         if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) == 0)
         {
           /* Non-NULL destination string indicates that SNI extension should
            * be used during TLS negotiation. */
           pxContext->pcDestination = ( char * ) pvPortMalloc( 1U + xOptionLength );

           if (pxContext->pcDestination == NULL)
           {
        	 lRetCode = SOCKETS_ENOMEM;
           }
           else
           {
             if (pvOptionValue != NULL)
             {
               memcpy(pxContext->pcDestination, pvOptionValue, xOptionLength );
               pxContext->pcDestination[ xOptionLength ] = '\0';
             }
             else
             {
               lRetCode = SOCKETS_EINVAL;
             }
           }
         }
         else
         {
           lRetCode = SOCKETS_EISCONN;
         }

         break;

      case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:
        if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) == 0)
        {
          /* Non-NULL server certificate field indicates that the default trust
           * list should not be used. */
          pxContext->pcServerCertificate = ( char * ) pvPortMalloc( xOptionLength );

          if (pxContext->pcServerCertificate == NULL)
          {
        	lRetCode = SOCKETS_ENOMEM;
          }
          else
          {
            if (pvOptionValue != NULL)
            {
              memcpy( pxContext->pcServerCertificate, pvOptionValue, xOptionLength );
              pxContext->ulServerCertificateLength = xOptionLength;
            }
            else
            {
              lRetCode = SOCKETS_EINVAL;
            }
          }
        }
        else
        {
          lRetCode = SOCKETS_EISCONN;
        }

        break;

      case SOCKETS_SO_REQUIRE_TLS:
        if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) == 0)
        {
          pxContext->ulFlags |= securesocketsSOCKET_SECURE_FLAG;
        }
        else
        {
          /* Do not set the ALPN option if the socket is already connected. */
          lRetCode = SOCKETS_EISCONN;
        }
        break;

      case SOCKETS_SO_ALPN_PROTOCOLS:
        if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED) == 0)
        {
          /* Allocate a sufficiently long array of pointers. */
          pxContext->ulAlpnProtocolsCount = 1 + xOptionLength;

          if (NULL == (pxContext->ppcAlpnProtocols = (char **)pvPortMalloc(pxContext->ulAlpnProtocolsCount * sizeof(char *))))
          {
        	lRetCode = SOCKETS_ENOMEM;
          }
          else
          {
            pxContext->ppcAlpnProtocols[pxContext->ulAlpnProtocolsCount - 1 ] = NULL;
          }

          /* Copy each protocol string. */
          for (uint32_t ulProtocol = 0; (ulProtocol < pxContext->ulAlpnProtocolsCount - 1) && (lRetCode == pdFREERTOS_ERRNO_NONE); ulProtocol++)
          {
        	char ** ppcAlpnIn = ( char ** ) pvOptionValue;
        	size_t xLength = strlen(ppcAlpnIn[ ulProtocol ]);

            if ((pxContext->ppcAlpnProtocols[ ulProtocol ] = (char *)pvPortMalloc(1 + xLength)) == NULL)
            {
              lRetCode = SOCKETS_ENOMEM;
            }
            else
            {
              memcpy( pxContext->ppcAlpnProtocols[ ulProtocol ], ppcAlpnIn[ ulProtocol ], xLength );
              pxContext->ppcAlpnProtocols[ ulProtocol ][ xLength ] = '\0';
            }
          }
        }
        else
        {
          /* Do not set the ALPN option if the socket is already connected. */
          lRetCode = SOCKETS_EISCONN;
        }

        break;

      case SOCKETS_SO_SNDTIMEO:
        break;

      case SOCKETS_SO_RCVTIMEO:
        if (pvOptionValue != NULL)
        {
          xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t. */
          esp_netconn_set_receive_timeout(pxContext->xSocket, xTimeout);
        }
        else
        {
          lRetCode = SOCKETS_EINVAL;
        }

        break;

      case SOCKETS_SO_NONBLOCK:
    	if ((pxContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) != 0)
    	{
    	  /* Set the timeouts to the smallest value possible.
           * This isn't true nonblocking, but as close as we can get. */
          esp_netconn_set_receive_timeout(pxContext->xSocket, 1);
    	}
    	else
    	{
          lRetCode = SOCKETS_EISCONN;
    	}
        break;

      default:
    	lRetCode = SOCKETS_EINVAL;
    }
  }
  else
  {
	lRetCode = SOCKETS_EINVAL;
  }

  return lRetCode;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName(const char * pcHostName)
{
  if (pcHostName != NULL)
  {
    uint32_t ulIPAddres;
    if (WIFI_GetHostIP((char * )pcHostName, ( uint8_t *)&(ulIPAddres)) == eWiFiSuccess)
    {
      return ulIPAddres;
    }
  }

  return 0;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
  uint32_t ulIndex;

  /* Mark all the sockets as free */
  for (ulIndex = 0 ; ulIndex < (uint32_t)ESP_CFG_MAX_CONNS ; ulIndex++)
  {
     xSockets[ ulIndex ].ucInUse = 0;
  }

  /* Create the global mutex which is used to ensure
   * that only one socket is accessing the ucInUse bits in
   * the socket array. */
  xUcInUse = xSemaphoreCreateMutex();
  if (xUcInUse == NULL)
  {
	return pdFAIL;
  }

  /* Create the global mutex which is used to ensure
   * that only one socket is accessing the ucInUse bits in
   * the socket array. */
  xTLSConnect = xSemaphoreCreateMutex();
  if (xTLSConnect == NULL)
  {
	return pdFAIL;
  }

  return pdPASS;
}
/*-----------------------------------------------------------*/

static CK_SESSION_HANDLE xPkcs11Session = 0;
static CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;

uint32_t ulRand( void )
{
    CK_RV xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    CK_ULONG ulCount = 1;
    uint32_t ulRandomValue = 0;
    CK_SLOT_ID xSlotId = 0;

    portENTER_CRITICAL();

    if( 0 == xPkcs11Session )
    {
        /* One-time initialization. */

        /* Ensure that the PKCS#11 module is initialized. */
        if( 0 == xResult )
        {
            pxCkGetFunctionList = C_GetFunctionList;
            xResult = pxCkGetFunctionList( &pxPkcs11FunctionList );
        }

        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_Initialize( NULL );
        }

        /* Get the default slot ID. */
        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_GetSlotList(
                CK_TRUE,
                &xSlotId,
                &ulCount );
        }

        /* Start a session with the PKCS#11 module. */
        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_OpenSession(
                xSlotId,
                CKF_SERIAL_SESSION,
                NULL,
                NULL,
                &xPkcs11Session );
        }
    }

    if( 0 == xResult )
    {
        /* Request a sequence of cryptographically random byte values using
         * PKCS#11. */
        xResult = pxPkcs11FunctionList->C_GenerateRandom( xPkcs11Session,
                                                          ( CK_BYTE_PTR ) &ulRandomValue,
                                                          sizeof( ulRandomValue ) );
    }

    portEXIT_CRITICAL();

    /* Check if any of the API calls failed. */
    if( 0 != xResult )
    {
        ulRandomValue = 0;
    }

    return ulRandomValue;
}
/*-----------------------------------------------------------*/
