/*
 * We set the values of relevant global
 * variables to nondeterministic values
 */
void vSetGlobalVariables( void )
{
	xSchedulerRunning = nondet();
	uxSchedulerSuspended = nondet();
}
