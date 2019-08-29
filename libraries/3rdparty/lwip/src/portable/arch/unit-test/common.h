#include "tap.h"

struct sys_mbox {
    void *xMbox;
    void *xTask;
};
typedef struct sys_mbox sys_mbox_t;

typedef unsigned u32_t;

typedef int err_t;
typedef int BaseType_t;
typedef int portBASE_TYPE;

typedef void * QueueHandle_t;
typedef void * TaskHandle_t;

#define UNDEFINED 0xcafecafe

#define pdTRUE  1
#define pdFALSE 0

#define ERR_OK  0
#define ERR_MEM 1
#define ERR_ASSERT 255

#define SYS_ARCH_TIMEOUT 99

#define portMAX_DELAY 999
#define portTICK_PERIOD_MS 10

int critical;
#define post_crit_hook()        /* initially empty - redefine as needed */
#define taskENTER_CRITICAL()    do { critical++; } while (0)
#define taskEXIT_CRITICAL()     do { critical--; post_crit_hook(); } while (0)
#define critReset()             do { critical = 0; } while (0)
