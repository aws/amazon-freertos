/*
 * diagnostic.h
 *
 *  Created on: 11.05.2017
 *      Author: Tobias
 */

#ifndef LOBARO_COAP_SRC_DIAGNOSTIC_H_
#define LOBARO_COAP_SRC_DIAGNOSTIC_H_

/*
 * Methods for debugging the stack
 */


void PrintInteractions(CoAP_Interaction_t *pInteractions);
void PrintEndpoint(const NetEp_t* ep);
char* ResultToString(CoAP_Result_t res);

#endif /* LOBARO_COAP_SRC_DIAGNOSTIC_H_ */
