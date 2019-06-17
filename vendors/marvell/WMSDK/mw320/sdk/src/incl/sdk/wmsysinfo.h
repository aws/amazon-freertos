/*
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

/*! \file wmsysinfo.h
 *  \brief System Info Utilities
 *
 *  Here is the sysinfo usage:
 *  sysinfo [options] service name [command]
 *  where service name can be one of the following:
 *  [thread|socket|netstat|heap|stats|mdns|-h]
 *
 *  sysinfo is a diagnostic utility
 *
 *  The following services are supported:
 *
 *  netstat: Dump info about network stack.
 *          It can be used as follows:
 *          sysinfo netstat link/ip/tcp/udp
 *          Note: All available info about network stack is displayed if no
 *          argument is passed to netstat
 *
 *  thread: Dump info about operating system threads. In addition to
 *          dumping info, the 'stackmark' command writes a
 *          pattern to the unused portion of stacks' threads.
 *          After marking the stacks, dumping the info will
 *          report the highwater mark of the stack.
 *          This is useful for detecting stack overruns.
 *
 *  socket: Dump statistics of socket descriptor usage including
 *          available sockets, used sockets, tcp sockets etc
 *
 *  stats:  Dump statistics related to the WMSDK.
 *
 *  heap:   Dump the heap metadata in this following format
 *          HST:next-free-block:prev-block:block-size:block-type
 *          HST             : A tag to separate other logs from this log
 *          next-free-block : The address of the block which is free.
 *                          This list is sorted by block size - lowest
 *                          size block first
 *          prev-block      : Address of the previous block in the serial list
 *          block-size      : Size of the current block
 *          block-type      : The type of the block:
 *                          X: First block. This is not a part of the heap
 *                          and points to first real block in this heap
 *                          F: Free block
 *                          A: Allocated block
 *
 *  Options:
 *  -h              Print this message
 *  -n name         Only operate on the instance of the service
 *                  with this name.  E.g., 'sysinfo -n foo thread'
 *                  will only dump info for the foo thread.
 */

#ifndef _WMSYSINFO_H_
#define _WMSYSINFO_H_

#include <string.h>

/** Initialize System Information Utility
 *
 * This function registers system info CLI commands like 'sysinfo', 'memdump'
 * and 'memwrite' that can be used to query information about the various
 * subsystems.
 *
 * \return WM_SUCCESS on success, error otherwise
 */
int sysinfo_init(void);

#endif
