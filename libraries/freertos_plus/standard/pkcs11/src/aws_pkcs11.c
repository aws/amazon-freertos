#include "aws_pkcs11.h"
#include "pkcs11.h"
#include "aws_pkcs11_config.h"
#include "FreeRTOS.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"

/* C runtime includes. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
    CK_SLOT_ID * pxSlotId= NULL;

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
                                                 NULL, /* Application defined pointer. */
                                                 NULL, /* Callback function. */
                                                 pxSession );
    }

    return xResult;
}

/*-----------------------------------------------------------*/
#ifdef CreateMutex
    #undef CreateMutex
    #define CreateMutex    CreateMutex /* This is a hack because CreateMutex is redefined to CreateMutexW in synchapi.h in windows. :/ */
#endif

CK_RV xInitializePKCS11( void )
{
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_C_INITIALIZE_ARGS xInitArgs;

    xInitArgs.CreateMutex = NULL;
    xInitArgs.DestroyMutex = NULL;
    xInitArgs.LockMutex = NULL;
    xInitArgs.UnlockMutex = NULL;
    xInitArgs.flags = CKF_OS_LOCKING_OK;
    xInitArgs.pReserved = NULL;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Initialize the PKCS #11 module. */
    if( xResult == CKR_OK )
    {
        xResult = pxFunctionList->C_Initialize( &xInitArgs );
    }

    return xResult;
}

CK_RV xInitializePkcs11Session( CK_SESSION_HANDLE * pxSession )
{
    CK_RV xResult;
    CK_SLOT_ID * pxSlotId = NULL;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_ULONG xSlotCount;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Initialize the module. */
    if( xResult == CKR_OK )
    {
        xResult = xInitializePKCS11();
        if( xResult == CKR_CRYPTOKI_ALREADY_INITIALIZED )
        {
            xResult = CKR_OK;
        }
    }

    /* Get a list of slots available. */
    if( xResult == CKR_OK )
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

    if( xResult == CKR_OK && pxFunctionList->C_Login != NULL )
    {
        xResult = pxFunctionList->C_Login( *pxSession,
                                           CKU_USER,
                                           ( CK_UTF8CHAR_PTR )configPKCS11_DEFAULT_USER_PIN,
                                           sizeof( configPKCS11_DEFAULT_USER_PIN ) - 1 );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/* @brief Finds an object with a given label if it exists.
 *
 *   This function wraps C_FindObjectsInit, C_FindObjects, and C_FindObjectsFinal.
 *
 *   \param[in] xSession         A valid PKCS #11 session.
 *   \param[in] pcLabelName      The label of the object to be found.
 *   \param[out] pxHandle        Pointer to the handle of the found object,
 *                               or 0 if no object is found.
 *   \return CKR_OK if PKCS #11 calls were successful.  PKCS #11
 *   error code if not.
 *
 *   \note This function returns CKR_OK even if an object with the given
 *   CKA_LABEL is not found.  It is critical that functions verify that
 *   the object handle value is not equal to 0 (the invalid handle)
 *   before attempting to use the handle.
 */
CK_RV xFindObjectWithLabelAndClass( CK_SESSION_HANDLE xSession,
                                    const char * pcLabelName,
                                    CK_OBJECT_CLASS xClass,
                                    CK_OBJECT_HANDLE_PTR pxHandle )
{
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_ATTRIBUTE xTemplate[ 2 ] =
    {
        { CKA_LABEL, ( char * ) pcLabelName, strlen( pcLabelName ) },
        { CKA_CLASS, &xClass,                sizeof( CK_OBJECT_CLASS ) }
    };

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xResult = pxFunctionList->C_FindObjectsInit( xSession, xTemplate, sizeof( xTemplate ) / sizeof( CK_ATTRIBUTE ) );
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

    if( ulCount == 0 )
    {
        *pxHandle = pkcs11INVALID_OBJECT_HANDLE;
    }

    return xResult;
}

void vAppendSHA256AlgorithmIdentifierSequence( uint8_t * x32ByteHashedMessage,
                                               uint8_t * x51ByteHashOidBuffer )
{
    uint8_t xOidSequence[] = pkcs11STUFF_APPENDED_TO_RSA_SIG;
    memcpy( x51ByteHashOidBuffer, xOidSequence, sizeof( xOidSequence ) );
    memcpy( &x51ByteHashOidBuffer[ sizeof( xOidSequence ) ], x32ByteHashedMessage, 32 );
    return;
}

/*-----------------------------------------------------------*/
