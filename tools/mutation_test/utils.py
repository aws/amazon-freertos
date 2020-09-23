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
import yaml
from colorama import Fore, Style, init as color_init


# ----------------------------------------------------------------------------------------
def get_uuid():
    return uuid.uuid4().hex


def raw_print(value):
    print(value, end='', flush=True)
    Log.log_file(value)

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


def to_json(obj):
    return json.dumps(obj, indent=4)


def to_yaml(obj):
    return yaml.dump(obj, default_flow_style=False)


def flatten(iterable):
    return itertools.chain.from_iterable(iterable)


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


def mkdir(path):
    p = Path(path)
    p.mkdir(exist_ok=True)
    return p


def copydir(src, dst, ignore=[]):
    src = str(src)
    dst = str(dst)
    default_ignore = set(['.git'])
    ignore = set(ignore) | default_ignore

    tmp = Path('.tmp') / get_uuid()
    shutil.rmtree(tmp, ignore_errors=True)
    shutil.copytree(src, tmp, ignore=shutil.ignore_patterns(*ignore))
    dir_util.copy_tree(tmp, dst)
    shutil.rmtree(tmp)


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


def find_idx(seq, predicate):
    return next((idx for idx, elem in enumerate(seq) if predicate(elem)), None)


def glob_list(full_list, filter_expr, *, default=None):
    """
    Given a comma-separated list of positive and negative (prefixed with -) globs,
    e.g., '*,-foo,-bar*', return a matching subset of the full list.
    """
    globs = filter_expr.split(',')
    # We want to preserve order, so list is used instead of set.
    result = default if default else []
    for g in globs:
        match = fnmatch.filter(full_list, g.lstrip('-'))
        if g.startswith('-'):
            result = [item for item in result if item not in match]
        else:
            result += [item for item in match if item not in result]
    return result


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


class CLIParser():
    class Formatter(argparse.HelpFormatter):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs, max_help_position=50, width=110)

    class _CLIParser(argparse.ArgumentParser):
        def __init__(self, **kwargs):
            self.sub_parsers = []
            super().__init__(**kwargs, formatter_class=CLIParser.Formatter)

        def add_parser(self, name, **kwargs):
            if not self.sub_parsers:
                self.sub_parsers = self.add_subparsers(
                    title="available commands", metavar="COMMAND [options ...]"
                )
            return self.sub_parsers.add_parser(name, **kwargs)

        # Override error to use our Log error method.
        def error(self, message):
            self.print_usage(sys.stderr)
            Log.error(message)
            sys.exit(1)

        # Override parse_args so that subparsers are removed during argument parsing.
        def parse_args(self, args=None, namespace=None):
            self.usage = self.format_usage()
            idx = find_idx(self._actions, lambda x: type(x).__name__ == '_SubParsersAction')
            action = idx and self._actions.pop(idx)
            args = super().parse_args(args)
            action = idx and self._actions.insert(idx, action)
            return args

    def __init__(self, arg_parser=None, **kwargs):
        self.arg_parser = arg_parser or self._CLIParser(**kwargs)
        self.commands = {}

    def __getitem__(self, cmd):
        return self.commands[cmd]

    @staticmethod
    def from_arg(*args, **kwargs):
        parser = argparse.ArgumentParser(add_help=False)
        parser.add_argument(*args, **kwargs)
        return parser

    def print_help(self):
        self.arg_parser.print_help()

    def add_command(self, name, **kwargs):
        parser = self.arg_parser.add_parser(name, help=kwargs.pop('help', None), **kwargs)
        return self.commands.setdefault(name, CLIParser(parser))

    def list_commands(self):
        return list(self.commands.keys())

    def add_argument(self, *args, **kwargs):
        self.arg_parser.add_argument(*args, **kwargs)

    def parse(self, argv=None):
        """
        Parse commandline arguments and return a list of `(command, args)` tuple. For example,
        `--src=/home/foo iot delete-thing --all` would return `[('__global__', Namespace(src='/home/foo')),
        ('iot', Namespace()), ('delete-thing', Namespace(all=True))]`
        """
        argv = argv or sys.argv[1:] or ['-h']
        if '-h' in argv or '--help' in argv:
            argparse.ArgumentParser.parse_args(self.arg_parser, argv)

        args_list = []
        parser = self
        command = '__global__'
        while True:
            next_cmd = next((arg for arg in argv if arg in parser.commands), None)
            if parser.commands and next_cmd is None:
                choices = ', '.join(f"'{c}'" for c in parser.commands)
                raise argparse.ArgumentError(None, f'No valid command, choose from ({choices})')
            if next_cmd:
                next_idx = argv.index(next_cmd)
                args_list.append((command, parser.arg_parser.parse_args(argv[:next_idx])))
                parser = parser[next_cmd]
                command = next_cmd
                argv = argv[next_idx + 1:]
            else:
                args_list.append((command, parser.arg_parser.parse_args(argv)))
                break
        return args_list


class Log:
    instance = None

    color_on = True
    color_init(autoreset=True)

    levels = {'error': 0, 'warning': 1, 'info': 2, 'debug': 3}
    level = levels['info']

    file_name = None

    @classmethod
    def error(cls, message):
        if cls.level >= cls.levels['error']:
            Log._print(Fore.RED, '[ERROR] ' + message)

    @classmethod
    def warning(cls, message):
        if cls.level >= cls.levels['warning']:
            Log._print(Fore.YELLOW, '[WARNING] ' + message)

    @classmethod
    def debug(cls, message):
        if cls.level >= cls.levels['debug']:
            Log._print(Fore.MAGENTA, '[DEBUG] ' + message)

    @classmethod
    def info(cls, message):
        if cls.level >= cls.levels['info']:
            Log._print(Fore.GREEN, '[INFO] ' + message)

    @classmethod
    def log(cls, message):
        Log.log_file(f'\n{message}')
        print(message)

    @classmethod
    def log_file(cls, message):
        if Log.file_name:
            try:
                with Log.file_name.open('a') as wr:
                    wr.write(message)
            except Exception as err:
                print(f"[ERROR] Unable to write file: {str(err)}")

    @classmethod
    def _print(cls, color, message):
        message = str(message)
        Log.log_file(f'\n{message}')
        if cls.color_on:
            message = Style.BRIGHT + color + message
        print(message)
