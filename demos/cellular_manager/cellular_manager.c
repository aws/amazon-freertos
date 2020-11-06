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

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* cellular include. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_manager.h"
#include "cellular_api.h"

/*-----------------------------------------------------------*/

static void _cellularManagerSmTrigger( CellularManagerContext_t * pContext,
                                       cellularSmEvent_t event );
static CellularManagerContext_t * _cellularManagerAllocContext( void );
static void _cellularManagerFreeContext( CellularManagerContext_t * pContext );
static void _cellularManagerMutexLock( CellularManagerContext_t * pContext );
static void _cellularManagerMutexUnlock( CellularManagerContext_t * pContext );
static void _connectionStateChangedCallback( CellularUrcEvent_t urcEvent,
                                             const CellularServiceStatus_t * pServiceStatus,
                                             void * pCallbackContext );
static void _pdnStateChangedCallback( CellularUrcEvent_t urcEvent,
                                      uint8_t contextId,
                                      void * pCallbackContext );
static void _modemEventCallback( CellularModemEvent_t modemEvent,
                                 void * pCallbackContext );

/*-----------------------------------------------------------*/

#if ( CELLULAR_CONFIG_STATIC_ALLOCATION_CONTEXT == 1 )
    static CellularManagerContext_t _cellularManagerContext;
#endif

/*-----------------------------------------------------------*/

static CellularManagerContext_t * _cellularManagerAllocContext( void )
{
    CellularManagerContext_t * pContext = NULL;

    taskENTER_CRITICAL();

    #if ( CELLULAR_CONFIG_STATIC_ALLOCATION_CONTEXT == 1 )
        {
            pContext = &_cellularManagerContext;
        }
    #else
        {
            pContext = ( CellularManagerContext_t * ) pvPortMalloc( sizeof( CellularManagerContext_t ) );
        }
    #endif

    if( pContext != NULL )
    {
        ( void ) memset( pContext, 0, sizeof( CellularManagerContext_t ) );
    }

    taskEXIT_CRITICAL();

    return pContext;
}

/*-----------------------------------------------------------*/

static void _cellularManagerFreeContext( CellularManagerContext_t * pContext )
{
    taskENTER_CRITICAL();

    if( pContext != NULL )
    {
        #if ( CELLULAR_CONFIG_STATIC_ALLOCATION_CONTEXT == 0 )
            {
                vPortFree( pContext );
            }
        #endif
    }

    taskEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/

static void _cellularManagerMutexLock( CellularManagerContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Lock( &pContext->cellularSmMutex );
    }
}

/*-----------------------------------------------------------*/

static void _cellularManagerMutexUnlock( CellularManagerContext_t * pContext )
{
    if( pContext != NULL )
    {
        IotMutex_Unlock( &pContext->cellularSmMutex );
    }
}

/*-----------------------------------------------------------*/

static void _cellularManagerInitCheckSIMStatus( CellularManagerContext_t * pContext,
                                                CellularSimCardStatus_t * pSimStatus,
                                                const uint32_t waitInterval )
{
    uint8_t retries = 0;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    while( retries++ < CELLULAR_MAX_SIM_RETRY )
    {
        cellularStatus = Cellular_GetSimCardStatus( pContext->cellularHandle, pSimStatus );
        IotLogDebug( "SIM card state = %d, SIM lock state = %d", pSimStatus->simCardState, pSimStatus->simCardLockState );

        if( ( cellularStatus == CELLULAR_SUCCESS ) &&
            ( pSimStatus->simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
            ( pSimStatus->simCardLockState == CELLULAR_SIM_CARD_READY ) )
        {
            IotLogInfo( "SIM card is READY" );
            break;
        }

        vTaskDelay( pdMS_TO_TICKS( waitInterval ) );
    }
}

/*-----------------------------------------------------------*/

static CellularManagerError_t _getConnectionSateFromStatemachine( CellularManagerContext_t * pContext,
                                                                  CellularManagerConnectionState_t * pConnectionState )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;
    cellularSmState_t smState = CELLULAR_SM_UNKNOWN;

    if( pConnectionState == NULL )
    {
        cellularMgrStatus = CELLULAR_MANAGER_BAD_PARAMETER;
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        _cellularManagerMutexLock( pContext );

        if( pContext != NULL )
        {
            smState = pContext->cellularSmState;
        }

        _cellularManagerMutexUnlock( pContext );

        switch( smState )
        {
            case CELLULAR_SM_OFF:
                *pConnectionState = CELLULAR_CONNECTION_OFF;
                break;

            case CELLULAR_SM_REGISTERED:
                *pConnectionState = CELLULAR_CONNECTION_REGISTERED;
                break;

            case CELLULAR_SM_CONNECTED:
                *pConnectionState = CELLULAR_CONNECTION_CONNECTED;
                break;

            case CELLULAR_SM_RFOFF:
                *pConnectionState = CELLULAR_CONNECTION_RF_OFF;
                break;

            case CELLULAR_SM_OTA:
                *pConnectionState = CELLULAR_CONNECTION_DISCONNECTED_FOTA_IN_PROGRESS;
                break;

            case CELLULAR_SM_ON:
                *pConnectionState = CELLULAR_CONNECTION_DISCONNECTED;
                break;

            default:
                *pConnectionState = CELLULAR_CONNECTION_UNKNOWN;
                break;
        }
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

static void _connectionStateChangedCallback( CellularUrcEvent_t urcEvent,
                                             const CellularServiceStatus_t * pServiceStatus,
                                             void * pCallbackContext )
{
    CellularManagerContext_t * pContext = ( CellularManagerContext_t * ) pCallbackContext;
    CellularManagerConnectionState_t connectionState = CELLULAR_CONNECTION_UNKNOWN;

    if( ( pServiceStatus == NULL ) || ( ( urcEvent != CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION ) && ( urcEvent != CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION ) ) )
    {
        IotLogError( "Invalid ConnectionState callback urcEvent %d", urcEvent );
    }
    else if( pContext == NULL )
    {
        IotLogError( "Cellular manager context NULL!" );
    }
    else
    {
        if( urcEvent == CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION )
        {
            switch( pServiceStatus->psRegistrationStatus )
            {
                case CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME:
                case CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING:
                    _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_ATTACHED );
                    break;

                default:
                    /* PS de-registered. */
                    _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_DETACHED );
                    break;
            }
        }

        if( urcEvent == CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION )
        {
            switch( pServiceStatus->csRegistrationStatus )
            {
                case CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME:
                case CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING:
                    /* CS attach won't count as CELLULAR_EVENT_ATTACHED. */
                    break;

                default:
                    /* CS de-registered. */
                    _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_DETACHED );
                    break;
            }
        }

        /* query the correct value for connectionSate. */
        ( void ) _getConnectionSateFromStatemachine( pContext, &connectionState );
        IotLogDebug( "connectionStateChangedCallback connection changed to  %d", connectionState );

        if( ( pContext != NULL ) && ( pContext->cellularMgrCallbackEvents.connectionStateChangedCallback != NULL ) )
        {
            pContext->cellularMgrCallbackEvents.connectionStateChangedCallback( pContext->cellularMgrCallbackEvents.connectionStateChangedCallbackData, connectionState );
        }
    }
}

/*-----------------------------------------------------------*/

static void _pdnStateChangedCallback( CellularUrcEvent_t urcEvent,
                                      uint8_t contextId,
                                      void * pCallbackContext )
{
    CellularManagerContext_t * pContext = ( CellularManagerContext_t * ) pCallbackContext;
    CellularManagerConnectionState_t connectionState = CELLULAR_CONNECTION_UNKNOWN;

    switch( urcEvent )
    {
        case CELLULAR_URC_EVENT_PDN_ACTIVATED:
            _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_DATA_ACTIVE );
            break;

        case CELLULAR_URC_EVENT_PDN_DEACTIVATED:
            _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_DATA_INACTIVE );
            break;

        default:
            IotLogWarn( "_pdnStateChangedCallback Wrong URC event %d", urcEvent );
            break;
    }

    ( void ) _getConnectionSateFromStatemachine( pContext, &connectionState );
    IotLogDebug( "pdnStateChangedCallback connection changed to  %d", connectionState );

    if( ( pContext != NULL ) && ( pContext->cellularMgrCallbackEvents.connectionStateChangedCallback != NULL ) )
    {
        pContext->cellularMgrCallbackEvents.connectionStateChangedCallback( pContext->cellularMgrCallbackEvents.connectionStateChangedCallbackData, connectionState );
    }
}

/*-----------------------------------------------------------*/

static void _modemEventCallback( CellularModemEvent_t modemEvent,
                                 void * pCallbackContext )
{
    CellularManagerContext_t * pContext = ( CellularManagerContext_t * ) pCallbackContext;
    CellularManagerConnectionState_t connectionState = CELLULAR_CONNECTION_UNKNOWN;

    switch( modemEvent )
    {
        case CELLULAR_MODEM_EVENT_BOOTUP_OR_REBOOT:

            /* this event is determined by receiving a "READY" URC, since "READY" URC appears under
             * 1. first time boot up
             * 2. modem crash (reboot)
             * we need a flag to determine the actions. We should only put state machine to OFF state and report CELLULAR_CONNECTION_REBOOTED to the application
             * when modem crash (reboot) happens. Upon receiving such EVENT callback, the application should perform the following to recovery
             * 1. CellularManager_Cleanup()
             * 2. CellularManager_Init()
             * NOTE: no action or reporting needed for "first time boot up"
             */
            if( pContext != NULL )
            {
                if( pContext->isFirstTimeBootUp == true )
                {
                    _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_PWR_OFF );
                    connectionState = CELLULAR_CONNECTION_REBOOTED;
                }

                /* if isFirstTimeBootUp is false, it's the first time we receive this CELLULAR_MODEM_EVENT_BOOTUP_OR_REBOOT,
                 * must be first time boot up instead of crash
                 */
                else
                {
                    pContext->isFirstTimeBootUp = true;
                    /* no state change, no reporting to upper layer. */
                }
            }

            break;

        case CELLULAR_MODEM_EVENT_POWERED_DOWN:
            _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_PWR_OFF );
            connectionState = CELLULAR_CONNECTION_OFF;
            break;

        default:
            /* FOTA progress is not supported as of now. */
            break;
    }

    if( connectionState != CELLULAR_CONNECTION_UNKNOWN )
    {
        IotLogDebug( "ModemEventCallback connection changed to %d", connectionState );

        if( ( pContext != NULL ) && ( pContext->cellularMgrCallbackEvents.connectionStateChangedCallback != NULL ) )
        {
            pContext->cellularMgrCallbackEvents.connectionStateChangedCallback( pContext->cellularMgrCallbackEvents.connectionStateChangedCallbackData, connectionState );
        }
    }
}

/*-----------------------------------------------------------*/


static cellularSmState_t _cellularManagerSmTriggerSwitchSmOff( cellularSmEvent_t event,
                                                               cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( event )
    {
        case CELLULAR_EVENT_PWR_ON:
            newState = CELLULAR_SM_ON;
            break;

        /* In rare case, the registered urc may come earlier
         * than event CELLULAR_EVENT_PWR_ON
         */
        case CELLULAR_EVENT_ATTACHED:
            newState = CELLULAR_SM_REGISTERED;
            break;

        default:
            IotLogWarn( "_cellularManagerSmTriggerSwitchSmOff: Wrong event received: %d", event );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static cellularSmState_t _cellularManagerSmTriggerSwitchSmOn( cellularSmEvent_t event,
                                                              cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( event )
    {
        case CELLULAR_EVENT_PWR_OFF:
            newState = CELLULAR_SM_OFF;
            break;

        case CELLULAR_EVENT_RF_OFF:
            newState = CELLULAR_SM_RFOFF;
            break;

        case CELLULAR_EVENT_ATTACHED:
            newState = CELLULAR_SM_REGISTERED;
            break;

        case CELLULAR_EVENT_DETACHED:
            newState = CELLULAR_SM_ON;
            break;

        default:
            IotLogWarn( "_cellularManagerSmTriggerSwitchSmOn: Wrong event received: %d", event );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static cellularSmState_t _cellularManagerSmTriggerSwitchSmRegistered( cellularSmEvent_t event,
                                                                      cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( event )
    {
        case CELLULAR_EVENT_PWR_OFF:
            newState = CELLULAR_SM_OFF;
            break;

        case CELLULAR_EVENT_RF_OFF:
            newState = CELLULAR_SM_RFOFF;
            break;

        case CELLULAR_EVENT_DETACHED:
            newState = CELLULAR_SM_ON;
            break;

        case CELLULAR_EVENT_DATA_ACTIVE:
            newState = CELLULAR_SM_CONNECTED;
            break;

        default:
            IotLogWarn( "_cellularManagerSmTriggerSwitchSmRegistered: Wrong event received: %d", event );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static cellularSmState_t _cellularManagerSmTriggerSwitchSmConnected( cellularSmEvent_t event,
                                                                     cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( event )
    {
        case CELLULAR_EVENT_PWR_OFF:
            newState = CELLULAR_SM_OFF;
            break;

        case CELLULAR_EVENT_RF_OFF:
            newState = CELLULAR_SM_RFOFF;
            break;

        case CELLULAR_EVENT_DETACHED:
            newState = CELLULAR_SM_ON;
            break;

        case CELLULAR_EVENT_OTA:
            newState = CELLULAR_SM_OTA;
            break;

        case CELLULAR_EVENT_DATA_INACTIVE:
            newState = CELLULAR_SM_REGISTERED;
            break;

        default:
            IotLogWarn( "_cellularManagerSmTriggerSwitchSmConnected: Wrong event received: %d", event );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static cellularSmState_t _cellularManagerSmTriggerSwitchSmRfoff( cellularSmEvent_t event,
                                                                 cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( event )
    {
        case CELLULAR_EVENT_PWR_ON:
            newState = CELLULAR_SM_RFOFF;
            break;

        case CELLULAR_EVENT_PWR_OFF:
            newState = CELLULAR_SM_OFF;
            break;

        case CELLULAR_EVENT_RF_ON:
            newState = CELLULAR_SM_ON;
            break;

        default:
            IotLogWarn( "_cellularManagerSmTriggerSwitchSmRfoff: Wrong event received: %d", event );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static cellularSmState_t _cellularManagerSmTriggerSwitchSmOta( cellularSmEvent_t event,
                                                               cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( event )
    {
        case CELLULAR_EVENT_OTA_DONE:
            newState = CELLULAR_SM_OFF;
            break;

        case CELLULAR_EVENT_PWR_OFF:
            newState = CELLULAR_SM_OFF;
            break;

        default:
            IotLogWarn( "_cellularManagerSmTriggerSwitchSmOta: Wrong event received: %d", event );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static cellularSmState_t _cellularManagerSmTriggerSwitchOldState( cellularSmEvent_t event,
                                                                  cellularSmState_t oldState )
{
    cellularSmState_t newState = CELLULAR_SM_UNKNOWN;

    switch( oldState )
    {
        case CELLULAR_SM_OFF:
            newState = _cellularManagerSmTriggerSwitchSmOff( event, oldState );
            break;

        case CELLULAR_SM_ON:
            newState = _cellularManagerSmTriggerSwitchSmOn( event, oldState );
            break;

        case CELLULAR_SM_REGISTERED:
            newState = _cellularManagerSmTriggerSwitchSmRegistered( event, oldState );
            break;

        case CELLULAR_SM_CONNECTED:
            newState = _cellularManagerSmTriggerSwitchSmConnected( event, oldState );
            break;

        case CELLULAR_SM_RFOFF:
            newState = _cellularManagerSmTriggerSwitchSmRfoff( event, oldState );
            break;

        case CELLULAR_SM_OTA:
            newState = _cellularManagerSmTriggerSwitchSmOta( event, oldState );
            break;

        default:
            /* should never enter here. */
            IotLogError( "_cellularManagerSmTrigger: Wrong State: %d", oldState );
            break;
    }

    return newState;
}

/*-----------------------------------------------------------*/

static void _cellularManagerSmTrigger( CellularManagerContext_t * pContext,
                                       cellularSmEvent_t event )
{
    #if LIBRARY_LOG_LEVEL >= IOT_LOG_INFO
        static const _enumStringMap_t _cellularSmInfoMapping[] =
        {
            { "OFF",        ( uint32_t ) CELLULAR_SM_OFF        },
            { "ON",         ( uint32_t ) CELLULAR_SM_ON         },
            { "CONNECTED",  ( uint32_t ) CELLULAR_SM_CONNECTED  },
            { "REGISTERED", ( uint32_t ) CELLULAR_SM_REGISTERED },
            { "RFOFF",      ( uint32_t ) CELLULAR_SM_RFOFF      },
            { "OTA",        ( uint32_t ) CELLULAR_SM_OTA        },
            { "UNKNOWN",    ( uint32_t ) CELLULAR_SM_UNKNOWN    },
        };

        static const _enumStringMap_t _cellularEventMapping[] =
        {
            { "PWR_ON",       ( uint32_t ) CELLULAR_EVENT_PWR_ON        },
            { "PWR_OFF",      ( uint32_t ) CELLULAR_EVENT_PWR_OFF       },
            { "RF_ON",        ( uint32_t ) CELLULAR_EVENT_RF_ON         },
            { "RF_OFF",       ( uint32_t ) CELLULAR_EVENT_RF_OFF        },
            { "ATTACHED",     ( uint32_t ) CELLULAR_EVENT_ATTACHED      },
            { "DETACHED",     ( uint32_t ) CELLULAR_EVENT_DETACHED      },
            { "PDN_ACTIVE",   ( uint32_t ) CELLULAR_EVENT_DATA_ACTIVE   },
            { "PDN_INACTIVE", ( uint32_t ) CELLULAR_EVENT_DATA_INACTIVE },
            { "OTA",          ( uint32_t ) CELLULAR_EVENT_OTA           },
            { "OTA_DONE",     ( uint32_t ) CELLULAR_EVENT_OTA_DONE      },
        };
    #endif /* if LIBRARY_LOG_LEVEL >= IOT_LOG_INFO */

    cellularSmState_t oldState = CELLULAR_SM_UNKNOWN, newState = CELLULAR_SM_UNKNOWN;

    if( event >= CELLULAR_NUM_EVENT )
    {
        IotLogError( "Invalid State Machine event %d", event );
    }
    else
    {
        _cellularManagerMutexLock( pContext );

        if( pContext != NULL )
        {
            oldState = pContext->cellularSmState;
            /* if no valid state trigger, newState should be same as oldState. */
            newState = oldState;
        }

        newState = _cellularManagerSmTriggerSwitchOldState( event, oldState );

        if( oldState != newState )
        {
            if( ( pContext != NULL ) && ( newState != CELLULAR_SM_UNKNOWN ) )
            {
                pContext->cellularSmState = newState;
            }
        }

        _cellularManagerMutexUnlock( pContext );
        IotLogInfo( "State Machine Change oldState = %s, newState = %s ,trigger = %s",
                    _cellularSmInfoMapping[ oldState ].str, _cellularSmInfoMapping[ newState ].str, _cellularEventMapping[ event ].str );
    }
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_Init( CellularManagerContext_t ** pCellularManagerContext,
                                             const CellularCommInterface_t * pCommInterface )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularSimCardStatus_t simStatus;
    CellularManagerContext_t * pContext;
    const uint32_t waitInterval = 500UL;

    pContext = _cellularManagerAllocContext();

    if( pContext == NULL )
    {
        IotLogError( "CELLULAR manager context allocation failed" );
        cellularMgrStatus = CELLULAR_MANAGER_NO_MEMORY;
    }
    else
    {
        if( IotMutex_Create( &pContext->cellularSmMutex, false ) )
        {
            pContext->cellularSmMutexCreated = true;
        }
        else
        {
            cellularMgrStatus = CELLULAR_MANAGER_NO_MEMORY;
        }
    }

    /* Cellular HAL Init. */
    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        pContext->cellularSmState = CELLULAR_SM_OFF;
        pContext->isFirstTimeBootUp = false;
        cellularStatus = Cellular_Init( &pContext->cellularHandle, pCommInterface );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            _cellularManagerSmTrigger( pContext, CELLULAR_EVENT_PWR_ON );
        }
        else
        {
            cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
        }
    }

    /* Check sim status. */
    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        pContext->libOpened = true;

        /* at this point, if isFirstTimeBootUp still false, we must have missed READY URC, set it to true. */
        if( pContext->isFirstTimeBootUp == false )
        {
            pContext->isFirstTimeBootUp = true;
        }

        /* Check the sim card status. */
        _cellularManagerInitCheckSIMStatus( pContext, &simStatus, waitInterval );
    }

    /* if any error happens, we need to do clean up. */
    if( cellularMgrStatus != CELLULAR_MANAGER_SUCCESS )
    {
        ( void ) CellularManager_Cleanup( pContext );
    }
    else
    {
        /* Assigned allocated context back to user pointer. */
        *pCellularManagerContext = pContext;
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_Cleanup( CellularManagerContext_t * pCellularManagerContext )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;

    if( pCellularManagerContext == NULL )
    {
        cellularMgrStatus = CELLULAR_MANAGER_UNKNOWN;
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        if( pCellularManagerContext->cellularSmMutexCreated )
        {
            IotMutex_Destroy( &pCellularManagerContext->cellularSmMutex );
        }

        if( Cellular_Cleanup( pCellularManagerContext->cellularHandle ) != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to close HAL layer library" );
        }
        else
        {
            pCellularManagerContext->cellularHandle = NULL;
        }

        if( pCellularManagerContext != NULL )
        {
            _cellularManagerFreeContext( pCellularManagerContext );
        }
    }

    IotLogInfo( "CELLULAR Manager library closed" );

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_Connect( CellularManagerContext_t * pCellularManagerContext,
                                                uint8_t contextId,
                                                const CellularPdnConfig_t * pPdnConfig,
                                                uint32_t timeoutMilliseconds )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;
    const uint32_t waitInterval = 1000UL;
    uint32_t timeoutCountLimit = ( timeoutMilliseconds / waitInterval ) + 1U;
    uint32_t timeoutCount = 0;
    CellularServiceStatus_t serviceStatus;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if( ( pCellularManagerContext != NULL ) && ( pCellularManagerContext->libOpened == false ) )
    {
        cellularMgrStatus = CELLULAR_MANAGER_LIB_NOT_OPENED;
    }

    /* Setup PDN. */
    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        cellularStatus = Cellular_SetPdnConfig( pCellularManagerContext->cellularHandle, contextId, pPdnConfig );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
        }
    }

    /* Rescan network. */
    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        cellularStatus = Cellular_RfOff( pCellularManagerContext->cellularHandle );

        if( cellularStatus == CELLULAR_SUCCESS )
        {
            cellularStatus = Cellular_RfOn( pCellularManagerContext->cellularHandle );
        }

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
        }
    }

    /* check registration status. */
    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        while( timeoutCount < timeoutCountLimit )
        {
            cellularStatus = Cellular_GetServiceStatus( pCellularManagerContext->cellularHandle, &serviceStatus );

            if( ( cellularStatus == CELLULAR_SUCCESS ) &&
                ( serviceStatus.rat != CELLULAR_RAT_INVALID ) &&
                ( ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME ) ||
                  ( serviceStatus.psRegistrationStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING ) ) )
            {
                IotLogDebug( "Registration is successful" );
                _cellularManagerSmTrigger( pCellularManagerContext, CELLULAR_EVENT_ATTACHED );
                break;
            }
            else
            {
                timeoutCount++;

                if( timeoutCount >= timeoutCountLimit )
                {
                    cellularMgrStatus = CELLULAR_MANAGER_TIMEOUT;
                }

                vTaskDelay( pdMS_TO_TICKS( waitInterval ) );
            }
        }
    }

    /* Activate PDN. */
    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        cellularStatus = Cellular_ActivatePdn( pCellularManagerContext->cellularHandle, contextId );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            /* Try deactivate PDN then activate it again. */
            IotLogInfo( "Activate PDN failed. Deactivate the PDN and retry." );
            ( void ) Cellular_DeactivatePdn( pCellularManagerContext->cellularHandle, contextId );
            cellularStatus = Cellular_ActivatePdn( pCellularManagerContext->cellularHandle, contextId );

            if( cellularStatus != CELLULAR_SUCCESS )
            {
                IotLogError( "Activate PDN failed after retry." );
                cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
            }
        }
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        _cellularManagerSmTrigger( pCellularManagerContext, CELLULAR_EVENT_DATA_ACTIVE );
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_Disconnect( CellularManagerContext_t * pCellularManagerContext,
                                                   uint8_t contextId )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if( ( pCellularManagerContext != NULL ) && ( pCellularManagerContext->libOpened == false ) )
    {
        cellularMgrStatus = CELLULAR_MANAGER_LIB_NOT_OPENED;
    }

    cellularStatus = Cellular_DeactivatePdn( pCellularManagerContext->cellularHandle, contextId );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
    }
    else
    {
        _cellularManagerSmTrigger( pCellularManagerContext, CELLULAR_EVENT_DATA_INACTIVE );
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_GetConnectionState( CellularManagerContext_t * pCellularManagerContext,
                                                           CellularManagerConnectionState_t * pConnectionState )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularSimCardStatus_t simCardStatus;
    CellularServiceStatus_t serviceStatus;

    if( ( pCellularManagerContext != NULL ) && ( pCellularManagerContext->libOpened == false ) )
    {
        cellularMgrStatus = CELLULAR_MANAGER_LIB_NOT_OPENED;
    }
    else
    {
        if( pConnectionState == NULL )
        {
            cellularMgrStatus = CELLULAR_MANAGER_BAD_PARAMETER;
        }
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        cellularMgrStatus = _getConnectionSateFromStatemachine( pCellularManagerContext, pConnectionState );

        /* Need to specify disconnected reason. */
        if( *pConnectionState == CELLULAR_CONNECTION_DISCONNECTED )
        {
            cellularStatus = Cellular_GetSimCardStatus( pCellularManagerContext->cellularHandle, &simCardStatus );

            if( cellularStatus == CELLULAR_SUCCESS )
            {
                if( ( simCardStatus.simCardLockState != CELLULAR_SIM_CARD_READY ) || ( simCardStatus.simCardState != CELLULAR_SIM_CARD_INSERTED ) )
                {
                    *pConnectionState = CELLULAR_CONNECTION_DISCONNECTED_NO_SIM;
                }
            }

            cellularStatus = Cellular_GetServiceStatus( pCellularManagerContext->cellularHandle, &serviceStatus );

            if( cellularStatus == CELLULAR_SUCCESS )
            {
                if( ( serviceStatus.csRejectionCause == 0u ) || ( serviceStatus.psRejectionCause == 0u ) )
                {
                    *pConnectionState = CELLULAR_CONNECTION_DISCONNECTED_REGISTRATION_REJECTED;
                }
            }
        }
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_AirplaneModeOn( CellularManagerContext_t * pCellularManagerContext )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;

    if( ( pCellularManagerContext != NULL ) && ( pCellularManagerContext->libOpened == false ) )
    {
        cellularMgrStatus = CELLULAR_MANAGER_LIB_NOT_OPENED;
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        if( Cellular_RfOff( pCellularManagerContext->cellularHandle ) != CELLULAR_SUCCESS )
        {
            cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
        }

        _cellularManagerSmTrigger( pCellularManagerContext, CELLULAR_EVENT_RF_OFF );
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_AirplaneModeOff( CellularManagerContext_t * pCellularManagerContext )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;

    if( ( pCellularManagerContext != NULL ) && ( pCellularManagerContext->libOpened == false ) )
    {
        cellularMgrStatus = CELLULAR_MANAGER_LIB_NOT_OPENED;
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        if( Cellular_RfOn( pCellularManagerContext->cellularHandle ) != CELLULAR_SUCCESS )
        {
            cellularMgrStatus = CELLULAR_MANAGER_HAL_ERROR;
        }

        _cellularManagerSmTrigger( pCellularManagerContext, CELLULAR_EVENT_RF_ON );
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_SetConnectionStateChangedCallback( CellularManagerContext_t * pCellularManagerContext,
                                                                          void * pUserData,
                                                                          CellularManagerConnectionStateChangedCallback_t connectionStateChangedCallback )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;

    if( connectionStateChangedCallback == NULL )
    {
        cellularMgrStatus = CELLULAR_MANAGER_BAD_PARAMETER;
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        ( void ) Cellular_RegisterUrcNetworkRegistrationEventCallback( pCellularManagerContext->cellularHandle, &_connectionStateChangedCallback, ( void * ) pCellularManagerContext );
        ( void ) Cellular_RegisterUrcPdnEventCallback( pCellularManagerContext->cellularHandle, &_pdnStateChangedCallback, ( void * ) pCellularManagerContext );
        ( void ) Cellular_RegisterModemEventCallback( pCellularManagerContext->cellularHandle, &_modemEventCallback, ( void * ) pCellularManagerContext );

        if( pCellularManagerContext != NULL )
        {
            pCellularManagerContext->cellularMgrCallbackEvents.connectionStateChangedCallback = connectionStateChangedCallback;
            pCellularManagerContext->cellularMgrCallbackEvents.connectionStateChangedCallbackData = pUserData;
        }
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_SetSignalStrengthChangedCallback( CellularManagerContext_t * pCellularManagerContext,
                                                                         void * pUserData,
                                                                         CellularManagerSignalStrengthChangedCallback_t signalStrengthChangedCallback )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;

    if( signalStrengthChangedCallback == NULL )
    {
        cellularMgrStatus = CELLULAR_MANAGER_BAD_PARAMETER;
    }

    if( cellularMgrStatus == CELLULAR_MANAGER_SUCCESS )
    {
        /* currently no signal URC avail. */
        if( pCellularManagerContext != NULL )
        {
            pCellularManagerContext->cellularMgrCallbackEvents.signalStrengthChangedCallback = signalStrengthChangedCallback;
            pCellularManagerContext->cellularMgrCallbackEvents.signalStrengthChangedCallbackData = pUserData;
        }
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/

CellularManagerError_t CellularManager_GetCellularHandle( const CellularManagerContext_t * pCellularManagerContext,
                                                          CellularHandle_t * pCellularHandle )
{
    CellularManagerError_t cellularMgrStatus = CELLULAR_MANAGER_SUCCESS;

    if( pCellularManagerContext == NULL )
    {
        cellularMgrStatus = CELLULAR_MANAGER_UNKNOWN;
    }
    else
    {
        *pCellularHandle = pCellularManagerContext->cellularHandle;
    }

    return cellularMgrStatus;
}

/*-----------------------------------------------------------*/
