#include "common.h"

int keep;
void *mbox;
#define xQueueCreate(x, y) ({ keep = (x); mbox; })
#define xqcReset() do { \
    keep = -UNDEFINED;  \
    mbox = &mbox;       \
} while (0)

#include "sys_mbox_new.c"

int main(void) {
    int err;
    sys_mbox_t foo, fooDflt = { &foo, &foo };

    #define reset() do {    \
        xqcReset();         \
        foo = fooDflt;      \
    } while (0)

    plan(8);

    /* happy path */
    reset();
    err = sys_mbox_new(&foo, 123);
    ok(err == ERR_OK);
    ok(keep == 123);
    ok(foo.xTask == NULL);
    ok(foo.xMbox == &mbox);

    /* xQueueCreate returns NULL */
    reset();
    mbox = NULL;
    err = sys_mbox_new(&foo, 234);
    ok(err == ERR_MEM);
    ok(keep == 234);
    ok(foo.xTask == &foo); /* unchanged */
    ok(foo.xMbox == &foo); /* unchanged */

    return tally();
}
