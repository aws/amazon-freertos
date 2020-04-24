#include <sys/unistd.h>

/* Both sscanf() and fscanf() use common std() of newlib/libc/stdio/findfp.c internally
 * and the std() requires the address of __sread() which calls _read_r() --> read().
 * Because of this, read() is necessary for not only fscanf() but also sscanf(). */

int read(int fd, _PTR buf, size_t cnt)
{
    (void)fd;
    (void)buf;
    (void)cnt;

    return -1;
}
