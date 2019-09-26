/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file defines the memory management related macros.
*
*
* \ingroup  grMutualAuth
*
*/

#ifndef _MEMMGMT_H_
#define _MEMMGMT_H_

#include "FreeRTOS.h"

///Malloc function to allocate the heap memory
#define OCP_MALLOC(size)			pvPortMalloc(size)

///Malloc function to allocate the heap memory
#define OCP_CALLOC(block,blocksize)	calloc(block,blocksize)

///To free the allocated memory
#define OCP_FREE(node)				vPortFree(node)

///To copy the data from source to destination 
#define OCP_MEMCPY(dst,src,size)	memcpy(dst,src,size)

///To copy the data from source to destination 
#define OCP_MEMSET(src,val,size)	memset(src,val,size)

#endif /* _MEMMGMT_H_ */

