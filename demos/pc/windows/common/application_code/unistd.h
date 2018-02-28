/*
 * Amazon FreeRTOS V1.2.2
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef _UNISTD_H
#define _UNISTD_H    1

#include <stdlib.h>
#include <io.h>
/*#include <getopt.h> / * getopt at: https://gist.github.com/ashelly/7776712 * / */
/*#include <process.h> / * for getpid() and the exec..() family * / */
/*#include <direct.h> / * for _getcwd() and _chdir() * / */

#define srandom          srand
#define random           rand

/* Values for the second argument to access.
 * These may be OR'd together.  */
#define R_OK             4 /* Test for read permission.  */
#define W_OK             2 /* Test for write permission.  */
/*#define   X_OK    1       / * execute permission - unsupported in windows* / */
#define F_OK             0 /* Test for existence.  */

#define access           _access
#define dup2             _dup2
#define execve           _execve
#define ftruncate        _chsize
#define unlink           _unlink
#define fileno           _fileno
#define getcwd           _getcwd
#define chdir            _chdir
#define isatty           _isatty
#define lseek            _lseek
/* read, write, and close are NOT being #defined here, because while there are file handle specific versions for Windows, they probably don't work for sockets. You need to look at your app and consider whether to call e.g. closesocket(). */

#define ssize_t          int

#define STDIN_FILENO     0
#define STDOUT_FILENO    1
#define STDERR_FILENO    2
/* should be in some equivalent to <sys/types.h> */
typedef __int8             int8_t;
typedef __int16            int16_t;
typedef __int32            int32_t;
typedef __int64            int64_t;
typedef unsigned __int8    uint8_t;
typedef unsigned __int16   uint16_t;
typedef unsigned __int32   uint32_t;
typedef unsigned __int64   uint64_t;

#endif /* unistd.h  */
