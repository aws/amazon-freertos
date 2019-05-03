#!/usr/bin/env python3

import subprocess
import os
import sys
from glob import glob

if os.path.isfile('patched'):
    sys.exit()

for tmpfile in glob("*.patch"):
    print("patch", tmpfile)
    result = subprocess.run(["git", "apply",
                             os.path.join('tools', 'cbmc', 'patches', tmpfile)],
                            cwd='../../..')
    if result.returncode:
        print("patching failed: {}".format(tmpfile))

with open('patched', 'w') as outp:
    outp.write('')
