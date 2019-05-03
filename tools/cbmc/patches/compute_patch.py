#!/usr/bin/env python3
#
# Generation of patches for CBMC proofs.
#
# Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import json
import logging
import os
import re
import subprocess
import textwrap

logging.basicConfig(format="{script}: %(levelname)s %(message)s".format(
    script=os.path.basename(__file__)))

LOGGER = logging.getLogger("ComputeConfigurations")

class DirtyGitError(Exception):
    pass


def prolog():
    return textwrap.dedent("""\
        This script generates patch files for the header files used
        in the cbmc proof. These patches permit setting values of preprocessor
        macros as part of the proof configuration.
    """)


def find_all_defines():
    """Collects all define values in Makefile.json.

       Some of the Makefiles use # in the json to make comments.
       As this is non standard json, we need to remove the comment
       lines before parsing. Then we extract all defines from the file.
    """
    defines = set()
    for fldr, _, fyles in os.walk("../proofs/."):
        if not "Makefile.json" in fyles:
            continue
        file = os.path.join(fldr, "Makefile.json")
        with open(file, "r") as source:
            content = "".join([line for line in source
                               if not line.startswith("#")])
            makefile = json.loads(content)
            if "DEF" in makefile.keys():
                define_regex = r"(\w+)\=(?:[\w\{\}\.]+)"
                for define in makefile["DEF"]:
                    matched = re.match(define_regex, define)
                    if matched:
                        defines.add(matched.group(1))
    return defines

def manipulate_headerfile(defines, header_file):
    """Wraps all defines used in an ifndef."""
    define_re = re.compile(r"\s*#\s*define\s*(\w+)\s")
    modified_content = ""
    with open(header_file, "r") as source:
        last = ""
        for line in source:
            match = re.match(define_re, line)
            if (match and
                    match.group(1) in defines and
                    not last.lstrip().startswith("#ifndef")):
                modified_content += textwrap.dedent("""\
                    #ifndef {target}
                        {original}\
                    #endif
                    """.format(target=match.group(1), original=line))
            else:
                modified_content += line
            last = line
    with open(header_file, "w") as output:
        output.write(modified_content)


def header_dirty(header_file):
    """Check that the header_file is not previously modified."""
    diff_state = subprocess.run(["git", "diff-files"],
                                 capture_output=True)

    if bytes(header_file.replace("../", ""), "latin-1") in diff_state.stdout:
        return True
    return False


def create_patch(defines, header_file):
    """Computes a patch enclosing defines used in CBMC proofs with #ifndef."""

    if not header_dirty(header_file):
        manipulate_headerfile(defines, header_file)
        patch = subprocess.run(["git", "diff", header_file],
                               capture_output=True)
        subprocess.call(["git", "checkout", "--", header_file])
        header_path_part = header_file.replace("../", "").replace("/", "_")
        path_name = "auto_patch_" + header_path_part + ".patch"
        with open(path_name, "wb") as patch_file:
            patch_file.write(patch.stdout)
    else:
        error_message = textwrap.dedent("""\
        It seems like {} is in dirty state.
        This script cannot patch files in dirty state.""".format(header_file))
        raise DirtyGitError(error_message)

def create_patches():
    defines = find_all_defines()
    shared_prefix = "../../../demos/pc/windows/common/config_files/"
    create_patch(defines, shared_prefix + "FreeRTOSConfig.h")
    create_patch(defines, shared_prefix + "FreeRTOSIPConfig.h")


if __name__ == '__main__':
    create_patches()
