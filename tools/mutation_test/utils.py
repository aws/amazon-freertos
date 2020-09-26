import os
import sys
import platform
import argparse
import glob
import itertools
import json
import fnmatch
import shutil
import uuid
from distutils import dir_util
from pathlib import Path
from subprocess import Popen, CalledProcessError


# ----------------------------------------------------------------------------------------
# Print related helpers
def raw_print(value):
    print(value, end='', flush=True)

class bcolors:
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'

def color_print(message, color):
    """ Print a message to stderr with colored highlighting """
    sys.stderr.write("%s%s%s\n" % (color, message, bcolors.ENDC))

def green_print(s):
    color_print(s, bcolors.GREEN)

def red_print(s):
    color_print(s, bcolors.RED)

def yellow_print(s):
    color_print(s, bcolors.YELLOW)

# ----------------------------------------------------------------------------------------
# List processing related helpers
def merge(intervals):
    """ Turns `intervals` [[0,2],[1,5],[7,8]] to [[0,5],[7,8]].
    """
    out = []
    for i in sorted(intervals, key=lambda i: i[0]):
        if out and i[0] <= out[-1][1]:
            out[-1][1] = max(out[-1][1], i[1])
        else:
            out += i,
    return out

def flatten(intervals):
    """ Turns `intervals` [[0,5], [8,10]] to [0, 1, 2, 3, 4, 5, 8, 9, 10].
    
    Note that it is inclusive of the second value.
    """
    out = []
    for i in intervals:
        out += list(range(i[0], i[1] + 1))
    return out

def find_idx(seq, predicate):
    return next((idx for idx, elem in enumerate(seq) if predicate(elem)), None)

# ----------------------------------------------------------------------------------------
# Directory related helpers

def lsdir(path, *, recurse=False):
    if recurse:
        dirs = []
        files = []
        for dirpath, dirnames, filenames in os.walk(path):
            for d in dirnames:
                dirs.append(os.path.relpath(os.path.join(dirpath, d), path))
            for f in filenames:
                files.append(os.path.relpath(os.path.join(dirpath, f), path))
    else:
        _, dirs, files = next(os.walk(path))
    return sorted(dirs), sorted(files)

def delete_from_parent_dir(path):
    p = Path(path)
    parent_dir = p.parent
    if parent_dir == p:
        raise Exception('Refusing to delete root directory')
    p = str(p.relative_to(parent_dir))
    if get_host_os() == 'win':
        cmd = ['rd', '/S', '/Q', p]
    else:
        cmd = f'rm -rf {p}'
    try:
        Popen(cmd, shell=True, cwd=str(parent_dir)).communicate()
    except CalledProcessError as err:
        raise Exception(str(err))

# ----------------------------------------------------------------------------------------
# System related helpers

def get_default_serial_port():
    """ 
    Return a default serial port.
    """
    # Import is done here in order to move it after the check_environment() ensured that pyserial has been installed
    import serial.tools.list_ports

    ports = list(reversed(sorted(
        p.device for p in serial.tools.list_ports.comports())))
    try:
        print("Choosing default port %s (use '-p PORT' option to set a specific serial port)" % ports[0].encode('ascii', 'ignore'))
        return ports[0]
    except IndexError:
        raise RuntimeError("No serial ports found. Connect a device, or use '-p PORT' option to set a specific port.")

def get_host_os():
    """
    Return host OS name.
    """
    host_os = platform.system()
    os_map = {
        'Windows': 'win',
        'Linux': 'linux',
        'Darwin': 'macos'
    }
    return os_map[host_os]

def add_to_env_path(files, search_path):
    """
    Search `files` in `search_path` and add results to environment variable PATH if found. Return
    True if all files are found, otherwise False.
    """
    # Return true if files is empty.
    if not files:
        return True

    search_path = [glob.glob(os.path.realpath(p)) for p in search_path]
    search_path = list(itertools.chain(*search_path))
    for f in files:
        # Check if file is in search_path.
        file_path = shutil.which(f, path=os.pathsep.join(search_path))
        if file_path:
            current_paths = os.environ['PATH'].split(os.pathsep)
            # Remove file_dir from the system PATH if presented, then insert it at the front.
            file_dir = os.path.dirname(file_path)
            try:
                current_paths.remove(file_dir)
            except ValueError:
                pass
            current_paths.insert(0, file_dir)
            os.environ['PATH'] = os.pathsep.join(current_paths)

        # Ensure that the file is in the system path.
        if not shutil.which(f):
            return False

    return True

