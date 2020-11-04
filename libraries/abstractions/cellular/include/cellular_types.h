/*
 * Amazon FreeRTOS CELLULAR Preview Release
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef __CELLULAR_TYPES_H__
#define __CELLULAR_TYPES_H__

#include <stdbool.h>

/**
 * @brief Invalid signal value.
 */
#define CELLULAR_INVALID_SIGNAL_VALUE        ( -32768 )
#define CELLULAR_INVALID_SIGNAL_BAR_VALUE    ( 0xFFU )

/**
 * @brief Opaque Cellular handle.
 */
struct CellularContext;
typedef struct CellularContext         CellularContext_t;
typedef struct CellularContext         * CellularHandle_t;

/**
 * @brief Opaque socket handle.
 */
struct CellularSocketContext;
typedef struct CellularSocketContext   * CellularSocketHandle_t;

/**
 * @brief Status code returns from APIs.
 */
typedef enum CellularError
{
    CELLULAR_SUCCESS = 0,            /**< The operation was successful. */
    CELLULAR_INVALID_HANDLE,         /**< Invalid handle. */
    CELLULAR_MODEM_NOT_READY,        /**< The modem is not ready yet. */
    CELLULAR_LIBRARY_NOT_OPEN,       /**< The cellular library is not open yet. */
    CELLULAR_LIBRARY_ALREADY_OPEN,   /**< The cellular library is already open. */
    CELLULAR_BAD_PARAMETER,          /**< One or more of the input parameters is not valid. */
    CELLULAR_NO_MEMORY,              /**< Memory allocation failure. */
    CELLULAR_TIMEOUT,                /**< The operation timed out. */
    CELLULAR_SOCKET_CLOSED,          /**< The supplied socket is already closed. */
    CELLULAR_SOCKET_NOT_CONNECTED,   /**< The supplied socket is not connected. */
    CELLULAR_INTERNAL_FAILURE,       /**< The Cellular library internal failure. */
    CELLULAR_RESOURCE_CREATION_FAIL, /**< Resource creation for Cellular library failed. */
    CELLULAR_UNSUPPORTED,            /**< The operation is not supported. */
    CELLULAR_NOT_ALLOWED,            /**< The operation is not allowed. */
    CELLULAR_UNKNOWN                 /**< Any other error other than the above mentioned ones. */
} CellularError_t;

/**
 * @brief Enums representing Radio Access Technologies (RATs). Reference 3GPP TS 27.007 PLMN selection +COPS.
 */
typedef enum CellularRat
{
    CELLULAR_RAT_GSM = 0,
    CELLULAR_RAT_WCDMA = 2,
    CELLULAR_RAT_EDGE = 3,
    CELLULAR_RAT_HSDPA = 4,
    CELLULAR_RAT_HSUPA = 5,
    CELLULAR_RAT_HSDPAHSUPA = 6,
    CELLULAR_RAT_LTE = 7,
    CELLULAR_RAT_CATM1 = 8,
    CELLULAR_RAT_NBIOT = 9,
    CELLULAR_RAT_MAX,
    CELLULAR_RAT_INVALID = 0xFF
} CellularRat_t;

/**
 * @brief SIM card state codes.
 */
typedef enum CellularSimCardState
{
    CELLULAR_SIM_CARD_REMOVED = 0,
    CELLULAR_SIM_CARD_INSERTED,
    CELLULAR_SIM_CARD_STATUS_MAX,
    CELLULAR_SIM_CARD_UNKNOWN
} CellularSimCardState_t;

/**
 * @brief SIM card lock state codes. Reference 3GPP TS 27.007 Enter PIN +CPIN.
 */
typedef enum CellularSimCardLockState
{
    CELLULAR_SIM_CARD_READY = 0,
    CELLULAR_SIM_CARD_PIN,
    CELLULAR_SIM_CARD_PUK,
    CELLULAR_SIM_CARD_PIN2,
    CELLULAR_SIM_CARD_PUK2,
    CELLULAR_SIM_CARD_PH_NET_PIN,
    CELLULAR_SIM_CARD_PH_NET_PUK,
    CELLULAR_SIM_CARD_PH_NETSUB_PIN,
    CELLULAR_SIM_CARD_PH_NETSUB_PUK,
    CELLULAR_SIM_CARD_SP_PIN,
    CELLULAR_SIM_CARD_SP_PUK,
    CELLULAR_SIM_CARD_CORP_PIN,
    CELLULAR_SIM_CARD_CORP_PUK,
    CELLULAR_SIM_CARD_IMSI_PIN,
    CELLULAR_SIM_CARD_IMSI_PUK,
    CELLULAR_SIM_CARD_INVALID,
    CELLULAR_SIM_CARD_LOCK_UNKNOWN,
} CellularSimCardLockState_t;

/**
 * @brief Represents network registration mode. Reference 3GPP TS 27.007 PLMN selection +COPS.
 */
typedef enum CellularNetworkRegistrationMode
{
    CELLULAR_NETWORK_REGISTRATION_MODE_AUTO = 0,
    CELLULAR_NETWORK_REGISTRATION_MODE_MANUAL = 1,
    CELLULAR_NETWORK_REGISTRATION_MODE_DEREGISTER = 2,
    CELLULAR_NETWORK_REGISTRATION_MODE_MANUAL_THEN_AUTO = 4,
    CELLULAR_NETWORK_REGISTRATION_MODE_MAX,
    CELLULAR_NETWORK_REGISTRATION_MODE_UNKNOWN,
} CellularNetworkRegistrationMode_t;

/**
 * @brief Represents network registration status. Reference 3GPP TS 27.007 network registration status.
 */
typedef enum CellularNetworkRegistrationStatus
{
    CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED_NOT_SEARCHING = 0,
    CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME = 1,
    CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED_SEARCHING = 2,
    CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTRATION_DENIED = 3,
    CELLULAR_NETWORK_REGISTRATION_STATUS_UNKNOWN = 4,
    CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING = 5,
    CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME_SMS_ONLY = 6,
    CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING_SMS_ONLY = 7,
    CELLULAR_NETWORK_REGISTRATION_STATUS_ATTACHED_EMERG_SERVICES_ONLY = 8,
    CELLULAR_NETWORK_REGISTRATION_STATUS_MAX,
} CellularNetworkRegistrationStatus_t;

/**
 * @brief Represents operator name format.
 */
typedef enum CellularOperatorNameFormat
{
    CELLULAR_OPERATOR_NAME_FORMAT_LONG = 0,
    CELLULAR_OPERATOR_NAME_FORMAT_SHORT = 1,
    CELLULAR_OPERATOR_NAME_FORMAT_NUMERIC = 2,
    CELLULAR_OPERATOR_NAME_FORMAT_NOT_PRESENT = 9,
    CELLULAR_OPERATOR_NAME_FORMAT_MAX,
} CellularOperatorNameFormat_t;

/**
 * @brief Represents URC events.
 */
typedef enum CellularUrcEvent
{
    CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION,
    CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION,
    CELLULAR_URC_EVENT_PDN_ACTIVATED,
    CELLULAR_URC_EVENT_PDN_DEACTIVATED,
    CELLULAR_URC_EVENT_SIGNAL_CHANGED,
    CELLULAR_URC_SOCKET_OPENED,
    CELLULAR_URC_SOCKET_OPEN_FAILED,
    CELLULAR_URC_EVENT_OTHER, /* Any URC event other than above. */
} CellularUrcEvent_t;

/**
 * @brief Represents Modem events.
 */
typedef enum CellularModemEvent
{
    CELLULAR_MODEM_EVENT_BOOTUP_OR_REBOOT,
    CELLULAR_MODEM_EVENT_POWERED_DOWN,
    CELLULAR_MODEM_EVENT_PSM_ENTER
} CellularModemEvent_t;

/**
 * @brief Represents PDN context type.
 */
typedef enum CellularPdnContextType
{
    CELLULAR_PDN_CONTEXT_IPV4 = 1,
    CELLULAR_PDN_CONTEXT_IPV6 = 2,
    CELLULAR_PDN_CONTEXT_IPV4V6 = 3,
    CELLULAR_PDN_CONTEXT_TYPE_MAX,
} CellularPdnContextType_t;

/**
 * @brief Represents PDN authentication type.
 */
typedef enum CellularPdnAuthType
{
    CELLULAR_PDN_AUTH_NONE = 0,   /**< No authentication. */
    CELLULAR_PDN_AUTH_PAP,        /**< Password Authentication Protocol (PAP). */
    CELLULAR_PDN_AUTH_CHAP,       /**< Challenge Handshake Authentication Protocol (CHAP). */
    CELLULAR_PDN_AUTH_PAP_OR_CHAP /**< PAP or CHAP. */
} CellularPdnAuthType_t;

/**
 * @brief Represents socket domain.
 */
typedef enum CellularSocketDomain
{
    CELLULAR_SOCKET_DOMAIN_AF_INET, /**< IPv4 Internet Protocols. */
    CELLULAR_SOCKET_DOMAIN_AF_INET6 /**< IPv6 Internet Protocols. */
} CellularSocketDomain_t;

/**
 * @brief Represents socket type.
 */
typedef enum CellularSocketType
{
    CELLULAR_SOCKET_TYPE_DGRAM, /**< Datagram. */
    CELLULAR_SOCKET_TYPE_STREAM /**< Byte-stream. */
} CellularSocketType_t;

/**
 * @brief Represents socket protocol.
 */
typedef enum CellularSocketProtocol
{
    CELLULAR_SOCKET_PROTOCOL_UDP,
    CELLULAR_SOCKET_PROTOCOL_TCP
} CellularSocketProtocol_t;

/**
 * @brief Represents data access modes.
 */
typedef enum CellularSocketAccessMode
{
    CELLULAR_ACCESSMODE_BUFFER = 0,  /* Data access buffer mode. */
    CELLULAR_ACCESSMODE_DIRECT_PUSH, /* Data access direct push mode. */
    CELLULAR_ACCESSMODE_TRANSPARENT, /* Data access transparent mode. */
    CELLULAR_ACCESSMODE_NOT_SET      /* Data access not set. */
} CellularSocketAccessMode_t;

/**
 * @brief Represents IP address.
 */
typedef enum CellularIPAddressType
{
    CELLULAR_IP_ADDRESS_V4,
    CELLULAR_IP_ADDRESS_V6
} CellularIPAddressType_t;

/**
 * @brief Represents socket option level.
 */
typedef enum CellularSocketOptionLevel
{
    CELLULAR_SOCKET_OPTION_LEVEL_IP,       /**< IP layer options. */
    CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT /**< Transport (TCP/UDP) layer options. */
} CellularSocketOptionLevel_t;

/**
 * @brief Socket option names.
 */
typedef enum CellularSocketOption
{
    CELLULAR_SOCKET_OPTION_MAX_IP_PACKET_SIZE, /**< Set Max IP packet size. */
    CELLULAR_SOCKET_OPTION_SEND_TIMEOUT,       /**< Set send timeout (in milliseconds). */
    CELLULAR_SOCKET_OPTION_RECV_TIMEOUT,       /**< Set receive timeout (in milliseconds). */
    CELLULAR_SOCKET_OPTION_PDN_CONTEXT_ID      /**< Set PDN Context ID to use for the socket. */
} CellularSocketOption_t;

/**
 * @brief packet Status Names.
 */
typedef enum CellularPktStatus
{
    CELLULAR_PKT_STATUS_OK = 0,             /* The operation was successful. */
    CELLULAR_PKT_STATUS_TIMED_OUT,          /* The operation timed out. */
    CELLULAR_PKT_STATUS_FAILURE,            /* There was some internal failure. */
    CELLULAR_PKT_STATUS_BAD_REQUEST,        /* Request was not valid. */
    CELLULAR_PKT_STATUS_BAD_RESPONSE,       /* The response received was not valid. */
    CELLULAR_PKT_STATUS_SIZE_MISMATCH,      /* There is a size mismatch between the params. */
    CELLULAR_PKT_STATUS_BAD_PARAM,          /* One or more params is not valid. */
    CELLULAR_PKT_STATUS_SEND_ERROR,         /* Modem returned a send error. */
    CELLULAR_PKT_STATUS_INVALID_HANDLE,     /* Invalid context. */
    CELLULAR_PKT_STATUS_CREATION_FAIL,      /* Resource creation fail. */
    CELLULAR_PKT_STATUS_PREFIX_MISMATCH,    /* Invalid prefix from Modem resp. */
    CELLULAR_PKT_STATUS_INVALID_DATA,       /* Invalid data from Modem resp. */
    CELLULAR_PKT_STATUS_PENDING_DATA,       /* Pending data from Modem resp. */
    CELLULAR_PKT_STATUS_PENDING_DATA_BUFFER /* Pending data from Modem resp. */
} CellularPktStatus_t;

/**
 * @brief Represents AT Command type.
 */
typedef enum CellularATCommandType
{
    CELLULAR_AT_NO_RESULT,            /**<  no response expected, only OK, ERROR etc. */
    CELLULAR_AT_WO_PREFIX,            /**<  string response without a prefix. */
    CELLULAR_AT_WITH_PREFIX,          /**<  string response with a prefix. */
    CELLULAR_AT_MULTI_WITH_PREFIX,    /**<  multiple line response all start with a prefix. */
    CELLULAR_AT_MULTI_WO_PREFIX,      /**<  multiple line response with or without a prefix. */
    CELLULAR_AT_MULTI_DATA_WO_PREFIX, /**<  multiple line data response with or without a prefix. */
    CELLULAR_AT_NO_COMMAND            /**<  no command is waiting response. */
} CellularATCommandType_t;

/**
 * @brief SIM Card status.
 */
typedef struct CellularSimCardStatus
{
    CellularSimCardState_t simCardState;
    CellularSimCardLockState_t simCardLockState;
} CellularSimCardStatus_t;

/**
 * @brief Public Land Mobile Network (PLMN) information.
 */
typedef struct CellularPlmnInfo
{
    char mcc[ CELLULAR_MCC_MAX_SIZE + 1 ]; /**< Mobile Country Code (MCC). */
    char mnc[ CELLULAR_MNC_MAX_SIZE + 1 ]; /**< Mobile Network Code (MNC). */
} CellularPlmnInfo_t;

/**
 * @brief SIM Card information.
 */
typedef struct CellularSimCardInfo
{
    char iccid[ CELLULAR_ICCID_MAX_SIZE + 1 ]; /**< Integrated Circuit Card Identifier aka SIM Card Number. */
    char imsi[ CELLULAR_IMSI_MAX_SIZE + 1 ];   /**< International Mobile Subscriber Identity. */
    CellularPlmnInfo_t plmn;                   /**< Public Land Mobile Network (PLMN) information. */
} CellularSimCardInfo_t;

/**
 * @brief Modem information.
 */
typedef struct CellularModemInfo
{
    char hardwareVersion[ CELLULAR_HW_VERSION_MAX_SIZE + 1 ];   /**< Hardware version number. */
    char firmwareVersion[ CELLULAR_FW_VERSION_MAX_SIZE + 1 ];   /**< Firmware version number. */
    char serialNumber[ CELLULAR_SERIAL_NUM_MAX_SIZE + 1 ];      /**< Module serial number. */
    char imei[ CELLULAR_IMEI_MAX_SIZE + 1 ];                    /**< International Mobile Equipment Identity (IMEI). */
    char manufactureId[ CELLULAR_MANUFACTURE_ID_MAX_SIZE + 1 ]; /**< Manufacture Identity. */
    char modelId[ CELLULAR_MODEL_ID_MAX_SIZE + 1 ];             /**< Model Identity. */
} CellularModemInfo_t;

/**
 * @brief Represents time.
 */
typedef struct CellularTime
{
    uint8_t month;    /**< 1..12 (Jan = 1 .. Dec = 12). */
    uint8_t day;      /**< Day of the month 1..31. */
    uint8_t hour;     /**< Hour of the day 0..23. */
    uint8_t minute;   /**< Minute of the hour 0..59. */
    uint8_t second;   /**< Second of the minute 0..59. */
    uint16_t year;    /**< Year (like 2019). */
    int32_t timeZone; /**< Timezone represented in 15 minute increments (UTC+1 will be 4 because 1 hour = 60 minutes = 4 * 15 minutes). */
    uint8_t dst;      /**< Daylight savings ( 0 = No adjustment for daylight savings
                      *                      1 = +1 hour for daylight savings
                      *                      2 = +2 hours for daylight savings ). */
} CellularTime_t;

/**
 * @brief Represents signal information.
 */
typedef struct CellularSignalInfo
{
    int16_t rssi; /**< Received Signal Strength Indicator (RSSI) in dBm.*/
    int16_t rsrp; /**< LTE Reference Signal Received Power (RSRP) in dBm. */
    int16_t rsrq; /**< LTE Reference Signal Received Quality (RSRQ) in dB. */
    int16_t sinr; /**< LTE Signal to Interference-Noise Ratio in dB. */
    int16_t ber;  /**< Bit Error Rate (BER) in 0.01%. */
    uint8_t bars; /**< A number between 0 to 5 (both inclusive) indicating signal strength. */
} CellularSignalInfo_t;

/**
 * @brief Represents network service status.
 */
typedef struct CellularServiceStatus
{
    CellularRat_t rat;                                         /**< Radio Access Technology (RAT). */
    CellularNetworkRegistrationMode_t networkRegistrationMode; /**< Network registration mode (auto/manual etc.) currently selected. */
    CellularNetworkRegistrationStatus_t csRegistrationStatus;  /**< CS (Circuit Switched) registration status (registered/searching/roaming etc.). */
    CellularNetworkRegistrationStatus_t psRegistrationStatus;  /**< PS (Packet Switched) registration status (registered/searching/roaming etc.). */
    CellularPlmnInfo_t plmnInfo;                               /**< Registered MCC and MNC information. */
    CellularOperatorNameFormat_t operatorNameFormat;           /**< Format of registered network operator name. */
    char operatorName[ CELLULAR_NETWORK_NAME_MAX_SIZE + 1 ];   /**< Registered network operator name. */
    uint8_t csRejectionType;                                   /**< CS Reject Type. 0 - 3GPP specific Reject Cause. 1 - Manufacture specific. */
    uint8_t csRejectionCause;                                  /**< Reason why the CS (Circuit Switched) registration attempt was rejected. */
    uint8_t psRejectionType;                                   /**< PS Reject Type. 0 - 3GPP specific Reject Cause. 1 - Manufacture specific. */
    uint8_t psRejectionCause;                                  /**< Reason why the PS (Packet Switched) registration attempt was rejected. */
} CellularServiceStatus_t;

/**
 * @brief Represents A singly-lined list of intermediate AT responses.
 */
typedef struct CellularATCommandLine
{
    struct CellularATCommandLine * pNext;
    char * pLine;
} CellularATCommandLine_t;

/**
 * @brief Represents AT Command response.
 */
typedef struct CellularATCommandResponse
{
    bool status;                    /* true: modem returns Success, false: Error. */
    CellularATCommandLine_t * pItm; /* Any intermediate responses. */
} CellularATCommandResponse_t;

/**
 * @brief Represents PSM settings.
 */
typedef struct CellularPsmSettings
{
    /*
     * 0 = PSM Disable
     * 1 = PSM Enable.
     */
    uint8_t mode; /**< PSM mode value (as shown above). */

    /*
     * Bits 5 to 1 represent the binary coded timer value
     * Bits 6 to 8 define the timer value unit as follows:
     * Bits
     * 8 7 6
     * 0 0 0 value is incremented in multiples of 10 minutes
     * 0 0 1 value is incremented in multiples of 1 hour
     * 0 1 0 value is incremented in multiples of 10 hours
     * 0 1 1 value is incremented in multiples of 2 seconds
     * 1 0 0 value is incremented in multiples of 30 seconds
     * 1 0 1 value is incremented in multiples of 1 minute
     *
     * e.g. "00001010" equals to 100 minutes.
     * first uint8_t is used for PSM set, whole uint32_t is used for PSM get.
     */
    uint32_t periodicTauValue; /**< TAU (T3412) value encoded as per spec (as shown above). */

    /*
     * Bits 5 to 1 represent the binary coded timer value
     * Bits 6 to 8 define the timer value unit as follows:
     * Bits
     * 8 7 6
     * 0 0 0 value is incremented in multiples of 10 minutes
     * 0 0 1 value is incremented in multiples of 1 hour
     * 0 1 0 value is incremented in multiples of 10 hours
     * 0 1 1 value is incremented in multiples of 2 seconds
     * 1 0 0 value is incremented in multiples of 30 seconds
     * 1 0 1 value is incremented in multiples of 1 minute
     *
     * e.g. "00001010" equals to 100 minutes.
     * first uint8_t is used for PSM set, whole uint32_t is used for PSM get.
     */
    uint32_t periodicRauValue; /**< RAU (T3312) value encoded as per Spec (as shown above). */

    /*
     * Bits 5 to 1 represent the binary coded timer value
     * Bits 6 to 8 define the timer value unit as follows:
     * Bits
     * 8 7 6
     * 0 0 0 value is incremented in multiples of 10 minutes
     * 0 0 1 value is incremented in multiples of 1 hour
     * 0 1 0 value is incremented in multiples of 10 hours
     * 0 1 1 value is incremented in multiples of 2 seconds
     * 1 0 0 value is incremented in multiples of 30 seconds
     * 1 0 1 value is incremented in multiples of 1 minute
     *
     * e.g. "00001010" equals to 100 minutes.
     * first uint8_t is used for PSM set, whole uint32_t is used for PSM get.
     */
    uint32_t gprsReadyTimer; /**< GPRS Ready timer (T3314) value encoded as per Spec. */

    /*
     * Bits 5 to 1 represent the binary coded timer value.
     * Bits 6 to 8 define the timer value unit as follows:
     * Bits
     * 8 7 6
     * 0 0 0 value is incremented in multiples of 2 seconds
     * 0 0 1 value is incremented in multiples of 1 minute
     * 0 1 0 value is incremented in multiples of decihours
     * 1 1 1 value indicates that the timer is deactivated.
     *
     * "00001111" equals to 30 seconds.
     * first uint8_t is used for PSM set, whole uint32_t is used for PSM get.
     */
    uint32_t activeTimeValue; /**< Active Time (T3324) value encoded as per spec (as shown above). */
} CellularPsmSettings_t;

/**
 * @brief Represents e-I-DRX settings.
 */
typedef struct CellularEidrxSettings
{
    /*
     * 0 Disable the use of e-I-DRX
     * 1 Enable the use of e-I-DRX
     * 2 Enable the use of e-I-DRX and enable the unsolicited result code
     * 3 Disable the use of e-I-DRX and discard all parameters for e-I-DRX or,
     *   if available, reset to the manufacturer specific default values.
     */
    uint8_t mode; /**< e-I-DRX mode (as shown above). */

    /*
     * 2 GSM
     * 3 UTRAN
     * 4 LTE Cat M1
     * 5 LTE Cat NB1
     */
    uint8_t rat; /**< Radio Access Technology (as shown above). */

    /*
     * String type. Half a byte in a 4 bit format.
     * bit
     * 4 3 2 1 E-UTRAN e-I-DRX cycle length duration
     * 0 0 0 0   5.12 seconds
     * 0 0 0 1   10.24 seconds
     * 0 0 1 0   20.48 seconds
     * 0 0 1 1   40.96 seconds
     * 0 1 0 0   61.44 seconds
     * 0 1 0 1   81.92 seconds
     * 0 1 1 0   102.4 seconds
     * 0 1 1 1   122.88 seconds
     * 1 0 0 0   143.36 seconds
     * 1 0 0 1   163.84 seconds
     * 1 0 1 0   327.68 seconds
     * 1 0 1 1   655,36 seconds
     * 1 1 0 0   1310.72 seconds
     * 1 1 0 1   2621.44 seconds
     * 1 1 1 0   5242.88 seconds
     * 1 1 1 1   10485.76 seconds
     */
    uint8_t requestedEdrxVaue;  /**< Requested eDRX value encoded as per spec (as shown above). */
    uint8_t nwProvidedEdrxVaue; /**< Network provided eDRX value encoded as per spec (as shown above). Only applicable in URC. */

    /*
     * LTE Cat M1 mode
     * bit
     * 4 3 2 1 Paging Time Window length
     * 0 0 0 0 1.28 seconds
     * 0 0 0 1 2.56 seconds
     * 0 0 1 0 3.84 seconds
     * 0 0 1 1 5.12 seconds
     * 0 1 0 0 6.4 seconds
     * 0 1 0 1 7.68 seconds
     * 0 1 1 0 8.96 seconds
     * 0 1 1 1 10.24 seconds
     * 1 0 0 0 11.52 seconds
     * 1 0 0 1 12.8 seconds
     * 1 0 1 0 14.08 seconds
     * 1 0 1 1 15.36 seconds
     * 1 1 0 0 16.64 seconds
     * 1 1 0 1 17.92 seconds
     * 1 1 1 0 19.20 seconds
     * 1 1 1 1 20.48 seconds
     *
     * LTE Cat NB1 mode
     * bit
     * 4 3 2 1 Paging Time Window length
     * 0 0 0 0 2.56 seconds
     * 0 0 0 1 5.12 seconds
     * 0 0 1 0 7.68 seconds
     * 0 0 1 1 10.24 seconds
     * 0 1 0 0 12.8 seconds
     * 0 1 0 1 15.36 seconds
     * 0 1 1 0 17.92 seconds
     * 0 1 1 1 20.48 seconds
     * 1 0 0 0 23.04 seconds
     * 1 0 0 1 25.6 seconds
     * 1 0 1 0 28.16 seconds
     * 1 0 1 1 30.72 seconds
     * 1 1 0 0 33.28 seconds
     * 1 1 0 1 35.84 seconds
     * 1 1 1 0 38.4 seconds
     * 1 1 1 1 40.96 seconds
     */
    uint8_t pagingTimeWindow; /**< Paging time window encoded as per spec (as shown above). Only applicable in URC. */
} CellularEidrxSettings_t;

typedef struct CellularEidrxSettingsList
{
    CellularEidrxSettings_t eidrxList[ CELLULAR_EDRX_LIST_MAX_SIZE ];
    uint8_t count;
} CellularEidrxSettingsList_t;

/**
 * @brief Represents IP Address.
 */
typedef struct CellularIPAddress
{
    CellularIPAddressType_t ipAddressType;              /**< Type of IP address (IPv4/IPv6). */
    char ipAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE + 1 ]; /**< IP Address. NULL terminated. */
} CellularIPAddress_t;

/**
 * @brief Represents a PDN config.
 */
typedef struct CellularPdnConfig
{
    CellularPdnContextType_t pdnContextType;                   /**< PDN Context type. */
    CellularPdnAuthType_t pdnAuthType;                         /**< PDN Authentication type. */
    const char apnName[ CELLULAR_APN_MAX_SIZE + 1 ];           /**< APN name. */
    const char username[ CELLULAR_PDN_USERNAME_MAX_SIZE + 1 ]; /**< Username. */
    const char password[ CELLULAR_PDN_PASSWORD_MAX_SIZE + 1 ]; /**< Password. */
} CellularPdnConfig_t;

/**
 * @brief Represents status of a PDN context.
 */
typedef struct CellularPdnStatus
{
    uint8_t contextId;                       /**< 1-16 are valid values. */
    uint8_t state;                           /**< 0 = Deactivated, 1 = Activated. */
    CellularPdnContextType_t pdnContextType; /**< PDN Context type. */
    CellularIPAddress_t ipAddress;           /**< Local IP address after the context is activated. */
} CellularPdnStatus_t;

/**
 * @brief Represents socket address.
 */
typedef struct CellularSocketAddress
{
    CellularIPAddress_t ipAddress; /**< IP address. */
    uint16_t port;                 /**< Port number. */
} CellularSocketAddress_t;

/**
 * @brief Callback used to inform about the response of an AT command sent
 * using Cellular_ATCommandRaw API.
 *
 * @param[in] cellularHandle The opaque cellular context pointer created by Cellular_Init.
 * @param[in] pResponse The response received for the AT command.
 * @param[in] pData is pATCommandPayload pointer in Cellular_ATCommandRaw parameters.
 * @param[in] dataLen is the string length of pATCommandPayloadin in Cellular_ATCommandRaw parameters.
 *
 * @return CELLULAR_PKT_STATUS_OK if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
typedef CellularPktStatus_t ( * CellularATCommandResponseReceivedCallback_t ) ( CellularHandle_t cellularHandle,
                                                                                const CellularATCommandResponse_t * pAtResp,
                                                                                void * pData,
                                                                                uint16_t dataLen );

/**
 * @brief Callback used to inform about a Network Registration URC event.
 *
 * @param[in] urcEvent URC Event that happened.
 * @param[in] pServiceStatus The status of the network service.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_RegisterUrcNetworkRegistrationEventCallback function.
 */
typedef void ( * CellularUrcNetworkRegistrationCallback_t )( CellularUrcEvent_t urcEvent,
                                                             const CellularServiceStatus_t * pServiceStatus,
                                                             void * pCallbackContext );

/**
 * @brief Callback used to inform about PDN URC events.
 *
 * @param[in] urcEvent URC Event that happened.
 * @param[in] contextId Context ID of the PDN context
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_RegisterUrcPdnEventCallback function.
 */
typedef void ( * CellularUrcPdnEventCallback_t )( CellularUrcEvent_t urcEvent,
                                                  uint8_t contextId,
                                                  void * pCallbackContext );

/**
 * @brief Callback used to inform about signal strength changed URC event.
 *
 * @param[in] urcEvent URC Event that happened.
 * @param[in] pSignalInfo The new signal information.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_RegisterUrcSignalStrengthChangedCallback function.
 */
typedef void ( * CellularUrcSignalStrengthChangedCallback_t )( CellularUrcEvent_t urcEvent,
                                                               const CellularSignalInfo_t * pSignalInfo,
                                                               void * pCallbackContext );

/**
 * @brief Generic callback used to inform all other URC events.
 *
 * @param[in] pRawData Raw data received in the URC event.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_RegisterUrcGenericCallback function.
 */
typedef void ( * CellularUrcGenericCallback_t )( const char * pRawData,
                                                 void * pCallbackContext );

/**
 * @brief Callback used to inform about modem events.
 *
 * @param[in] modemEvent The modem event.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_RegisterModemEventCallback function.
 */
typedef void ( * CellularModemEventCallback_t )( CellularModemEvent_t modemEvent,
                                                 void * pCallbackContext );

/**
 * @brief Callback used to inform about the status of socket open.
 *
 * @param[in] urcEvent URC Event that happened.
 * @param[in] socketHandle Socket handle for which data is ready.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_SocketRegisterSocketOpenCallback function.
 */
typedef void ( * CellularSocketOpenCallback_t )( CellularUrcEvent_t urcEvent,
                                                 CellularSocketHandle_t socketHandle,
                                                 void * pCallbackContext );

/**
 * @brief Callback used to inform that data is ready for reading on a socket.
 *
 * @param[in] socketHandle Socket handle for which data is ready.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_SocketRegisterDataReadyCallback function.
 */
typedef void ( * CellularSocketDataReadyCallback_t )( CellularSocketHandle_t socketHandle,
                                                      void * pCallbackContext );

/**
 * @brief Callback used to inform that remote end closed the connection for a
 * connected socket.
 *
 * @param[in] socketHandle Socket handle for which remote end closed the
 * connection.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_SocketRegisterClosedCallback function.
 */
typedef void ( * CellularSocketClosedCallback_t )( CellularSocketHandle_t socketHandle,
                                                   void * pCallbackContext );

#endif /* __CELLULAR_TYPES_H__ */
