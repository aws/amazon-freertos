/* Not sure why "abort" implementation was disabled. Enable it because tinycbor uses "assert" which calls abort. */

#if 1 /* This file will be removed later because abort() is not used any more. */
/* CC-RX's user define function */
void abort( void );
void abort( void )
{
    for(;;) {}
}
#endif
