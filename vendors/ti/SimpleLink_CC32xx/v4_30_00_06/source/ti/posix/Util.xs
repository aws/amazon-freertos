/*
 * Copyright (c) 2017-2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Util.xs ========
 */

/*
 *  ======== incPath ========
 */
function incPath(targetName)
{
    var incs = new Array();
    var cgt = "";
    var match = null;

    /* special case ticlang */
    if (targetName.match(/^ti\.targets\.arm\.clang/)) {
        /* wonky, but matches the syntax of .match() */
        match = ["ignore", "ticlang"];
    } else {
        /* extract the first word of the target package name */
        match = targetName.match(/^(\w+)\.targets/);
    }

    /* compute the include path suffix */
    switch (match != null ? match[1] : "") {
        case "gnu":      cgt = "gcc";     break;
        case "iar":      cgt = "iar";     break;
        case "ti":       cgt = "ccs";     break;
        case "ticlang":  cgt = "ticlang"; break;
        default:
            cgt = null;
    }

    /* if unsupported compiler, return empty array */
    if (cgt == null) {
        return(incs);
    }

    /* currently, just one include path */
    var base = this.$package.packageBase;
    incs.push("-I" + base + cgt);
    return (incs);
}
