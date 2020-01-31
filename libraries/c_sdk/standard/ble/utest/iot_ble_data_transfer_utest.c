//
// Created by parallels on 1/30/20.
//

#include <stdbool.h>
#include <pthread.h>

#include <unity.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"


/* ==========================  FUNCTION PROTOTYPES  ========================= */
static void initCallbacks();


/* ============================  GLOBAL VARIABLES =========================== */

static uint16_t malloc_free_calls = 0;

/* ==========================  CALLBACK FUNCTIONS =========================== */

/* ============================   UNITY FIXTURES ============================ */
void setUp( void )
{

}

/* called before each testcase */
void tearDown( void )
{

}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return( numFailures > 0 );
}
/* ==========================  Helper functions  ============================ */

/* ======================  TESTING SOCKETS_Send  ============================ */
