

#ifndef _ATCA_DEBUG_H
#define _ATCA_DEBUG_H

#include "atca_status.h"

void atca_trace_config(FILE* fp);

ATCA_STATUS atca_trace(ATCA_STATUS status);
ATCA_STATUS atca_trace_msg(ATCA_STATUS status, const char * msg);

#endif /* _ATCA_DEBUG_H */
