#include <pthread.h>

struct task;

typedef void ( * task_callback )( void * );

struct task * task_create( task_callback tc,
                           void * arg );

void task_join( struct task * t );

void task_kill( struct task * t );
