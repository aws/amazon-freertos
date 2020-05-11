#include "task_control.h"
#include <stdlib.h>

struct task
{
    pthread_t tid;
};

struct task * task_create( task_callback tc,
                           void * arg )
{
    int ret;
    struct task * t = malloc( sizeof( struct task ) );

    ret = pthread_create( &t->tid, NULL, ( void *( * )( void * ) )tc, arg );

    if( ret != 0 )
    {
        free( t );
        return NULL;
    }

    return t;
}

void task_join( struct task * t )
{
    pthread_join( t->tid, NULL );
}

void task_kill( struct task * t )
{
    free( t );
    pthread_exit( NULL );
}
