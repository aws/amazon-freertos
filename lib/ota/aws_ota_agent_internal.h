/*
Amazon FreeRTOS OTA Agent V0.9.3
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

#ifndef _AWS_OTA_AGENT_INTERNAL_H_
#define _AWS_OTA_AGENT_INTERNAL_H_
#include "aws_ota_types.h"
#include "aws_ota_agent.h"

#define kBitsPerByte          8                             /* Number of bits in a byte. This is used by the block bitmap implementation. */
#define kOTA_FileBlockSize    1024                          /* Data section size of the file data block message (excludes the header). */

typedef enum
{
    eIngest_Result_FileComplete = -1,        /* The file transfer is complete and the signature check passed. */
    eIngest_Result_SigCheckFail = -2,        /* The file transfer is complete but the signature check failed. */
    eIngest_Result_FileCloseFail = -3,       /* There was a problem trying to close the receive file. */
    eIngest_Result_NullContext = -4,         /* The specified OTA context pointer is null. */
    eIngest_Result_BadFileHandle = -5,       /* The receive file pointer is invalid. */
    eIngest_Result_UnexpectedBlock = -6,     /* We were asked to ingest a block but weren't expecting one. */
    eIngest_Result_BlockOutOfRange = -7,     /* The received block is out of the expected range. */
    eIngest_Result_BadData = -8,             /* The data block from the server was malformed. */
    eIngest_Result_WriteBlockFailed = -9,    /* The PAL layer failed to write the file block. */
    eIngest_Result_Continue = 0,             /* The block was accepted and we're expecting more. */
} IngestResult_t;

extern IngestResult_t TEST_OTA_prvIngestDataBlock( OTA_FileContext_t * C,
                                                   const char * pacRawMsg,
                                                   u32 iMsgSize );
extern OTA_FileContext_t * TEST_OTA_prvParseJobDocFromJSON( const char * pacRawMsg,
                                                            u32 iMsgLen );

extern bool_t TEST_OTA_prvOTA_Close( OTA_FileContext_t * const C );

#endif /* ifndef _AWS_OTA_AGENT_INTERNAL_H_ */
