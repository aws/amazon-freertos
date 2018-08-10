/*
 * Amazon FreeRTOS Device Defender Agent V1.0.0
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
#include "aws_cbor.h"
#include "aws_cbor_print.h"
#include "aws_defender_internals.h"
#include "unity_fixture.h"

TEST_GROUP( aws_defender_report_header );
TEST_SETUP( aws_defender_report_header )
{
}

TEST_TEAR_DOWN( aws_defender_report_header )
{
}

TEST_GROUP_RUNNER( aws_defender_report_header )
{
    RUN_TEST_CASE( aws_defender_report_header,
                   GetHeader_report_id_increments_with_each_call_to_grab_the_header );
    RUN_TEST_CASE(
        aws_defender_report_header, GetHeader_version_matches_metrics_version );
}

TEST( aws_defender_report_header,
      GetHeader_report_id_increments_with_each_call_to_grab_the_header )
{
    cbor_handle_t first_header = DEFENDER_GetHeader();
    cbor_int_t first_id =
        CBOR_FromKeyReadInt( first_header, DEFENDER_report_id_tag );

    CBOR_Delete( &first_header );

    cbor_handle_t second_header = DEFENDER_GetHeader();
    cbor_int_t second_id =
        CBOR_FromKeyReadInt( second_header, DEFENDER_report_id_tag );
    CBOR_Delete( &second_header );

    TEST_ASSERT_GREATER_THAN_INT( first_id, second_id );
}

TEST( aws_defender_report_header, GetHeader_version_matches_metrics_version )
{
    cbor_handle_t header = DEFENDER_GetHeader();
    char * version = CBOR_FromKeyReadString( header, DEFENDER_version_tag );

    CBOR_Delete( &header );
    TEST_ASSERT_EQUAL_STRING( DEFENDER_METRICS_VERSION, version );
    free( version );
}
