#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "optiga/optiga_crypt.h"
#include "mbedtls/entropy_poll.h"

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)


int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{

	optiga_lib_status_t status = OPTIGA_LIB_ERROR;

	status = optiga_crypt_random(eTRNG, output, len);
	if ( status !=  OPTIGA_LIB_SUCCESS)
	{
		*olen = 0;
		return 1;
	}

	*olen = len;
	return 0;
}
#endif
