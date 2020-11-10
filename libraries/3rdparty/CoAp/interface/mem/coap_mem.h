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
//Taken from http://www.fourmilab.ch/bget/
//"BGET is in the public domain. You can do anything you like with it."
//Thank you Mr. Walker for your great work!
#include "../coap_interface.h"
#ifndef __COAP_MEM__
#define __COAP_MEM__

	void coap_mem_init(uint8_t* pMemoryArea, int16_t size);
	void coap_mem_release(void* buf);		//release memory
	void* coap_mem_get(int16_t size); 		//request memory (4 byte header + alignment = overhead of allocator)
	void* coap_mem_get0(int16_t size); 		//get zero initialized buf mem
	int32_t coap_mem_size(uint8_t* buf); 	//get size of user buffer without memory allocator header
	uint8_t* coap_mem_buf_lowEnd();			//lowEnd of total memory area under control of memory allocator
	uint8_t* coap_mem_buf_highEnd();		//highEnd of total memory area under control of memory allocator
	void coap_mem_stats();					//Display/Print some debug information for amount of memory usage

	#endif


