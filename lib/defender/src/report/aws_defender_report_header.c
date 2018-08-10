/*
 * Amazon FreeRTOS Device Defender Agent V1.0.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#include "aws_defender_internals.h"

/*
 * Returns a non-crypto, but random number for initializing the report ID
 * This helps to prevent duplicate report IDs from the same device after POR
 */
static uint32_t prvDEFENDER_ReportIdInit( void );
#ifdef __free_rtos__
    #include "FreeRTOS.h"
    static uint32_t prvDEFENDER_ReportIdInit( void )
    {
        return configRAND32();
    }
#else
    #include <stdlib.h>
    static uint32_t prvDEFENDER_ReportIdInit( void )
    {
        /* Todo: detect platform and use apporpriate PRNG */
        /* e.g. random() on POSIX */
        return rand();
    }
#endif /* ifdef __free_rtos__ */

static int32_t lReportId;
const char * pcDEFENDER_METRICS_VERSION = "1.0";

cbor_handle_t DEFENDER_GetHeader( void )
{
    lReportId = lReportId == 0 ? prvDEFENDER_ReportIdInit() : lReportId;

    cbor_handle_t pxHeader = CBOR_New( 0 );

    CBOR_AssignKeyWithInt( pxHeader, DEFENDER_report_id_tag, ++lReportId );
    CBOR_AssignKeyWithString(
        pxHeader, DEFENDER_version_tag, pcDEFENDER_METRICS_VERSION );

    return pxHeader;
}

int32_t DEFENDER_GetLastReportId( void )
{
    return lReportId;
}
