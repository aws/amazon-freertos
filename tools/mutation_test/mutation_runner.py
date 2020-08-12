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
import pty
import time
import argparse
import re
import signal

import comm
import utils

# set root path to /FreeRTOS/
dir_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.join(dir_path, os.pardir)
root_path = os.path.join(root_path, os.pardir)

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

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def color_print(message, color):
    """ Print a message to stderr with colored highlighting """
    sys.stderr.write("%s%s%s\n" % (color, message, bcolors.ENDC))

def green_print(s):
    color_print(s, bcolors.OKGREEN)

def red_print(s):
    color_print(s, bcolors.RED)

def yellow_print(s):
    color_print(s, bcolors.YELLOW)

def run_command(command, timeout=500):
    try:
        # (master_fd, slave_fd) = pty.openpty()
        process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE, close_fds=True, bufsize=0)
        start = time.time()
        fail = True
        while True:
            # read line from serial pipe
            output = process.stdout.readline()
            
            # if somehow the call ends early
            if not output:
                break
            print(output.strip())

            # check if all tests have passed then we have discovered a live mutant
            if PassFlag in str(output):
                fail = False
            
            # end the process since we know at least one test has failed
            # if crash -> treat as fail
            if any(flag in str(output) for flag in [EndFlag, RebootFlag, NoConnectFlag, FailFlag]):
                break

            curr = time.time()
            # timeout is treated as fail
            if curr - start > timeout:
                yellow_print("Timeout")
                break
        # end the process
        if process.poll() is None:
            # idf_monitor is still running
            process.kill()
            rc = process.wait()
        else:
            rc = process.poll()
        return rc, fail
    except:
        traceback.print_exc()
        process.kill()
    finally:
        end = time.time()
        yellow_print("Elapsed Time: {}s".format(str(format(end - start, '.2f'))))
        # os.close(slave_fd)
        # os.close(master_fd)

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
    with open(csv_path, 'w') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=headers)
        writer.writeheader()
        for row in dict:
            writer.writerow(row)
        yellow_print("Successfully wrote to CSV - {}".format(csv_path))

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

class ExternalCommandFailed(MutationTestError):
    pass

def main():
    parser = argparse.ArgumentParser('Mutation Testing')
    parser.add_argument(
        '--mutants', '-m',
        help='Number of mutants',
        default=10
    )
    parser.add_argument(
        '--src', '-s',
        help='Source file to mutate',
        default="iot_wifi.c"
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
    args = parser.parse_args()

    src = args.src
    mutant_cnt = int(args.mutants)
    timeout = int(args.timeout)
    csv = args.csv

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
            original_line, mutated_line, line_number = mutate.main('{}.old'.format(src), src)
            # cd to root
            os.chdir(root_path)
            # cmake the mutant
            # cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S . -B build -DAFR_ENABLE_TESTS=1
            cmd = "cmake -DVENDOR={} -DBOARD={} -DCOMPILER={} -S . -B build -DAFR_ENABLE_TESTS=1".format(vendor, board, compiler)
            yellow_print(cmd)
            subprocess.check_call(shlex.split(cmd))
            # build and execute the mutant
            # ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -B build
            cmd = "./vendors/espressif/esp-idf/tools/idf.py erase_flash flash -B build"
            yellow_print(cmd)
            alive = False
            try:
                try: 
                    subprocess.check_call(shlex.split(cmd))
                except subprocess.CalledProcessError as e:
                    print(e.output)
                    raise ExternalCommandFailed('Flash command failed.')
                yellow_print("Done Flashing")
                output, alive = read_device_output(get_default_serial_port(), start_flag=None, end_flags=[EndFlag], pass_flag=PassFlag, exec_timeout=500)
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
                    green_print("Mutant is Killed")
                else:
                    red_print("Mutant is Alive")
                trials.append({'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                                'result':"FAIL/KILLED" if not alive else "PASS/LIVE"})
                yellow_print("Successful Mutant Runs: {}".format(total))
            except ExternalCommandFailed:
                yellow_print("Cannot compile, discard and move on")
                # Include no-compile in csv results?
                # results.append({'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                #                 'result':"No-Compile"})
                nc += 1
            finally:
                # restore mutant
                os.chdir(src_path)
                shutil.copyfile('{}.old'.format(src), src)
                yellow_print("Source code restored")
    except:
        traceback.print_exc()
        raise
    finally:
        # calculate mutant score
        score = percentage(failures, total)
        yellow_print(score)
        yellow_print("Alive: {} Killed: {} Mutants: {} No-Compile: {} Attempted Runs: {}"
                .format(total - failures, failures, total, nc, total + nc))
        trials.append({'line': "{} KILLS".format(failures) , 'original':"{} MUTANTS".format(total),
                        'mutant':"SCORE", 'result':"{}%".format(score)})
        
        # aggregate pass/fail counts
        aggregates = []
        for test in test_to_group:
            aggregates.append({'Group': test_to_group[test],
                               'Test': test,
                               'Kills': test_to_kills[test],
                               'Passes': test_to_total[test] - test_to_kills[test],
                               'Total': test_to_total[test],
                               'Score': percentage(test_to_kills[test], test_to_total[test])})

        # log to csv
        if csv:
            os.chdir(dir_path)
            current_time = datetime.datetime.now()
            current_date = current_time.strftime('%m%d%y')
            current_time = current_time.strftime('%m%d%y_%H%M')
            utils.mkdir(current_date)
            csv_path = os.path.join(current_date, current_time)
            utils.mkdir(csv_path)
            trials_csv = os.path.join(csv_path, "{}-{}_{}_mutants.csv".format(current_date, current_time, mutant_cnt))
            per_test_csv = os.path.join(csv_path, "{}-{}_tests.csv".format(current_date, current_time)) 
            to_csv(trials_csv, ['line', 'original', 'mutant', 'result'], trials)
            to_csv(per_test_csv, ['Group', 'Test', 'Kills', 'Passes', 'Total', 'Score'], aggregates)

            
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