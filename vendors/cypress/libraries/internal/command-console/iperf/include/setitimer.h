#if !defined(SETITIMER_H) && !defined(HAVE_SETITIMER) && defined(WIN32)
#define SETITIMER_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ITIMER_REAL 0

struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

typedef void SigAlrmFunc();
typedef SigAlrmFunc *SigAlrmFuncPtr;

void setsigalrmfunc( SigAlrmFuncPtr inFunc );

int setitimer(int which, const struct itimerval * value, struct itimerval * ovalue);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* !defined(SETITIMER_H) && !defined(HAVE_SETITIMER) && defined(WIN32) */
