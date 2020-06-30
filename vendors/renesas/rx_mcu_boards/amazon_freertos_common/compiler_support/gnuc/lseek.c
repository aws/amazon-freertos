#include <sys/unistd.h>

/* Both sscanf() and fscanf() use common std() of newlib/libc/stdio/findfp.c internally
 * and the std() requires the address of __sseek() which calls _lseek_r() --> lseek().
 * Because of this, lseek() is necessary for not only fscanf() but also sscanf(). */

_off_t lseek(int fd, _off_t pos, int whence)
{
    (void)fd;
    (void)pos;
    (void)whence;

    return (_off_t) - 1;
}
