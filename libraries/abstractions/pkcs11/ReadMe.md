# PKCS #11 Cryptographic Libraries #
## Introduction ##
FreeRTOS uses the PKCS #11 crypto API standard for operations involving asymmetric keys, random number generation, and hashing. Use cases include TLS certificate client authentication and code-sign signature verification.  We use PKCS #11 because:
* It is an open standard that has benefitted from 20 years of production use, 
* It is familiar to open-source developers, and 
* It has specifically been designed to allow crypto-aware apps to be agnostic of the underlying implementation (i.e. whether hardware or software based, and regardless of the choice of crypto algorithm). 
 
FreeRTOS offers two distinct options for silicon partners to integrate with our PKCS #11-aware libraries:
 
* Provide a complete PKCS #11 module implementation (although even with this approach, in order to ease the porting burden, we only require a subset of the API – see below)
* Implement a platform abstraction layer (PAL) 

## When to implement a complete PKCS #11 module ##
If you wish to integrate a crypto engine that allows for handle-based private key operations to take place outside of main MCU RAM, such that the private key itself is not exposed to the main MCU app, then you must implement a “monolithic” PKCS #11 module. The benefit of this approach is that it allows the app to take advantage of offload crypto while mitigating the threats of private key cloning and theft. The cost of this approach is the additional effort required to implement a complete PKCS #11 module (i.e. as opposed to a PAL, described below).
 
In order to keep FreeRTOS as lean as possible, we use subset of PKCS #11. That subset is defined by our qualification tests, https://github.com/aws/amazon-freertos/blob/master/libraries/abstractions/pkcs11/test/iot_test_pkcs11.c. Implementers are free to integrate more than our required subset of PKCS #11, but it is optional to do so. 
 
The PKCS #11 API functions required by FreeRTOS are described in the following table. 

FreeRTOS Library | Required PKCS #11 API Family
----------------------- | ----------------------------
Any | Initialize, Finalize, Open/Close Session, GetSlotList, Login
Provisioning Demo | GenerateKeyPair, CreateObject, DestroyObject, InitToken, GetTokenInfo
TLS | Random, Sign, FindObject, GetAttributeValue
FreeRTOS+TCP | Random, Digest
OTA | Verify, Digest, FindObject, GetAttributeValue
 
The PKCS #11 interface is defined in the open-standard documentation available here http://docs.oasis-open.org/pkcs11/pkcs11-base/v2.40/os/pkcs11-base-v2.40-os.html.

## When to implement a PAL ##
If you do not fall into the above category, then we recommend that you implement a PAL (i.e. rather than a full PKCS #11 module). The PAL approach basically amounts to a highly constrained shim for your flash driver. The purpose of the PAL is to allow our software-based reference implementation of PKCS #11 to access onboard flash for storage of persisted private keys and certificates.
 
If you will also be completing a first-time MCU port for FreeRTOS, you may wish to choose the PAL approach even if your embedded solution also offers asymmetric crypto offload. That approach is likely to allow you to get your MCU port qualified for FreeRTOS more quickly. You can fast-follow with the custom PKCS #11 module. This is especially true if you have not previously implemented a PKCS #11 module.
 
The PAL interface is defined as follows.
```
CK_RV PKCS11_PAL_Initialize( void );

CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( 
    CK_ATTRIBUTE_PTR pxLabel,
    CK_BYTE_PTR pucData,
    CK_ULONG ulDataSize );
 
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( 
    CK_BYTE_PTR pxLabel,
    CK_ULONG usLength );
 
CK_RV PKCS11_PAL_GetObjectValue(
     CK_OBJECT_HANDLE xHandle,
     CK_BYTE_PTR * ppucData,
     CK_ULONG_PTR pulDataSize,
     CK_BBOOL * pIsPrivate );
 
void PKCS11_PAL_GetObjectValueCleanup( 
    CK_BYTE_PTR pucData,
    CK_ULONG ulDataSize );
    

CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle );
```
A stub of the PAL interface can be found at https://github.com/aws/amazon-freertos/blob/master/vendors/vendor/boards/board/ports/pkcs11/core_pkcs11_pal.c.
