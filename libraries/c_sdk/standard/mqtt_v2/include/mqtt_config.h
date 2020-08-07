#ifndef MQTT_CONFIG_H_
#define MQTT_CONFIG_H_

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Logging related header files are required to be included in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define LIBRARY_LOG_NAME and  LIBRARY_LOG_LEVEL.
 * 3. Include the header file "logging_stack.h".
 */

/* Include header that defines log levels. */
#include "logging_levels.h"

/* Configure name and log level for the MQTT library. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "MQTT"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_ERROR
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/


#include "iot_ble_data_transfer.h"
#include "platform/iot_threads.h"
typedef struct NetworkContext
{
    IotBleDataTransferChannel_t * pChannel;
    IotSemaphore_t isReady;
    void * buf;
    size_t bufSize;
} NetworkContext_t;


/**
 * @brief The maximum number of MQTT PUBLISH messages that may be pending
 * acknowledgement at any time.
 *
 * QoS 1 and 2 MQTT PUBLISHes require acknowlegement from the server before
 * they can be completed. While they are awaiting the acknowledgement, the
 * client must maintain information about their state. The value of this
 * macro sets the limit on how many simultaneous PUBLISH states an MQTT
 * context maintains.
 */
#define MQTT_MAX_QUEUED_PUBLISH_MESSAGES    10

/**
 * @brief The maximum number of MQTT PUBLISH messages that may be pending
 * acknowledgement at any time.
 *
 * QoS 1 and 2 MQTT PUBLISHes require acknowledgement from the server before
 * they can be completed. While they are awaiting the acknowledgement, the
 * client must maintain information about their state. The value of this
 * macro sets the limit on how many simultaneous PUBLISH states an MQTT
 * context maintains.
 */
#define MQTT_STATE_ARRAY_MAX_COUNT          10U

/**
 * @brief MQTT client identifier.
 *
 * No two clients may use the same client identifier simultaneously.
 */
#define MQTT_CLIENT_IDENTIFIER              "testclient"


#endif /* ifndef MQTT_CONFIG_H_ */
