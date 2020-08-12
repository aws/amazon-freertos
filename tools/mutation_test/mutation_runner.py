import mutate
import shutil
import subprocess
import shlex
import os
import sys
import serial
import csv
import datetime
import traceback
import time
import argparse
import re
import signal
import ast

import comm
import utils

# set root path to /FreeRTOS/
dir_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.join(dir_path, os.pardir)
root_path = os.path.join(root_path, os.pardir)

# test time
current_time = datetime.datetime.now()
current_date = current_time.strftime('%m%d%y')
current_time = current_time.strftime('%H%M')

# configs
vendor = 'espressif'
board = 'esp32_wrover_kit'
compiler = 'xtensa-esp32'
src_path = 'vendors/espressif/boards/esp32/ports/wifi'

src_path = os.path.join(root_path, src_path)
idf_path = os.path.join(root_path, 'vendors/espressif/esp-idf/tools/idf.py')

# TEST REGEX
EndFlag                             = "-------ALL TESTS FINISHED-------"
StartFlag                           = "---------STARTING TESTS---------"
PassFlag                            = "OK"
FailFlag                            = "FAIL"
RebootFlag                          = "Rebooting..."
NoConnectFlag                       = "WiFi failed to connect to AP SSSS."

TestRegEx = re.compile(r'TEST[(](\w+), (\w+)[)].* *(PASS|FAIL)')

def read_device_output(port, *, start_flag=None, end_flags=None, pass_flag=None, start_timeout=360, exec_timeout):
    """
    Read output from a board. Will look for `start_flag`, or wait for output if not provided, until
    `start_timeout` expires. If success, continue to read output until `exec_timeout` expires or
    end_flag is detected if provided.
    """
    try:
        with serial.Serial(port, 115200, timeout=comm.SERIAL_TIMEOUT) as port:
            output, alive = comm.read_target_output(port, start_flag, end_flags, pass_flag, start_timeout, exec_timeout)
    except serial.SerialException as e:
        raise SerialPortError(str(e))
    return output, alive

def percentage(part, whole):
        return format(100 * (float(part) / (float(whole) if whole != 0 else 1)), '.2f')

def to_csv(csv_path, headers, dict):
    """
    Create a csv file at `csv_path` with `headers` using data in `dict` where each key in `dict` is
    equal to one of the `headers` and `dict[key]` will be written.
    """
    with open(csv_path, 'w') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=headers)
        writer.writeheader()
        for row in dict:
            writer.writerow(row)
        utils.yellow_print("Successfully wrote to CSV - {}".format(csv_path))

def create_jobfile(path=os.path.join(dir_path, '{}-{}.jobfile'), **kwargs):
    """
    Creates a jobfile at `path`. Contains a dictionary string type. 
    """
    with open(path, 'w') as f:
        f.write(kwargs)


def get_default_serial_port():
    """ Return a default serial port. esptool can do this (smarter), but it can create
    inconsistencies where esptool.py uses one port and idf_monitor uses another.

    Same logic as esptool.py search order, reverse sort by name and choose the first port.
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

class MutationTestError(Exception):
    def __init__(self, message):
        self.message = message
        self.enhanced_message = 'MT Source: {}. Exception: {}'.format(root_path, message)
        super(MutationTestError, self).__init__(self.enhanced_message)

class SerialPortError(MutationTestError):
    pass

class CompileFlashFailed(MutationTestError):
    pass

def main():
    parser = argparse.ArgumentParser('Mutation Testing')
    parser.add_argument(
        '--mutants', '-m',
        help='Number of mutants',
        default=10
    )
    parser.add_argument(
        '--src',
        help='Source file to mutate',
        default="iot_wifi.c"
    )
    parser.add_argument(
        '--port', '-p',
        help="Serial port to read from",
        default=None
    )
    parser.add_argument(
        '--timeout', '-t',
        help='Timeout for each mutant in seconds',
        default=500
    )
    parser.add_argument(
        '--csv', '-c',
        help='Store to csv',
        action='store_true',
        default=False
    )
    parser.add_argument(
        '--seed',
        help='Random seed to generate mutants with',
        default=None
    )
    parser.add_argument(
        '--jobfile', '-j',
        help='Specify path to a jobfile to repeat a test; will replace all arguments provided',
        default=None
    )
    args = parser.parse_args()

    src = args.src
    port = args.port if args.port else get_default_serial_port()
    mutant_cnt = int(args.mutants)
    timeout = int(args.timeout)
    csv = args.csv
    seed = args.seed if args.seed else None
    if args.jobfile:
        with open(args.jobfile, 'r') as f:
            jobfile = ast.literal_eval(f.read())
            src = jobfile['src']
            port = jobfile['port']
            mutant_cnt = int(jobfile['mutant_cnt'])
            timeout = int(jobfile['timeout'])
            csv = jobfile['csv']
            seed = jobfile['seed']
    
    # cd to source
    os.chdir(src_path)

    total = 0
    failures = 0
    nc = 0
    trials = []
    test_to_group = {}
    test_to_kills = {}
    test_to_total = {}
    try:
        while total < mutant_cnt:
            # create a copy of the original
            shutil.copyfile(src, '{}.old'.format(src))
            # create a mutant
            original_line, mutated_line, line_number, seed = mutate.main('{}.old'.format(src), src, seed)
            # cd to root
            os.chdir(root_path)
            # cmake the mutant
            # cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S . -B build -DAFR_ENABLE_TESTS=1
            cmd = "cmake -DVENDOR={} -DBOARD={} -DCOMPILER={} -S . -B build -DAFR_ENABLE_TESTS=1".format(vendor, board, compiler)
            utils.yellow_print(cmd)
            subprocess.check_call(shlex.split(cmd))
            # build and execute the mutant
            # ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -B build
            cmd = "./vendors/espressif/esp-idf/tools/idf.py erase_flash flash -B build -p {}".format(port)
            utils.yellow_print(cmd)
            alive = False
            try:
                # Flash to device
                try: 
                    subprocess.check_call(shlex.split(cmd))
                except subprocess.CalledProcessError as e:
                    print(e.output)
                    raise CompileFlashFailed('Flash command failed.')
                utils.yellow_print("Done Flashing")
                # Read device output through serial port
                output, alive = read_device_output(port=port, 
                                                   start_flag=None, 
                                                   end_flags=[EndFlag], 
                                                   pass_flag=PassFlag, 
                                                   exec_timeout=timeout)
                # Analyze the results
                results = re.findall(TestRegEx, output)
                for group, test, result in results:
                    if test not in test_to_group:
                        test_to_group[test] = group
                    if test not in test_to_total:
                        test_to_total[test] = 1
                    else:
                        test_to_total[test] += 1
                    if test not in test_to_kills:
                        test_to_kills[test] = 0
                    elif result == 'FAIL':
                        test_to_kills[test] += 1
                total += 1
                if not alive:
                    failures += 1
                    utils.green_print("Mutant is Killed")
                else:
                    utils.red_print("Mutant is Alive")
                # Add result to CSV queue
                trials.append({'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                                'result':"FAIL/KILLED" if not alive else "PASS/LIVE"})
                utils.yellow_print("Successful Mutant Runs: {}/{}".format(total, mutant_cnt))
            except CompileFlashFailed:
                utils.yellow_print("Cannot compile, discard and move on")
                # Include no-compile in csv results?
                # results.append({'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                #                 'result':"No-Compile"})
                nc += 1
            finally:
                # restore mutant
                os.chdir(src_path)
                shutil.copyfile('{}.old'.format(src), src)
                utils.yellow_print("Source code restored")
    except:
        traceback.print_exc()
        raise
    finally:
        # calculate mutant score
        score = percentage(failures, total)
        utils.yellow_print(score)
        utils.yellow_print("Alive: {} Killed: {} Mutants: {} No-Compile: {} Attempted Runs: {}"
                .format(total - failures, failures, total, nc, total + nc))
        trials.append({'line': "" , 'mutant':"SCORE", 
                       'original':"{}/{}".format(failures, total),'result':"{}%".format(score)})
        
        # aggregate pass/fail counts
        aggregates = []
        for test in test_to_group:
            aggregates.append({'Group': test_to_group[test],
                               'Test': test,
                               'Kills': test_to_kills[test],
                               'Passes': test_to_total[test] - test_to_kills[test],
                               'Total': test_to_total[test]})

        # log to csv
        if csv:
            os.chdir(dir_path)
            utils.mkdir(current_date)
            csv_path = os.path.join(current_date, current_time)
            utils.mkdir(csv_path)
            trials_csv = os.path.join(csv_path, "{}-{}_{}_mutants.csv".format(current_date, current_time, mutant_cnt))
            per_test_csv = os.path.join(csv_path, "{}-{}_tests.csv".format(current_date, current_time)) 
            to_csv(trials_csv, ['line', 'original', 'mutant', 'result'], trials)
            to_csv(per_test_csv, ['Group', 'Test', 'Kills', 'Passes', 'Total'], aggregates)

        create_jobfile(src=src,
                       mutant_cnt=mutant_cnt,
                       timeout=timeout,
                       csv=csv,
                       seed=seed)

        # cleanup
        os.chdir(src_path)
        os.remove('{}.old'.format(src))
        print('Done')
        sys.exit()

# Capture SIGINT (usually Ctrl+C is pressed) and SIGTERM, and exit gracefully.
for s in (signal.SIGINT, signal.SIGTERM):
    signal.signal(s, lambda sig, frame: sys.exit())

if __name__ == "__main__":
    main()