/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
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
 *  ======== libBuilder.xs ========
 */

/* Return the lib path based on SimpleLink Library conventions */
function getLibPath(targetString)
{
    let isa = targetString.split(".").pop().toLowerCase();
    let toolchain = "undefined";

    if (targetString.match(/ti/)) {
        if (targetString.match(/clang/)) {
            toolchain = "ticlang";
        }
        else {
            toolchain = "ccs";
        }
    }
    else if (targetString.match(/gnu/)) {
        toolchain = "gcc";
    }
    else if (targetString.match(/iar/)) {
        toolchain = "iar";
    }

    return ("lib/" + toolchain + "/" + isa + "/");
}

/*
 *  ======== makeLibs ========
 */
function makeLibs(name, targets, objects, cOpts)
{
    var lib;
    var gccOpts;
    /* http://rtsc.eclipseprojects.io/cdoc-tip/index.html#xdc/bld/Library.html#.Attrs */
    var attrs = {
        copts: "",
        profile: "",
        suffix: ".a"
    };
    /* Legacy attributes to maintain compatibility */
    var attrs_legacy = {
        copts: "",
        profile: ""
    };

    /*
     * Array of build profiles to build libs for. Profile must be supported by
     * the XDC ITarget.xdc (ie ti.arm.clang.M4) interface.
     *
     * Disabling coverage profiles. Currently, only ticlang supports
     * coverage.
     */
    var profiles = [ "release_lto", "release" /*, "coverage" */];

    function addLibraryProfiles()
    {
        for (var i = 0; i < profiles.length; i++) {
            if (target.profiles[profiles[i]] != undefined) {

                /* Add appropriate build profile name */
                var libName = (profiles[i] == "release" ? name :
                    [ name, profiles[i] ].join("_"));

                /*
                 * Construct the output path to follow SimpleLink library
                 * naming conventions.
                 *
                 *  <namespace>/lib/<toolchain>/<isa>/<libname>.a
                 *  drivers/lib/ticlang/m4f/drivers_cc32xx_release.a
                 */
                let libOutput = getLibPath(targetName) + libName;

                /*
                 * Construct the output path for legacy libraries to maintain
                 * compatibility
                 */
                let libOutput_legacy = "lib/" + libName;

                /* Specify the build profile to use for the target library */
                attrs.profile = profiles[i];
                attrs_legacy.profile = profiles[i];

                /* Generate libraries following SimpleLink Library naming */
                lib = Pkg.addLibrary(libOutput, target, attrs);
                lib.addObjects(objs);

                /* Generate legacy libraries to maintain compatibility */
                lib = Pkg.addLibrary(libOutput_legacy, target, attrs_legacy);
                lib.addObjects(objs);
            }
        }
    }

    for each (var targetName in targets) {
        /* Only proceed if target already in Build object */
        if (Build.printBuildTargets().indexOf(targetName + "\n") < 0) {
            continue;
        }

        /* Load target module */
        var target = xdc.module(targetName);

        var objs = objects.concat();

        /*
         *  Exlude incompatible assembly files from the object list.
         *  The objects parameter includes the super-set list. We prune that here.
         *  This code assumes that assembly files follow the form xyz_ccs.asm,
         *  xyz_gnu.asm, and xyz_iar.asm.
         */
        if (!targetName.match('clang')) {
            for (var i = objs.length - 1; i >= 0; i--) {
                if (objs[i].match('_clang.asm')) {
                    objs.splice(i, 1);
                }
            }
        }
        if (!targetName.match('ti.targets') || targetName.match('clang')) {
            for (var i = objs.length - 1; i >= 0; i--) {
                if (objs[i].match('_ccs.asm')) {
                    objs.splice(i, 1);
                }
            }
        }
        if (!targetName.match('gnu.targets')) {
            for (var i = objs.length - 1; i >= 0; i--) {
                if (objs[i].match('_gnu.asm')) {
                    objs.splice(i, 1);
                }
            }
        }
        if (!targetName.match('iar.targets')) {
            for (var i = objs.length - 1; i >= 0; i--) {
                if (objs[i].match('_iar.asm')) {
                    objs.splice(i, 1);
                }
            }
        }

        /*
         *  When using '-O2' optimizations, GCC automatically sets the
         *  '-fisolate-erroneous-paths-dereference' flag.
         *
         *  From 'Using the GNU Compiler Collection - For GCC version 9.2.1':
         *  "-fisolate-erroneous-paths-dereference -- Detect paths that
         *  trigger erroneous or undefined behavior due to dereferencing a
         *  null pointer. Isolate those paths from the main control flow and
         *  turn the statement with erroneous or undefined behavior into a
         *  trap. This flag is enabled by default at '-O2' and higher and
         *  depends on '-fdelete-null-pointer-checks' also being enabled."
         *
         *  The '-fdelete-null-pointer-checks' option is also enabled by
         *  default when using '-O2' optimization.
         */
        if (targetName.match('gnu.targets')) {
            gccOpts = " -fno-isolate-erroneous-paths-dereference ";
        }
        else {
            gccOpts = "";
        }

        /* Disable asserts & logs for the non-instrumented library */
        attrs.copts = cOpts + gccOpts +
            " -Dxdc_runtime_Log_DISABLE_ALL -Dxdc_runtime_Assert_DISABLE_ALL";
        attrs_legacy.copts = attrs.copts;

        /* Create library instances to generate libraries */
        lib = addLibraryProfiles();
    }
}

var cc26x1Targets = [
    "ti.targets.arm.clang.M4",
    "ti.targets.arm.elf.M4",
    "gnu.targets.arm.M4",
    "iar.targets.arm.M4",
];

var cc26xxTargets = [
    "ti.targets.arm.clang.M4F",
    "ti.targets.arm.elf.M4F",
    "gnu.targets.arm.M4F",
    "iar.targets.arm.M4F",
];

var cc32xxTargets = [
    "ti.targets.arm.clang.M4",
    "ti.targets.arm.elf.M4",
    "gnu.targets.arm.M4",
    "iar.targets.arm.M4",
];

var msp432e4Targets = [
    "ti.targets.arm.clang.M4F",
    "ti.targets.arm.elf.M4F",
    "gnu.targets.arm.M4F",
    "iar.targets.arm.M4F",
];

var mtxxTargets = [
    "ti.targets.arm.clang.M33",
    "ti.targets.arm.clang.M33F",
];
