/** @file fs.h
*
*  @brief Interface to File System
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/


#ifndef __FS__H__
#define __FS__H__

#include <stddef.h>

/* the file type is opaque to the caller. */
typedef int *file;

#define FS_EOF -1
#define FS_EINVAL -2
#define FS_EUNIMPLEMENTED -3
#define FS_ENOENT -4
#define FS_EMFILE -5
#define FS_EROFS -6
#define FS_EOVERFLOW -6

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

struct fs {
	int fs_errno;
	file *(* fopen)(struct fs *fs, const char *path, const char *mode);
	int (* fclose)(file *f);
	size_t (* fread)(void *ptr, size_t size, size_t nmemb, file *f);
	size_t (* fwrite)(const void *ptr, size_t size, size_t nmemb, file *f);
	long (* ftell)(file *f);
	int (* fseek)(file *f, long offset, int whence);
};

#endif /* __FS__H__ */
