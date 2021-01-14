/*
 * Copyright (c) 2017 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== Util.xdc ========
 */
package ti.posix;

/*!
 *  ======== Util ========
 *  Helper module
 */
metaonly module Util
{
    /*!
     *  ======== incPath ========
     *  Compute the POSIX include path for the given target name
     *
     *  When building source files which use the POSIX interface
     *  provided by this package, it is necessary to supply the
     *  include path to the POSIX header files. This must be done
     *  from your package build script. There are different header
     *  files for each supported compiler. You must use the correct
     *  include path for your compiler.
     *
     *  Use this function to compute the correct POSIX include path.
     *  Load this module in your package build script and call this
     *  function with the current target name. It will return an array
     *  of include paths (typically just one).
     *
     *  Use the `Array.join()` method if you need to append these
     *  include paths to a string:
     *
     *  @p(code)
     *      var Util = xdc.useModule('ti.posix.Util');
     *      incs += Util.incPath("ti.targets.arm.elf.M4F").join(" ");
     *  @p
     *
     *  Use the `Array.push()` method to append the array to an existing
     *  array of include paths. If you have a target object, use the
     *  `$name` property to get the canonical name of the target:
     *
     *  @p(code)
     *      var Util = xdc.useModule('ti.posix.Util');
     *      incAry.push(Util.incPath(target.$name));
     *  @p
     *
     *  @param(targetName) the canonical name of the target
     *
     *  @b(return value)
     *  An array of strings, where each array element is an include
     *  path prefixed with '-I'.
     *
     *  An empty array is returned for unsupported targets.
     */
    Any incPath(String targetName);
}
