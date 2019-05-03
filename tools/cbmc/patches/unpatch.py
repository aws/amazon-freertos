#!/usr/bin/env python3

import subprocess
import os
from glob import glob

try:
    os.remove("patched")
except FileNotFoundError:
    pass
for tmpfile in glob("*.patch"):
    print("unpatch", tmpfile)
    result = subprocess.run(["git", "apply", "-R",
                             os.path.join('tools', 'cbmc', 'patches', tmpfile)],
                            cwd='../../..')
    if result.returncode:
        print("Unpatching failed: {}".format(tmpfile))
