#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool cy_isreadable( void* serial );

int cy_read( void* serial );

#ifdef __cplusplus
}
#endif
