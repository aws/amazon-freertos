import json
import os
import argparse
import shutil
import traceback
import sys
import signal
import re

from utils import get_default_serial_port, yellow_print
import mutation_runner

FuncRegEx = r'^(?:\w ){0,1}\w+ \w+[(](?:(?:.+\n.+)*|.*)[)]'

# set root path to /FreeRTOS/
dir_path = os.path.dirname(os.path.realpath(__file__))
os.chdir('../..')
root_path = os.getcwd()
os.chdir(dir_path)

def write_line_prints(output, source_code, funcs):
    """
    Creates a new source code file with additional statement to print the line of the 
    Writes a file `output` containing contents of `source_code`. Finds element of `funcs` in
    `source_code` and adds a print that reveals function and line number for each function
    in the `source_code`.
    """
    source_code = source_code.split('\n')
    insert_line = -1
    line_dict = {}
    with open(output, 'w') as f:
        for function_string in funcs:
            span = function_string.count('\n') + 1
            for i in range(0, len(source_code)):
                if i + span < len(source_code):
                    unjoined = [source_code[j] for j in range(i, i + span)]
                joined = ''
                for s in unjoined:
                    joined += s + '\n'
                joined.strip('\n')
                if function_string in joined:
                    line_dict[i] = function_string
        insert_line = -1
        for i in range(0, len(source_code)):
            line = source_code[i]   
            if i in line_dict:
                span = line_dict[i].count('\n') + 1
                insert_line = i + span
            if i == insert_line:
                f.write(line+"\n")
                f.write("configPRINTF((\"MUTATION %s: %d\\n\", __FUNCTION__, {}));"
                    .format(i - span + 1) + "\n")
            else:
                f.write(line+"\n")

def main():
    parser = argparse.ArgumentParser('Lines Executed Discover')
    parser.add_argument(
        '--src_config', '-s',
        help='Select the test config file to use eg. -s wifi',
        required=True
    )
    parser.add_argument(
        '--timeout', '-t',
        help='Timeout for each mutant in seconds',
        default=500
    )
    args = parser.parse_args()

    # configs
    with open(os.path.join(dir_path, os.path.join('configs', '{}.json'.format(args.src_config)))) as f:
        config = json.load(f)

    vendor = config['vendor']
    compiler = config['compiler']
    board = config['board']
    port = get_default_serial_port()
    timeout = int(args.timeout)

    backup = mutation_runner.generate_test_runner(test_groups)

    os.chdir(root_path)

    try:
        for s in src:
            shutil.copyfile(s, "{}.old".format(s))
            with open(s) as f:
                text = f.read()
                funcs = re.findall(FuncRegEx, text, re.MULTILINE)
                write_line_prints(s, text, funcs)
        # run once
        output, _ = mutation_runner.flash_and_read(vendor, board, compiler, port, timeout)

        # TODO : Process the output which should contain FUNCTION and LINE macro output
        

    except Exception as e:
        traceback.print_exc()
        raise Exception(e)
    finally:
        # restore files
        # restore aws_test_runner.c
        shutil.copy(backup, os.path.splitext(backup)[0])

        for s in src:
            shutil.copyfile("{}.old".format(s), s)
            os.remove("{}.old".format(s))
            yellow_print("Source code restored")

# Capture SIGINT (usually Ctrl+C is pressed) and SIGTERM, and exit gracefully.
for s in (signal.SIGINT, signal.SIGTERM):
    signal.signal(s, lambda sig, frame: sys.exit())

if __name__ == "__main__":
    main()