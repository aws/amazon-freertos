#ifndef _AWS_IOT_DEFENDER_INTERNAL_H_
#define _AWS_IOT_DEFENDER_INTERNAL_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Defender include. */
#include "aws_iot_defender.h"

/* constants */
#define AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR    1

#define AWS_IOT_DEFENDER_METRICS_FORMAT_JSON    2

/* default configuration */
#ifndef AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
    #define AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS    ( 300 )
#endif

#ifndef AWS_IOT_DEFENDER_DEFAULT_MQTT_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_DEFAULT_MQTT_TIMEOUT_SECONDS    ( 10U )
#endif

#ifndef AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE
    #define AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE    AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR
#endif

#ifndef AWS_IOT_DEFENDER_USE_LONG_TAG
    #define AWS_IOT_DEFENDER_USE_LONG_TAG    ( 0 )
#endif

/* below is internal only */


/**
 * select long tag or short tag based on configuration AWS_IOT_DEFENDER_USE_LONG_TAG
 *
 */
#if  AWS_IOT_DEFENDER_USE_LONG_TAG == 1
    #define AwsIotDefenderInternal_SelectTag( long_tag, short_tag )    ( long_tag )
#else
    #define AwsIotDefenderInternal_SelectTag( long_tag, short_tag )    ( short_tag )
#endif

/**
 * define encoder/decoder based on configuration AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE
 *
 */
#if AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE == AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR

    #define _METRICS_FORMAT    "cbor"
    #define _Encoder           _AwsIotSerializerCborEncoder
    #define _Decoder           _AwsIotSerializerCborDecoder

#elif AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE == AWS_IOT_DEFENDER_METRICS_FORMAT_JSON

    #define _METRICS_FORMAT    "json"
    #define _Encoder           _AwsIotSerializerJsonEncoder
    #define _Decoder           _AwsIotSerializerJsonDecoder

#else /* if AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE == AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR */
    #error "AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE must be either AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR or AWS_IOT_DEFENDER_METRICS_FORMAT_JSON."

#endif /* if AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE == AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR */

/* define topics used by definder */
#define _TOPIC_PUBLISH     "$aws/things/"AWS_IOT_DEFENDER_THING_NAME "/defender/metrics/" _METRICS_FORMAT

#define _TOPIC_ACCEPTED    _TOPIC_PUBLISH "/accepted"

#define _TOPIC_REJECTED    _TOPIC_PUBLISH "/rejected"

/* common tags in metrics report */
#define  _HEADER_TAG       AwsIotDefenderInternal_SelectTag( "header", "hed" )
#define  _REPORTID_TAG     AwsIotDefenderInternal_SelectTag( "report_id", "rid" )
#define  _VERSION_TAG      AwsIotDefenderInternal_SelectTag( "version", "v" )
#define  _METRICS_TAG      AwsIotDefenderInternal_SelectTag( "metrics", "met" )

#endif /* ifndef _AWS_IOT_DEFENDER_INTERNAL_H_ */
