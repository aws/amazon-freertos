#include <sys/unistd.h>

/* Both sscanf() and fscanf() use common std() of newlib/libc/stdio/findfp.c internally
 * and the std() requires the address of __sclose() which calls _close_r() --> close().
 * Because of this, close() is necessary for not only fscanf() but also sscanf(). */

int close(int fd)
{
    (void)fd;

    return -1;
}
