/*
 * $ Copyright Cypress Semiconductor $
 */

#include "cy_result_mw.h"

/******************************************************
 *                      Macros
 ******************************************************/
/*
 * Results returned by WIFI SUPPLICANT library
 */
#define CY_RSLT_MODULE_WIFI_SUPPLICANT_ERR_CODE_START          (0)
#define CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE                     CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_WIFI_SUPPLICANT_BASE, CY_RSLT_MODULE_WIFI_SUPPLICANT_ERR_CODE_START)

#define CY_RSLT_WIFI_SUPPLICANT_GENERIC_ERROR                  ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 1))
#define CY_RSLT_WIFI_SUPPLICANT_OUT_OF_HEAP_SPACE              ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 2))
#define CY_RSLT_WIFI_SUPPLICANT_TIMEOUT                        ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 4))
#define CY_RSLT_WIFI_SUPPLICANT_ERROR                          ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 5))
#define CY_RSLT_WIFI_SUPPLICANT_IN_PROGRESS                    ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 6))
#define CY_RSLT_WIFI_SUPPLICANT_ABORTED                        ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 7))
#define CY_RSLT_WIFI_SUPPLICANT_NOT_STARTED                    ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 8))
#define CY_RSLT_WIFI_SUPPLICANT_ERROR_STACK_MALLOC_FAIL        ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 9))
#define CY_RSLT_WIFI_SUPPLICANT_COMPLETE                       ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 10))
#define CY_RSLT_WIFI_SUPPLICANT_ERROR_AT_THREAD_START          ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 11))
#define CY_RSLT_WIFI_SUPPLICANT_UNPROCESSED                    ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 12))
#define CY_RSLT_WIFI_SUPPLICANT_ERROR_CREATING_EAPOL_PACKET    ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 13))
#define CY_RSLT_WIFI_SUPPLICANT_ERROR_READING_BSSID            ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 14))
#define CY_RSLT_WIFI_SUPPLICANT_RECEIVED_EAP_FAIL              ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 15))
#define CY_RSLT_WIFI_SUPPLICANT_TLS_HANDSHAKE_FAILURE          ((cy_rslt_t)(CY_RSLT_WIFI_SUPPLICANT_ERROR_BASE + 16))
