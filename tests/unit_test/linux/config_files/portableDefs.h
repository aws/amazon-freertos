#ifndef PORTABLE_DEFS_H_
#define PORTABLE_DEFS_H_

#include </usr/include/errno.h>

/* redefine EWOULDBLOCK to be different than EAGAIN to broaden the test coverage
 * and cover more cases
 */
#undef EWOULDBLOCK
#define EWOULDBLOCK    300

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portable.h"


#endif /* ifndef PORTABLE_DEFS_H_ */
