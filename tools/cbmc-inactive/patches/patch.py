#!/usr/bin/env python3

import subprocess
import os
import sys

if os.path.isfile('patched'):
    sys.exit()

patchfiles = [filename for filename in os.listdir('.')
              if filename.endswith('.patch') ]

tmpdir = 'c:\\temp\\' if sys.platform == 'win32' else '/tmp/'
for patchfile in patchfiles:
    tmpfile = tmpdir + patchfile
    print("Patch file: {}".format(patchfile))
    with open(patchfile) as inp, open(tmpfile, "w", newline="\n") as outp:
        outp.write(inp.read())
    cwd = os.getcwd()
    result = subprocess.run(["git", "apply", tmpfile], cwd='../../..')
    if result.returncode:
        print("Patch failed: {}".format(patchfile))

with open('patched', 'w') as outp:
    outp.write('')

