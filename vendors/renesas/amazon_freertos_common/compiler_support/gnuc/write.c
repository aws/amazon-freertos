#include <sys/unistd.h>

/* Both sscanf() and fscanf() use common std() of newlib/libc/stdio/findfp.c internally
 * and the std() requires the address of __swrite() which calls _write_r() --> write().
 * Because of this, write() is necessary for not only fscanf() but also sscanf(). */

int write(int fd, _CONST _PTR buf, size_t cnt)
{
    (void)fd;
    (void)buf;
    (void)cnt;

    return -1;
}
