#include "common.h"

int uqmwRet;
void *uqmwHandle;
#define uxQueueMessagesWaiting(x)  ({ uqmwHandle = (x); uqmwRet; })
#define uqmwReset()         do { uqmwRet = 0; uqmwHandle = &uqmwHandle; } while (0)

void *xtadHandle;
int xtadCallCount;
#define xTaskAbortDelay(x)  do { xtadCallCount++; xtadHandle = (x); } while (0)
#define xtadReset()         do { xtadCallCount = 0; xtadHandle = &xtadHandle; } while (0)

void *vqdHandle;
int vqdCallCount;
#define vQueueDelete(x)     do { vqdCallCount++; vqdHandle = (x); } while (0)
#define vqdReset()          do { vqdCallCount = 0; vqdHandle = &vqdHandle; } while (0)

int assertCount;
#define configASSERT(x)     do { if (!(x)) { assertCount++; return; } } while (0)
#define assertReset()       do { assertCount = 0; } while (0)

#include "sys_mbox_free.c"

int main(void) {
    void *mbox, *task;
    sys_mbox_t foo, fooDflt = { &mbox, &task };

    #define reset() do {    \
        critReset();        \
        uqmwReset();        \
        xtadReset();        \
        vqdReset();         \
        assertReset();      \
        foo = fooDflt;      \
    } while (0)
    reset();

    /* invariant sets are cumulative */
    #define invariants(x) do {      \
        switch (x) {                \
        /* crit section reached */  \
        case 2:                     \
            ok(foo.xMbox == NULL);  \
            ok(assertCount == 0);   \
        /* always */                \
        case 1:                     \
            ok(critical == 0);      \
        }                           \
        reset();                    \
    } while (0)

    plan(38);

    /* NULL arg is NOOP */
    sys_mbox_free(NULL);
    ok(uqmwHandle == &uqmwHandle);
    ok(assertCount == 0);
    ok(xtadCallCount == 0);
    ok(vqdCallCount == 0);
    invariants(1);

    /* trip no-messages assertion */
    uqmwRet = 1;
    sys_mbox_free(&foo);
    ok(uqmwHandle == &mbox);
    ok(assertCount == 1);
    ok(xtadCallCount == 0);
    ok(vqdCallCount == 0);
    invariants(1);

    /* NULL struct members */
    foo = (sys_mbox_t){NULL, NULL};
    sys_mbox_free(&foo);
    ok(uqmwHandle == NULL);
    ok(xtadCallCount == 0);
    ok(vqdCallCount == 0);
    invariants(2);

    /* NULL mbox member */
    foo.xMbox = NULL;
    sys_mbox_free(&foo);
    ok(uqmwHandle == NULL);
    ok(xtadCallCount == 1);
    ok(xtadHandle == &task);
    ok(vqdCallCount == 0);
    invariants(2);

    /* NULL task member */
    foo.xTask = NULL;
    sys_mbox_free(&foo);
    ok(uqmwHandle == &mbox);
    ok(xtadCallCount == 0);
    ok(vqdCallCount == 1);
    ok(vqdHandle == &mbox);
    invariants(2);

    /* happy path */
    sys_mbox_free(&foo);
    ok(uqmwHandle == &mbox);
    ok(xtadCallCount == 1);
    ok(xtadHandle == &task);
    ok(vqdCallCount == 1);
    ok(vqdHandle == &mbox);
    invariants(2);

    return tally();
}
