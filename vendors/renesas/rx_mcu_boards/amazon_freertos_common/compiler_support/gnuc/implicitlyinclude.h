#if !defined(MBEDTLS_USER_CONFIG_FILE)
#define MBEDTLS_USER_CONFIG_FILE "mbedtls_user_config.h"
#endif

/* A workaround for incompatibility of basic macro definitions. */
#if !defined(__RX)
#define __RX 1
#endif

#if defined(__RX_LITTLE_ENDIAN__)
/* GNURX defines also #define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__ */
#if !defined(__LIT)
#define __LIT 1
#endif
#elif defined(__RX_BIG_ENDIAN__)
/* GNURX defines also #define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__ */
#if !defined(__BIG)
#define __BIG 1
#endif
#endif

#if defined(__RX_FPU_INSNS__)
#if !defined(__FPU)
#define __FPU 1
#endif
#endif

#if defined(__RXv2__)
#if !defined(__RXV2)
#define __RXV2 1
#endif
#endif
