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
/*
 * coap_blockwise.h
 *
 *  Created on: 17.11.2014
 *      Author: Tobias
 */

#ifndef SRC_COAP_COAP_BLOCKWISE_H_
#define SRC_COAP_COAP_BLOCKWISE_H_

typedef enum
{
	BLOCK_1=27, //pertains to request payload
	BLOCK_2=23  //pertains to response payload
}CoAP_blockwise_option_type_t;

typedef enum
{
	BLOCK_SIZE_NOT_USED = 0,
	BLOCK_SIZE_16 = 16,
	BLOCK_SIZE_32 = 32,
	BLOCK_SIZE_64 = 64,
	BLOCK_SIZE_128 = 128,
	BLOCK_SIZE_256 = 256,
	BLOCK_SIZE_512 = 512,
	BLOCK_SIZE_1024 = 1024
}CoAP_blockwise_blockSize_t;


#define DEFAULT_SERVER_BLOCK_SIZE BLOCK_SIZE_64

//Blockwise transfers
typedef struct
{
	CoAP_blockwise_option_type_t Type;
	CoAP_blockwise_blockSize_t BlockSize;
	bool MoreFlag;
	uint32_t BlockNum;
}CoAP_blockwise_option_t;


CoAP_Result_t _rom CoAP_SetPayload_CheckBlockOpt(CoAP_Message_t* pMsgReq, CoAP_Message_t* pMsgResp, uint8_t* pPayload, uint16_t payloadTotalSize, bool payloadIsVolatile);

CoAP_Result_t AddBlkOptionToMsg(CoAP_Message_t* msg, CoAP_blockwise_option_t* blkOption);
CoAP_Result_t GetBlock1OptionFromMsg(CoAP_Message_t* msg, CoAP_blockwise_option_t* BlkOption);
CoAP_Result_t GetBlock2OptionFromMsg(CoAP_Message_t* msg, CoAP_blockwise_option_t* BlkOption);

CoAP_Result_t _rom RemoveAllBlockOptionsFromMsg(CoAP_Message_t* msg, CoAP_blockwise_option_type_t Type);
CoAP_Result_t dbgBlkOption(CoAP_blockwise_option_t* blkOption);

int32_t getBlockwiseOffset(uint16_t TotalPayloadSize, CoAP_blockwise_option_t* blkOpt);

#endif /* SRC_COAP_COAP_BLOCKWISE_H_ */
