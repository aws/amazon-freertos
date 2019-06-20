#!/usr/bin/env python3
#
# Python script for preparing the code base for the CBMC proofs.
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

import logging
import os
import sys
import textwrap
from subprocess import CalledProcessError


from make_common_makefile import main as make_common_file
from make_configuration_directories import main as process_configurations
from make_proof_makefiles import main as make_proof_files
from make_cbmc_batch_files import create_cbmc_yaml_files

CWD = os.getcwd()
sys.path.append(os.path.normpath(os.path.join(CWD, "..", "patches")))

from compute_patch import create_patches
from compute_patch import DirtyGitError
from compute_patch import PatchCreationError
from patches_constants import HEADERS

PROOFS_DIR = os.path.dirname(os.path.abspath(__file__))


LOGGER = logging.getLogger("PrepareLogger")


def build():
    process_configurations()
    make_common_file()
    make_proof_files()
    try:
        create_cbmc_yaml_files()
    except CalledProcessError as e:
        logging.error(textwrap.dedent("""\
            An error occured during cbmc-batch generation.
            The error message is: {}
            """.format(str(e))))
        exit(1)

    try:
        create_patches(HEADERS)
    except (DirtyGitError, PatchCreationError) as e:
        logging.error(textwrap.dedent("""\
            An error occured during patch creation.
            The error message is: {}
            """.format(str(e))))
        exit(1)


if __name__ == '__main__':
    logging.basicConfig(format="{script}: %(levelname)s %(message)s".format(
        script=os.path.basename(__file__)))
    build()
