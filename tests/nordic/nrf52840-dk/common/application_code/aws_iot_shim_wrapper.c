/* Platform layer includes. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include "aws_ble_config.h"
#include "aws_iot_mqtt_ble.h"
#include "platform/aws_iot_network.h"


AwsIotNetworkError_t AwsIotNetwork_Init( void )
{

}

void AwsIotNetwork_Cleanup( void )
{
  
}

AwsIotNetworkError_t AwsIotNetwork_CreateConnection( AwsIotNetworkConnection_t * const pNetworkConnection,
                                                     const char * const pHostName,
                                                     uint16_t port,
                                                     const AwsIotNetworkTlsInfo_t * const pTlsInfo )
{

}

void AwsIotNetwork_CloseConnection( AwsIotNetworkConnection_t networkConnection )
{

}

void AwsIotNetwork_DestroyConnection( AwsIotNetworkConnection_t networkConnection )
{

}

AwsIotNetworkError_t AwsIotNetwork_SetMqttReceiveCallback( AwsIotNetworkConnection_t networkConnection,
                                                           AwsIotMqttConnection_t * pMqttConnection,
                                                           AwsIotMqttReceiveCallback_t receiveCallback )
{

}


size_t AwsIotNetwork_Send( void * networkConnection,
                           const void * const pMessage,
                           size_t messageLength )
{

}
