#pragma once

#include "cy_result.h"
#include "mtb_kvstore.h"

extern mtb_kvstore_t kvstore_obj;

cy_rslt_t kvstore_init(void);
