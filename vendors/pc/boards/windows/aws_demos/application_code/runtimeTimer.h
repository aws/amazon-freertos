#pragma once
/* Function for intializing runtime timer based on timer task.
   This function must be called before vTaskStartScheduler is called*/
void initializeRuntimeTimer(void);

/* Prints the current runtime of the simulator, based on timer task
   in MINUTES:SECONDS:MILLSECONDS format, with leading 0's*/
void printRuntime(void);