#if !defined(MBEDTLS_USER_CONFIG_FILE)
#define MBEDTLS_USER_CONFIG_FILE "mbedtls_user_config.h"
#endif

/* Avoid CC-RX's compiler warning message 'M0520193: Zero used for
 * undefined preprocessing identifier "XXXX"' in CC-RX's math.h */
#if !defined(_FEVAL)
#define _FEVAL 0
#endif

#if !defined(_HAS_C9X_FAST_FMA)
#define _HAS_C9X_FAST_FMA 0
#endif

/* Workaround for incompatibility of basic macro definitions. */
#if !defined(__RX__)
#define __RX__ 1
#endif

#if defined(__LIT)
#if !defined(__RX_LITTLE_ENDIAN__)
#define __RX_LITTLE_ENDIAN__ 1
#endif
#if !defined(__ORDER_LITTLE_ENDIAN__)
#define __ORDER_LITTLE_ENDIAN__ 1234
#endif
#if !defined(__ORDER_BIG_ENDIAN__)
#define __ORDER_BIG_ENDIAN__ 4321
#endif
#if !defined(__BYTE_ORDER__)
#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#endif
#elif defiend(__BIG)
#if !defined(__RX_BIG_ENDIAN__)
#define __RX_BIG_ENDIAN__ 1
#endif
#if !defined(__ORDER_LITTLE_ENDIAN__)
#define __ORDER_LITTLE_ENDIAN__ 1234
#endif
#if !defined(__ORDER_BIG_ENDIAN__)
#define __ORDER_BIG_ENDIAN__ 4321
#endif
#if !defined(__BYTE_ORDER__)
#define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#endif
#endif

#if defined(__FPU)
#if !defined(__RX_FPU_INSNS__)
#define __RX_FPU_INSNS__ 1
#endif
#endif

#if defined(__RXV2)
#if !defined(__RXv2__)
#define __RXv2__ 1
#endif
#endif
