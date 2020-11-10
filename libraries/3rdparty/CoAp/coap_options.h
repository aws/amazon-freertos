/*******************************************************************************
 * Copyright (c)  2015  Dipl.-Ing. Tobias Rohde, http://www.lobaro.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#ifndef COAP_OPTIONS_H_
#define COAP_OPTIONS_H_



//on addition of an option also change coap_options.C (array initializer) and count
//Proxy-Uri
extern size_t KNOWN_OPTIONS_COUNT;
extern uint16_t KNOWN_OPTIONS[];

#define OPT_FLAG_CRITICAL 	(1<<0)
#define OPT_FLAG_UNSAFE	  	(1<<1)
#define OPT_FLAG_NOCACHEKEY (1<<2)
#define OPT_FLAG_REPEATABLE	(1<<3)

#define MAX_OPTION_VALUE_SIZE (1034) //used in option "Proxy-Uri"

#define OPTION_PAYLOAD_MARKER (0xFF)

CoAP_Result_t parse_OptionsFromRaw(uint8_t* srcArr, uint16_t srcLength, uint8_t** pPayloadBeginInSrc, CoAP_option_t** pOptionsListBegin);
CoAP_Result_t pack_OptionsFromList(uint8_t* pDestArr, uint16_t* pBytesWritten, CoAP_option_t* pOptionsListBegin);
uint16_t  CoAP_NeededMem4PackOptions(CoAP_option_t* pOptionsListBegin);

CoAP_option_t* CoAP_FindOptionByNumber(CoAP_Message_t* msg, uint16_t number);
CoAP_Result_t CoAP_AppendUintOptionToList(CoAP_option_t** pOptionsListBegin, uint16_t OptNumber, uint32_t val); 
CoAP_Result_t CoAP_AppendOptionToList(CoAP_option_t** pOptionsListBegin, uint16_t OptNumber, uint8_t* buf, uint16_t length);
CoAP_Result_t CoAP_CopyOptionToList(CoAP_option_t** pOptionsListBegin, CoAP_option_t* OptToCopy);
CoAP_Result_t CoAP_RemoveOptionFromList(CoAP_option_t** pOptionListStart, CoAP_option_t* pOptionToRemove);
CoAP_Result_t CoAP_FreeOptionList(CoAP_option_t* pOptionsListBegin);

CoAP_Result_t CoAP_GetUintFromOption(const CoAP_option_t* pOption, uint32_t* value);

bool CoAP_OptionsAreEqual(CoAP_option_t* OptA, CoAP_option_t* OptB);
uint16_t CoAP_CheckForUnknownCriticalOption(CoAP_option_t* pOptionsListBegin);

void CoAP_printOptionsList(CoAP_option_t* pOptListBegin);

#endif /* COAP_OPTIONS_H_ */
