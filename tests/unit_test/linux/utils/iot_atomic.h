uint32_t Atomic_Decrement_u32( uint32_t * val )
{
    return __sync_fetch_and_sub( val, 1 );
}

uint32_t Atomic_Increment_u32( uint32_t * val )
{
    return __sync_fetch_and_add( val, 1 );
}
