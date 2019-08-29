/* minimal TAP: test anything protocol */
/* https://testanything.org/ */

#include <stdio.h>
#include <stdlib.h>

int _plan;      /* number of tests */
int _ok_cnt;    /* number of passing tests */
int _index;     /* ordinal of current test */
int _verbose;   /* flag: show all expressions */

/* how many tests should run? */
/* set verbose flag if environment var VERBOSE exists */
#define plan(x)  do {                               \
    _plan = (x);                                    \
    if (_plan)                                      \
        printf("%d..%d\n", 1, _plan);               \
    if (getenv("VERBOSE"))                          \
        _verbose = 1;                               \
} while (0)

/* test expression x and keep count of true values */
/* output diagnostic if false or if VERBOSE is set */
#define ok(x) do {                                  \
    char *msg = "not ok";                           \
    _index++;                                       \
    int pass = (x);                                 \
    if (pass) {                                     \
        _ok_cnt++;                                  \
        msg += 4;                                   \
    }                                               \
    if (_verbose || !pass)                          \
        printf("%s %d - %s:%d\t%s\n", msg, _index,  \
                __FILE__, __LINE__, #x);            \
    else                                            \
        printf("%s %d\n", msg, _index);             \
} while (0)

/* summarize test run, and return an exit value for main() */
#define tally() ({                                  \
    if (!_plan)                                     \
        plan(_index);                               \
    int pass = _plan == _ok_cnt;                    \
    printf("%s\n", pass ? "PASSED" : "FAILED");     \
    pass ? EXIT_SUCCESS : EXIT_FAILURE;             \
})
