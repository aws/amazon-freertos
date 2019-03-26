/* This example demonstrates connection with AWS IoT through HTTPS protocol. 
 *
 * AWS IoT: Publish to a topic through HTTPS/POST method:
 * http://docs.aws.amazon.com/iot/latest/developerguide/protocols.html
 *
 * AWS IoT: Thing Shadow RESTful API:
 * http://docs.aws.amazon.com/iot/latest/developerguide/thing-shadow-rest-api.html
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* TCP/IP abstraction includes. */
#include "aws_secure_sockets.h"

/* Demo configuration */
#include "aws_demo_config.h"

/* Update this file with AWS Credentials. */
#include "aws_clientcredential.h"

/* The trusted root certificate of HTTP-Server */
static const char tcptestECHO_HOST_ROOT_CA[] = "Paste root certificate here";

/* Dimensions the buffer used to generate the task name. */
#define echoMAX_TASK_NAME_LENGTH    8

/* Primary socket.  Socket declared globally so that it can be closed in test tear down
 * in the event that a test exits/fails before socket is closed. */
volatile Socket_t xSocket;
volatile BaseType_t xSocketOpen;

/* TCP Echo Client tasks multi-task test parameters. These can be configured in aws_test_tcp_config.h. */
#ifndef democonfigHTTPS_DEMO_TASK_STACK_SIZE
    #define democonfigHTTPS_DEMO_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 10 )
#endif
#ifndef democonfigHTTPS_DEMO_TASK_PRIORITY
    #define democonfigHTTPS_DEMO_TASK_PRIORITY      ( tskIDLE_PRIORITY )
#endif

/* For HTTPS test */

/**
 * @brief Boolean Type definition
 */
typedef enum {false = 0, true = !false} bool;

#define AWS_IOT_HTTPS_SERVER_NAME               "a1fljoeglhtf61.iot.us-east-2.amazonaws.com"
#define AWS_IOT_HTTPS_SERVER_PORT               8443

#define AWS_IOT_HTTPS_THINGNAME                 "Nuvoton-Mbed-D001"

/* Publish to user topic through HTTPS/POST 
 * HTTP POST https://"endpoint"/topics/"yourTopicHierarchy" */
const char USER_TOPIC_HTTPS_PATH[] = "/topics/Nuvoton/Mbed/D001?qos=1";
const char USER_TOPIC_HTTPS_REQUEST_METHOD[] = "POST";
const char USER_TOPIC_HTTPS_REQUEST_MESSAGE_BODY[] = "{ \"message\": \"Hello from Nuvoton Mbed device\" }";

/* Update thing shadow by publishing to UpdateThingShadow topic through HTTPS/POST
 * HTTP POST https://"endpoint"/topics/$aws/things/"thingName"/shadow/update */
const char UPDATETHINGSHADOW_TOPIC_HTTPS_PATH[] = "/topics/$aws/things/" AWS_IOT_HTTPS_THINGNAME "/shadow/update?qos=1";
const char UPDATETHINGSHADOW_TOPIC_HTTPS_REQUEST_METHOD[] = "POST";
const char UPDATETHINGSHADOW_TOPIC_HTTPS_REQUEST_MESSAGE_BODY[] = "{ \"state\": { \"reported\": { \"attribute1\": 3, \"attribute2\": \"1\" } } }";

/* Get thing shadow by publishing to GetThingShadow topic through HTTPS/POST
 * HTTP POST https://"endpoint"/topics/$aws/things/"thingName"/shadow/get */
const char GETTHINGSHADOW_TOPIC_HTTPS_PATH[] = "/topics/$aws/things/" AWS_IOT_HTTPS_THINGNAME "/shadow/get?qos=1";
const char GETTHINGSHADOW_TOPIC_HTTPS_REQUEST_METHOD[] = "POST";
const char GETTHINGSHADOW_TOPIC_HTTPS_REQUEST_MESSAGE_BODY[] = "";

/* Delete thing shadow by publishing to DeleteThingShadow topic through HTTPS/POST
 * HTTP POST https://"endpoint"/topics/$aws/things/"thingName"/shadow/delete */
const char DELETETHINGSHADOW_TOPIC_HTTPS_PATH[] = "/topics/$aws/things/" AWS_IOT_HTTPS_THINGNAME "/shadow/delete?qos=1";
const char DELETETHINGSHADOW_TOPIC_HTTPS_REQUEST_METHOD[] = "POST";
const char DELETETHINGSHADOW_TOPIC_HTTPS_REQUEST_MESSAGE_BODY[] = "";

/* Update thing shadow RESTfully through HTTPS/POST
 * HTTP POST https://endpoint/things/thingName/shadow */
const char UPDATETHINGSHADOW_THING_HTTPS_PATH[] = "/things/" AWS_IOT_HTTPS_THINGNAME "/shadow";
const char UPDATETHINGSHADOW_THING_HTTPS_REQUEST_METHOD[] = "POST";
const char UPDATETHINGSHADOW_THING_HTTPS_REQUEST_MESSAGE_BODY[] = "{ \"state\": { \"desired\": { \"attribute1\": 1, \"attribute2\": \"2\" }, \"reported\": { \"attribute1\": 2, \"attribute2\": \"1\" } } }";

/* Get thing shadow RESTfully through HTTPS/GET
 * HTTP GET https://"endpoint"/things/"thingName"/shadow */
const char GETTHINGSHADOW_THING_HTTPS_PATH[] = "/things/" AWS_IOT_HTTPS_THINGNAME "/shadow";
const char GETTHINGSHADOW_THING_HTTPS_REQUEST_METHOD[] = "GET";
const char GETTHINGSHADOW_THING_HTTPS_REQUEST_MESSAGE_BODY[] = "";

/* Delete thing shadow RESTfully through HTTPS/DELETE
 * HTTP DELETE https://endpoint/things/thingName/shadow */
const char DELETETHINGSHADOW_THING_HTTPS_PATH[] = "/things/" AWS_IOT_HTTPS_THINGNAME "/shadow";
const char DELETETHINGSHADOW_THING_HTTPS_REQUEST_METHOD[] = "DELETE";
const char DELETETHINGSHADOW_THING_HTTPS_REQUEST_MESSAGE_BODY[] = "";

/* HTTPS user buffer size */
const int HTTPS_USER_BUFFER_SIZE = 600;

const char *HTTPS_OK_STR = "200 OK";

/*-----------------------------------------------------------*/

bool run_req_resp(const char *https_path, const char *https_request_method, const char *https_request_message_body) {
    
    char _buffer[HTTPS_USER_BUFFER_SIZE];   /**< User buffer */
    bool ret = false;
    
    do {
        int tls_rc;
        bool _got200 = false;

        int _bpos;

        /* Fill the request buffer */
        _bpos = snprintf(_buffer, sizeof(_buffer) - 1,
                        "%s %s HTTP/1.1\r\n" "Host: %s\r\n" "Content-Length: %d\r\n" "\r\n" "%s",
                        https_request_method, https_path, AWS_IOT_HTTPS_SERVER_NAME, strlen(https_request_message_body), https_request_message_body);
        if (_bpos < 0 || ((size_t) _bpos) > (sizeof (_buffer) - 1)) {
            printf("snprintf failed: %d\n", _bpos);
            break;
        }
        _buffer[_bpos] = 0;
        /* Print request message */
        printf("HTTPS: Request message:\n");
        printf("%s\n", _buffer);
    
        int offset = 0;
        do {
            //tls_rc = _tlssocket->send((const unsigned char *) _buffer + offset, _bpos - offset);
            tls_rc = SOCKETS_Send(xSocket, (const unsigned char *) _buffer + offset, _bpos - offset, 0);
            //TEST_ASSERT_LESS_THAN_UINT32( 0, tls_rc );
            if (tls_rc > 0) {
                offset += tls_rc;
            }
        } while ((offset < _bpos) && (tls_rc > 0));
                //(tls_rc > 0 || tls_rc == MBEDTLS_ERR_SSL_WANT_READ || tls_rc == MBEDTLS_ERR_SSL_WANT_WRITE));
        if (tls_rc < 0) {
            printf("_tlssocket->send=%d\n", tls_rc);
            break;
        }

        /* Read data out of the socket */
        offset = 0;
        size_t content_length = 0;
        size_t offset_end = 0;
        char *line_beg = _buffer;
        char *line_end = NULL;
        do {
            //tls_rc = _tlssocket->recv((unsigned char *) _buffer + offset, sizeof(_buffer) - offset - 1);
            tls_rc = SOCKETS_Recv(xSocket, (unsigned char *) _buffer + offset, sizeof(_buffer) - offset - 1, 0);
            //TEST_ASSERT_LESS_THAN_UINT32( 0, tls_rc );
            if (tls_rc > 0) {
                offset += tls_rc;
            }
            
            /* Make it null-terminated */
            _buffer[offset] = 0;

            /* Scan response message
             *             
             * 1. A status line which includes the status code and reason message (e.g., HTTP/1.1 200 OK)
             * 2. Response header fields (e.g., Content-Type: text/html)
             * 3. An empty line (\r\n)
             * 4. An optional message body
             */
            if (! offset_end) {
                line_end = strstr(line_beg, "\r\n");
                if (line_end) {
                    /* Scan status line */
                    if (! _got200) {
                        _got200 = strstr(line_beg, HTTPS_OK_STR) != NULL;
                    }
        
                    /* Scan response header fields for Content-Length 
                     * 
                     * NOTE: Assume chunked transfer (Transfer-Encoding: chunked) is not used
                     * NOTE: Assume response field name are in lower case
                     */
                    if (content_length == 0) {
                        sscanf(line_beg, "content-length:%d", &content_length);
                    }
                
                    /* An empty line indicates end of response header fields */
                    if (line_beg == line_end) {
                        offset_end = line_end - _buffer + 2 + content_length;
                    }
                
                    /* Go to next line */
                    line_beg = line_end + 2;
                    line_end = NULL;
                }
            }
        } while ((offset_end == 0 || offset < offset_end) && (tls_rc > 0));
                //(tls_rc > 0 || tls_rc == MBEDTLS_ERR_SSL_WANT_READ || tls_rc == MBEDTLS_ERR_SSL_WANT_WRITE));
        if (tls_rc < 0) {
            //tls_rc != MBEDTLS_ERR_SSL_WANT_READ && 
            //tls_rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf("_tlssocket->read=%d\n", tls_rc);
            break;
        }
        _bpos = offset;

        _buffer[_bpos] = 0;

        /* Print status messages */
        printf("HTTPS: Received %d chars from server\n", _bpos);
        printf("HTTPS: Received 200 OK status ... %s\n", _got200 ? "[OK]" : "[FAIL]");
        printf("HTTPS: Received message:\n");
        printf("%s\n", _buffer);
    
        ret = true;
        
    } while (0);
    
    return ret;
}

void start_test() {
    
    int tls_rc;
     
    do {
        /* Publish to user topic through HTTPS/POST */
        printf("Publishing to user topic through HTTPS/POST\n");
        if (! run_req_resp(USER_TOPIC_HTTPS_PATH, USER_TOPIC_HTTPS_REQUEST_METHOD, USER_TOPIC_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Publishes to user topic through HTTPS/POST OK\n\n");
    
        /* Update thing shadow by publishing to UpdateThingShadow topic through HTTPS/POST */
        printf("Updating thing shadow by publishing to Update Thing Shadow topic through HTTPS/POST\n");
        if (! run_req_resp(UPDATETHINGSHADOW_TOPIC_HTTPS_PATH, UPDATETHINGSHADOW_TOPIC_HTTPS_REQUEST_METHOD, UPDATETHINGSHADOW_TOPIC_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Update thing shadow by publishing to Update Thing Shadow topic through HTTPS/POST OK\n\n");
        
        /* Get thing shadow by publishing to GetThingShadow topic through HTTPS/POST */
        printf("Getting thing shadow by publishing to GetThingShadow topic through HTTPS/POST\n");
        if (! run_req_resp(GETTHINGSHADOW_TOPIC_HTTPS_PATH, GETTHINGSHADOW_TOPIC_HTTPS_REQUEST_METHOD, GETTHINGSHADOW_TOPIC_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Get thing shadow by publishing to GetThingShadow topic through HTTPS/POST OK\n\n");
        
        /* Delete thing shadow by publishing to DeleteThingShadow topic through HTTPS/POST */
        printf("Deleting thing shadow by publishing to DeleteThingShadow topic through HTTPS/POST\n");
        if (! run_req_resp(DELETETHINGSHADOW_TOPIC_HTTPS_PATH, DELETETHINGSHADOW_TOPIC_HTTPS_REQUEST_METHOD, DELETETHINGSHADOW_TOPIC_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Delete thing shadow by publishing to DeleteThingShadow topic through HTTPS/POST OK\n\n");
        
        /* Update thing shadow RESTfully through HTTPS/POST */
        printf("Updating thing shadow RESTfully through HTTPS/POST\n");
        if (! run_req_resp(UPDATETHINGSHADOW_THING_HTTPS_PATH, UPDATETHINGSHADOW_THING_HTTPS_REQUEST_METHOD, UPDATETHINGSHADOW_THING_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Update thing shadow RESTfully through HTTPS/POST OK\n\n");
        
        /* Get thing shadow RESTfully through HTTPS/GET */
        printf("Getting thing shadow RESTfully through HTTPS/GET\n");
        if (! run_req_resp(GETTHINGSHADOW_THING_HTTPS_PATH, GETTHINGSHADOW_THING_HTTPS_REQUEST_METHOD, GETTHINGSHADOW_THING_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Get thing shadow RESTfully through HTTPS/GET OK\n\n");
        
        /* Delete thing shadow RESTfully through HTTPS/DELETE */
        printf("Deleting thing shadow RESTfully through HTTPS/DELETE\n");
        if (! run_req_resp(DELETETHINGSHADOW_THING_HTTPS_PATH, DELETETHINGSHADOW_THING_HTTPS_REQUEST_METHOD, DELETETHINGSHADOW_THING_HTTPS_REQUEST_MESSAGE_BODY)) {
            break;
        }
        printf("Delete thing shadow RESTfully through HTTPS/DELETE OK\n\n");
        
    } while (0);
}

static void prvHttpsDemoTask( void * pvParameters )
{
    BaseType_t xResult;
    SocketsSockaddr_t xEchoServerAddress;

    ( void ) pvParameters;

    /* Create a TCP socket. */
    xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );
    configASSERT( xSocket != SOCKETS_INVALID_SOCKET );

    /* Set the socket to use TLS. */
    xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_REQUIRE_TLS, NULL, ( size_t ) 0 );

    if( xResult == SOCKETS_ERROR_NONE )
    {
        /* Set the socket to use the secure server's root CA cert. */
        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE, 
                                      tcptestECHO_HOST_ROOT_CA, sizeof( tcptestECHO_HOST_ROOT_CA ) );

        if( xResult != SOCKETS_ERROR_NONE )
        {
            configPRINTF( ( "%s: Failed to setSockOpt SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE \r\n", __FUNCTION__ ) );
        }
    }
    else
    {
        configPRINTF( ( "%s: Failed to setSockOpt SOCKETS_SO_REQUIRE_TLS \r\n", __FUNCTION__ ) );
    }

    configPRINTF( ( "%s: Connect to server %s:%d\r\n", __FUNCTION__, AWS_IOT_HTTPS_SERVER_NAME, AWS_IOT_HTTPS_SERVER_PORT ) );
    xEchoServerAddress.ulAddress = SOCKETS_GetHostByName( AWS_IOT_HTTPS_SERVER_NAME );
    xEchoServerAddress.usPort = SOCKETS_htons( AWS_IOT_HTTPS_SERVER_PORT );
    xEchoServerAddress.ucLength = sizeof( SocketsSockaddr_t );
    xEchoServerAddress.ucSocketDomain = SOCKETS_AF_INET;

    xResult = SOCKETS_Connect( xSocket, &xEchoServerAddress, sizeof( xEchoServerAddress ) );
    configASSERT( xResult == SOCKETS_ERROR_NONE );

    /* Set time out to non-block mode. */
    xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_NONBLOCK, NULL, 0 );
    configASSERT( xResult == SOCKETS_ERROR_NONE );

    start_test();

    /* Close this socket. */
    xResult = SOCKETS_Close( xSocket );
    configASSERT( xResult == SOCKETS_ERROR_NONE );
}
/*-----------------------------------------------------------*/

void vStartHttpsDemoTasks( void )
{
    /* Create the HTTPS demo tasks. */
    xTaskCreate( prvHttpsDemoTask,                     /* The function that implements the task. */
                 "MainDemoTask",                       /* Just a text name for the task to aid debugging. */
                 democonfigHTTPS_DEMO_TASK_STACK_SIZE, /* The stack size is defined in FreeRTOSIPConfig.h. */
                 NULL,                                 /* The task parameter, not used in this case. */
                 democonfigHTTPS_DEMO_TASK_PRIORITY,   /* The priority assigned to the task is defined in FreeRTOSConfig.h. */
                 NULL );                               /* The task handle is not used. */
}
/*-----------------------------------------------------------*/
