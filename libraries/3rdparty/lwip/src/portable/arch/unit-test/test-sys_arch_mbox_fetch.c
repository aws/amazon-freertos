#include "common.h"

/* invariant to test upon exit of taskEXIT_CRITICAL() */
/* initially disabled for early failure tests */
int run_hook;
#undef  post_crit_hook
#define post_crit_hook()  do {          \
    if (run_hook)                       \
        ok(pxMailBox->xTask == task);   \
} while (0)

void *task;
#define xTaskGetCurrentTaskHandle() ({ task; })
#define xtgcReset()     do { task = &task; } while (0)

int xqrRet;
void *xqrSaveHandle;
void *xqrSaveBuffer;
int xqrSaveTmout;
int xqrCallCount;
#define xQueueReceive(handle, buffer, tmout)  ({ \
    xqrSaveHandle = (handle);           \
    xqrSaveBuffer = (buffer);           \
    xqrSaveTmout  = (tmout);            \
    if (!xqrRet && ++xqrCallCount == 3) \
        xqrRet = pdTRUE;                \
    xqrRet;                             \
})
#define xqrReset()  do {        \
    xqrCallCount = 0;           \
    xqrSaveHandle = NULL;       \
    xqrSaveBuffer = NULL;       \
    xqrSaveTmout = UNDEFINED;   \
    xqrRet = pdFALSE;           \
} while (0)

int xInsideISR;
#define configASSERT(x)  do { if (!(x)) return ERR_ASSERT; } while (0)

#include "sys_arch_mbox_fetch.c"

int main(void) {
    unsigned ret;
    void *mbox;
    sys_mbox_t foo, fooDflt = { &mbox, NULL };
    void *buffer;

    #define reset() do {    \
        critReset();        \
        xtgcReset();        \
        xqrReset();         \
        critical = 0;       \
        xInsideISR = 0;     \
        foo = fooDflt;      \
        ret = UNDEFINED;    \
    } while (0)
    reset();

    /* invariant sets are cumulative */
    #define invariants(x) do {                      \
        switch (x) {                                \
            case 2:                                 \
                ok(xqrSaveHandle == &mbox);         \
                ok(xqrSaveBuffer == &buffer);       \
                ok(foo.xTask == NULL);              \
            case 1:                                 \
                ok(critical == 0);                  \
        }                                           \
        reset();                                    \
    } while (0)

    plan(42);

    /* trip ISR assertion */
    xInsideISR = 1;
    ret = sys_arch_mbox_fetch(NULL, NULL, 0);
    ok(ret == ERR_ASSERT);
    invariants(1);

    /* bad first arg */
    ret = sys_arch_mbox_fetch(NULL, NULL, 0);
    ok(ret == SYS_ARCH_TIMEOUT);
    invariants(1);

    /* bad struct first element */
    foo.xMbox = NULL;
    ret = sys_arch_mbox_fetch(&foo, NULL, 0);
    ok(ret == SYS_ARCH_TIMEOUT);
    invariants(1);

    /* mbox in use by another */
    foo.xTask = &foo;
    ret = sys_arch_mbox_fetch(&foo, NULL, 0);
    ok(ret == SYS_ARCH_TIMEOUT);
    invariants(1);

    /* bad return from xTaskGetCurrentTaskHandle() */
    task = NULL;
    ret = sys_arch_mbox_fetch(&foo, NULL, 0);
    ok(ret == SYS_ARCH_TIMEOUT);
    invariants(1);

    /* enable post_crit_hook() for remaining cases */
    run_hook = 1;

    /* xQueueReceive fails, no buffer provided */
    ret = sys_arch_mbox_fetch(&foo, NULL, 1);
    ok(xqrSaveTmout == 1/portTICK_PERIOD_MS);
    ok(ret == SYS_ARCH_TIMEOUT);
    invariants(1);

    /* xQueueReceive fails */
    ret = sys_arch_mbox_fetch(&foo, &buffer, 1);
    ok(xqrSaveTmout == 1/portTICK_PERIOD_MS);
    ok(ret == SYS_ARCH_TIMEOUT);
    invariants(2);

    /* xQueueReceive fails, but later succeeds - only for timeout=0 */
    ret = sys_arch_mbox_fetch(&foo, &buffer, 0);
    ok(xqrSaveTmout == portMAX_DELAY);
    ok(ret == 1);
    invariants(2);

    /* happy path, with and without timeout */
    xqrRet = pdTRUE;
    ret = sys_arch_mbox_fetch(&foo, &buffer, 1);
    ok(xqrSaveTmout == 1/portTICK_PERIOD_MS);
    ok(ret == 1);
    invariants(2);

    xqrRet = pdTRUE;
    ret = sys_arch_mbox_fetch(&foo, &buffer, 0);
    ok(xqrSaveTmout == portMAX_DELAY);
    ok(ret == 1);
    invariants(2);

    return tally();
}
