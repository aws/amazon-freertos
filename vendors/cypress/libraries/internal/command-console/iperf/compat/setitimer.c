#ifdef WIN32

#include "setitimer.h"
#include <stdio.h>

/* Communication area for inter-thread communication */
typedef struct timerCA {
    struct itimerval value;
    HANDLE      event;
    CRITICAL_SECTION crit_sec;
} timerCA;

static timerCA timerCommArea;
static HANDLE timerThreadHandle = INVALID_HANDLE_VALUE;
static SigAlrmFuncPtr sigalrmfunc = NULL;

static DWORD WINAPI timer_thread( LPVOID param )  {
    DWORD waittime = INFINITE;

    for (;;)
    {
        int r = WaitForSingleObjectEx(timerCommArea.event, waittime, FALSE);
        if (r == WAIT_OBJECT_0)
        {
            /* Event signalled from main thread, change the timer */
            EnterCriticalSection(&timerCommArea.crit_sec);
            if (timerCommArea.value.it_value.tv_sec == 0 && timerCommArea.value.it_value.tv_usec == 0) {
                waittime = INFINITE; /* Cancel the interrupt */
            }
            else 
            {
                /* WaitForSingleObjectEx() uses milliseconds, round up */
                waittime = (timerCommArea.value.it_value.tv_usec + 999) / 1000 + timerCommArea.value.it_value.tv_sec * 1000;
            }
            ResetEvent(timerCommArea.event);
            LeaveCriticalSection(&timerCommArea.crit_sec);
        }
        else if (r == WAIT_TIMEOUT)
        {
            /* Timeout expired, signal SIGALRM and turn it off */
            if ( sigalrmfunc != NULL )
                sigalrmfunc();
            waittime = INFINITE;
        }
        else
        {
            /* Should never happen */
        }
    }

    return 0;
} // end timer_thread

void setsigalrmfunc( SigAlrmFuncPtr inFunc ) {
  sigalrmfunc = inFunc;
} // end setsigalrmfunc

/*
 * Win32 setitimer emulation by creating a persistent thread
 * to handle the timer setting and notification upon timeout.
 */
int setitimer(int which, const struct itimerval * value, struct itimerval * ovalue) {
    if (timerThreadHandle == INVALID_HANDLE_VALUE)
    {
        /* First call in this backend, create event and the timer thread */
        timerCommArea.event = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (timerCommArea.event == NULL)
            printf("failed to create timer event: %d\n", (int) GetLastError());

        memset(&timerCommArea.value, 0, sizeof(struct itimerval));

        InitializeCriticalSection(&timerCommArea.crit_sec);

        timerThreadHandle = CreateThread(NULL, 0, timer_thread, NULL, 0, NULL);
        if (timerThreadHandle == INVALID_HANDLE_VALUE)
            printf("failed to create timer thread: %d\n", (int) GetLastError());
    }

    /* Request the timer thread to change settings */
    EnterCriticalSection(&timerCommArea.crit_sec);
    if (ovalue)
        *ovalue = timerCommArea.value;
    timerCommArea.value = *value;
    LeaveCriticalSection(&timerCommArea.crit_sec);
    SetEvent(timerCommArea.event);

    return 0;
} // end setitimer

#endif /* WIN32 */
