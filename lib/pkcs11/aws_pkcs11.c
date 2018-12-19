#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"
#include "FreeRTOS.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"

/* C runtime includes. */
#include <stdio.h>
#include <stdint.h>

/*-----------------------------------------------------------*/

/* @brief Get a list of available PKCS #11 slots.
 *
 * \note This function allocates memory for slots.
 * Freeing this memory is the responsibility of the caller.
 *
 *	\param[out] ppxSlotId       Pointer to slot list.  This slot list is
 *                              malloc'ed by the function and must be
 *                              freed by the caller.
 *  \param[out] pxSlotCount     Pointer to the number of slots found.
 *
 *  \return CKR_OK or PKCS #11 error code. (PKCS #11 error codes are positive).
 *
 */
CK_RV prvGetSlotList( CK_SLOT_ID ** ppxSlotId,
                      CK_ULONG * pxSlotCount )
{
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_SLOT_ID * pxSlotId;

    xResult = C_GetFunctionList( &pxFunctionList );

    if( xResult == CKR_OK )
    {
        xResult = pxFunctionList->C_GetSlotList( CK_TRUE, /* Token Present. */
                                                 NULL,    /* We just want to know how many slots there are. */
                                                 pxSlotCount );
    }

    if( xResult == CKR_OK )
    {
        /* Allocate memory for the slot list. */
        pxSlotId = pvPortMalloc( sizeof( CK_SLOT_ID ) * ( *pxSlotCount ) );
        *ppxSlotId = pxSlotId;

        if( *ppxSlotId == NULL )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = pxFunctionList->C_GetSlotList( CK_TRUE, pxSlotId, pxSlotCount );
    }

    if( xResult != CKR_OK )
    {
        vPortFree( pxSlotId );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/* @brief Open a PKCS #11 Session.
 *
 *  \param[out] pxSession   Pointer to the session handle to be created.
 *  \param[out] xSlotId     Slot ID to be used for the session.
 *
 *  \return CKR_OK or PKCS #11 error code. (PKCS #11 error codes are positive).
 */
CK_RV prvOpenSession( CK_SESSION_HANDLE * pxSession,
                      CK_SLOT_ID xSlotId )
{
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR pxFunctionList;

    xResult = C_GetFunctionList( &pxFunctionList );

    if( xResult == CKR_OK )
    {
        xResult = pxFunctionList->C_OpenSession( xSlotId,
                                                 CKF_SERIAL_SESSION | CKF_RW_SESSION,
                                                 NULL,               /* Application defined pointer. */
                                                 NULL,               /* Callback function. */
                                                 pxSession );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

CK_RV xInitializePkcs11Session( CK_SESSION_HANDLE * pxSession )
{
    CK_RV xResult;
    CK_SLOT_ID * pxSlotId;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_ULONG xSlotCount;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Initialize the PKCS #11 module. */
    if( xResult == CKR_OK )
    {
        xResult = pxFunctionList->C_Initialize( NULL );
    }

    /* Get a list of slots available. */
    if( ( xResult == CKR_OK ) || ( xResult == CKR_CRYPTOKI_ALREADY_INITIALIZED ) )
    {
        xResult = prvGetSlotList( &pxSlotId, &xSlotCount );
    }

    /* Open a PKCS #11 session. */
    if( xResult == CKR_OK )
    {
        /* We will take the first slot available.
         * If your application has multiple slots, insert logic
         * for selecting an appropriate slot here.
         */
        xResult = prvOpenSession( pxSession, pxSlotId[ 0 ] );

        /* Free the memory allocated by prvGetSlotList. */
        vPortFree( pxSlotId );
    }

    xResult = pxFunctionList->C_Login( *pxSession, 
                                        CKU_USER, 
                                        configPKCS11_DEFAULT_USER_PIN, 
                                        sizeof( configPKCS11_DEFAULT_USER_PIN ) - 1 );

    return xResult;
}

/*-----------------------------------------------------------*/

CK_RV xFindObjectWithLabel( CK_SESSION_HANDLE xSession,
                            const char * pcLabelName,
                            CK_OBJECT_HANDLE_PTR pxHandle )
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;
    CK_FUNCTION_LIST_PTR pxFunctionList;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = strlen( pcLabelName ) + 1;
        xTemplate.pValue = ( char * ) pcLabelName;
        xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xFindInit = CK_TRUE;
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 pxHandle,
                                                 1,
                                                 &ulCount );
    }

    if( CK_TRUE == xFindInit )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/*-----------------------------------------------------------*/
