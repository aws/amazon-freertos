#!/usr/bin/env python3

import subprocess
import os

result = subprocess.run(["git", "checkout", "lib"], cwd='../../..')
try:
    os.remove("patched")
except FileNotFoundError:
    pass

