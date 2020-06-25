#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cy_result.h>

#define CY_OBJSTORE_MODULE                         (0x01D0)
#define CY_OBJSTORE_PARTIAL_READ_ONLY              CY_RSLT_CREATE(CY_RSLT_TYPE_WARNING, CY_OBJSTORE_MODULE, 0)
#define CY_OBJSTORE_NOT_INITIALIZED                CY_RSLT_CREATE(CY_RSLT_TYPE_INFO, CY_OBJSTORE_MODULE, 0)
#define CY_OBJSTORE_EEPROM_FAILED                  CY_RSLT_CREATE(CY_RSLT_TYPE_INFO, CY_OBJSTORE_MODULE, 1)
#define CY_OBJSTORE_NO_SUCH_OBJECT                 CY_RSLT_CREATE(CY_RSLT_TYPE_INFO, CY_OBJSTORE_MODULE, 2)
#define CY_OBJSTORE_STORE_UNFORMATTED              CY_RSLT_CREATE(CY_RSLT_TYPE_INFO, CY_OBJSTORE_MODULE, 3)
#define CY_OBJSTORE_INVALID_KEY                    CY_RSLT_CREATE(CY_RSLT_TYPE_INFO, CY_OBJSTORE_MODULE, 4)
#define CY_OBJSTORE_NO_SPACE                       CY_RSLT_CREATE(CY_RSLT_TYPE_INFO, CY_OBJSTORE_MODULE, 4)

typedef uint8_t Key_t ;

cy_rslt_t cy_objstore_is_initialized() ;
cy_rslt_t cy_objstore_initialize(bool redundant, int wearleveling) ;
cy_rslt_t cy_objstore_erase() ;
cy_rslt_t cy_objstore_format() ;
cy_rslt_t cy_objstore_store_object(Key_t key, const uint8_t *data, uint32_t size) ;
cy_rslt_t cy_objstore_find_object(Key_t key, uint32_t *index, uint32_t *objsize) ;
cy_rslt_t cy_objstore_read_object(Key_t key, uint8_t *data, uint32_t objsize) ;
cy_rslt_t cy_objstore_delete_object(Key_t key) ;


