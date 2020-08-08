import mutate
import shutil
import subprocess
import shlex
import os
import sys
import serial
import errno
import csv
import datetime
import traceback
import pty
import time
import argparse
import tty

# set root path to /FreeRTOS/
dir_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.join(dir_path, os.pardir)
root_path = os.path.join(root_path, os.pardir)

vendor = 'espressif'
board = 'esp32_wrover_kit'
compiler = 'xtensa-esp32'
src_path = os.path.join(root_path, 'vendors/espressif/boards/esp32/ports/wifi')
idf_path = os.path.join(root_path, 'vendors/espressif/esp-idf/tools/idf.py')

# EXIT SERIAL MONITOR KEY
CTRL_RBRACKET = '\x1d'  # Ctrl+]

# TEST REGEX
EndFlag                             = "-------ALL TESTS FINISHED-------"
StartFlag                           = "---------STARTING TESTS---------"
PassFlag                            = "OK"
FailFlag                            = "FAIL"
RebootFlag                          = "Rebooting..."
NoConnectFlag                       = "WiFi failed to connect to AP SSSS."

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

def run_command(command):
    try:
        (master_fd, slave_fd) = pty.openpty()
        process = subprocess.Popen(shlex.split(command), stdin=slave_fd, stdout=subprocess.PIPE, close_fds=True, bufsize=0)
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
            if PassFlag in output:
                fail = False
            
            # end the process since we know at least one test has failed
            # if crash -> treat as fail
            if any(flag in output for flag in [StartFlag, RebootFlag, NoConnectFlag]):
                break

            curr = time.time()
            # timeout is treated as fail
            if curr - start > 500:
                yellow_print("Timeout")
                break
        # end the process
        if process.poll() is None:
            # idf_monitor is still running
            try:
                process.kill()
            except OSError as e:
                if e.errno == errno.ESRCH:
                    # ignores a possible race condition which can occur when the process exits between poll() and kill()
                    pass
                else:
                    raise
            rc = process.wait()
        else:
            rc = process.poll()
        return rc, fail
    except KeyboardInterrupt:
        raise KeyboardInterrupt
    except:
        traceback.print_exc()
        try:
            process.kill()
        except OSError as e:
            if e.errno == errno.ESRCH:
                # ignores a possible race condition which can occur when the process exits between poll() and kill()
                pass
            else:
                raise
        except:
            print("reached here")
            pass
    finally:
        end = time.time()
        yellow_print("Elapsed Time: {}s".format(str(format(end - start, '.2f'))))
        os.close(slave_fd)
        os.close(master_fd)

def percentage(part, whole):
        return format(100 * (float(part) / (float(whole) if whole != 0 else 1)), '.2f')

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
        '--csv_off',
        help='Turn off csv record',
        action='store_true',
        default=False
    )
    args = parser.parse_args()

    src = args.src
    mutant_cnt = int(args.mutants)
    timeout = int(args.timeout)
    csv_off = args.csv_off

    # MIGHT BE USEFUL FOR SERIAL INPUT BUT CANT FIGURE OUT HOW TO DO IT
    # serial_instance = serial.serial_for_url(get_default_serial_port(), os.environ.get('MONITOR_BAUD', 115200),
    #                                         do_not_open=True)
    # serial_instance.dtr = False
    # serial_instance.rts = False

    # cd to source
    os.chdir(src_path)

    # TEMPORARY: COPY ORIGINAL SOURCE
    shutil.copyfile('iot_wifi_original.c', src)

    total = 0
    failures = 0
    nc = 0
    # CSV GENERATION
    # results should be collected in a way that matches the headers
    headers = ['line', 'original', 'mutant', 'result']
    results = []
    try:
        while total < mutant_cnt:
            # create a copy of the original
            shutil.copyfile(src, 'original')
            # create a mutant
            original_line, mutated_line, line_number = mutate.main('original', src)
            # cd to root
            os.chdir(root_path)
            # cmake the mutant
            # cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S . -B build -DAFR_ENABLE_TESTS=1
            cmd = "cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S . -B build -DAFR_ENABLE_TESTS=1"
            yellow_print(cmd)
            subprocess.check_call(shlex.split(cmd))
            # build and execute the mutant
            # ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -B build
            cmd = "./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -B build"
            yellow_print(cmd)
            fail = True
            try:
                rc, fail = run_command(cmd)
                yellow_print("Return Code: {}".format(rc))
                if rc == 2:
                    yellow_print("Cannot compile, discard and move on")
                    # Include no-compile in csv results?
                    # results.append({'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                    #                 'result':"No-Compile"})
                    nc += 1
                else:
                    total += 1
                    if fail:
                        failures += 1
                        green_print("Mutant is Killed")
                    else:
                        red_print("Mutant is Alive")
                    results.append({'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                                    'result':"FAIL/KILLED" if fail else "PASS/LIVE"})
                    yellow_print("Successful Mutant Runs: {}".format(total))
            except KeyboardInterrupt:
                raise KeyboardInterrupt
            except:
                traceback.print_exc()
            finally:
                # restore mutant
                os.chdir(src_path)
                shutil.copyfile('original', src)
    except:
        traceback.print_exc()
        raise
    finally:
        # calculate mutant score
        score = percentage(failures, total)
        yellow_print(score)
        yellow_print("Alive: {} Killed: {} Mutants: {} No-Compile: {} Attempted Runs: {}"
                .format(total - failures, failures, total, nc, total + nc))
        results.append({'line': "{} KILLS".format(failures) , 'original':"{} MUTANTS".format(total),
                        'mutant':"SCORE", 'result':"{}%".format(score)})

        # log to csv
        os.chdir(dir_path)
        current_time = datetime.datetime.now()
        current_date = current_time.strftime('%m%d%y')
        current_time = current_time.strftime('%m%d%y_%H%M')
        if not os.path.exists(current_date):
            os.mkdir(current_date)
        csv_path = os.path.join(current_date, "{}_{}_mutants.csv".format(current_time, mutant_cnt))
        if not csv_off:
            with open(csv_path, 'w') as csvfile:
                writer = csv.DictWriter(csvfile, fieldnames=headers)
                writer.writeheader()
                for row in results:
                    writer.writerow(row)
                yellow_print("Successfully wrote to CSV")
        # cleanup
        os.chdir(src_path)
        os.remove('original')
        print('Done')

if __name__ == "__main__":
    main()