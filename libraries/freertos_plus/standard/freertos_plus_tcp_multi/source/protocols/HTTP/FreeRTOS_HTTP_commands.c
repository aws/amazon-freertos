/*
 * FreeRTOS+TCP Multi Interface Labs Build 180222
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Authors include Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   This is a version of FreeRTOS+TCP that supports multiple network      ***
 ***   interfaces, and includes basic IPv6 functionality.  Unlike the base   ***
 ***   version of FreeRTOS+TCP, THE MULTIPLE INTERFACE VERSION IS STILL IN   ***
 ***   THE LAB.  While it is functional and has been used in commercial      ***
 ***   products we are still refining its design, the source code does not   ***
 ***   yet quite conform to the strict coding and style standards, and the   ***
 ***   documentation and testing is not complete.                            ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact                                  ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
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


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#include "FreeRTOS_HTTP_commands.h"

const struct xWEB_COMMAND xWebCommands[ WEB_CMD_COUNT ] =
{
	{	3,     "GET",		ECMD_GET },
	{	4,    "HEAD",		ECMD_HEAD },
	{	4,    "POST",		ECMD_POST },
	{	3,     "PUT",		ECMD_PUT },
	{	6,  "DELETE",		ECMD_DELETE },
	{	5,   "TRACE",		ECMD_TRACE },
	{	7, "OPTIONS",		ECMD_OPTIONS },
	{	7, "CONNECT",		ECMD_CONNECT },
	{	5,   "PATCH",		ECMD_PATCH },
	{	4,    "UNKN",		ECMD_UNK },
};

const char *webCodename (int aCode)
{
	switch (aCode) {
	case WEB_REPLY_OK:	//  = 200,
		return "OK";
	case WEB_NO_CONTENT:    // 204
		return "No content";
	case WEB_BAD_REQUEST:	//  = 400,
		return "Bad request";
	case WEB_UNAUTHORIZED:	//  = 401,
		return "Authorization Required";
	case WEB_NOT_FOUND:	//  = 404,
		return "Not Found";
	case WEB_GONE:	//  = 410,
		return "Done";
	case WEB_PRECONDITION_FAILED:	//  = 412,
		return "Precondition Failed";
	case WEB_INTERNAL_SERVER_ERROR:	//  = 500,
		return "Internal Server Error";
	}
	return "Unknown";
}
