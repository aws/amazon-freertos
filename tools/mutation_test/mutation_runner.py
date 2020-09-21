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
import random
import json
import collections
import fileinput
from pathlib import Path

import comm
import utils
import mutator
import coverage

# set root path to /FreeRTOS/
dir_path = os.path.dirname(os.path.realpath(__file__))
os.environ['DIR_PATH'] = dir_path
os.chdir('../..')
root_path = os.getcwd()
os.environ['ROOT_PATH'] = root_path
os.chdir(dir_path)

# test time
current_time = datetime.datetime.now()
current_date = current_time.strftime('%m%d%y')
current_time = current_time.strftime('%H%M')

idf_path = os.path.join(root_path, 'vendors/espressif/esp-idf/tools/idf.py')

# TEST REGEX
class FLAGS:
    EndFlag                             = "-------ALL TESTS FINISHED-------"
    StartFlag                           = "---------STARTING TESTS---------"
    PassFlag                            = "OK"
    FailFlag                            = "FAIL"
    CrashFlag                           = "Rebooting..."

TestRegEx = re.compile(r'TEST[(](\w+), (\w+)[)].* *(PASS|FAIL)')

def cmake(vendor, board, compiler):
    # cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S . -B build -DAFR_ENABLE_TESTS=1
    cmd = "cmake -DVENDOR={} -DBOARD={} -DCOMPILER={} -S . -B build -DAFR_ENABLE_TESTS=1".format(vendor, board, compiler)
    utils.yellow_print(cmd)
    subprocess.check_call(shlex.split(cmd))

def build():
    """
    Compiles freeRTOS from free-rtos/build dir
    """
    old_path = os.getcwd()
    os.chdir(os.path.join(root_path, "build"))
    try:
        subprocess.check_call(["make", "-j", "14", "all"])
    except subprocess.CalledProcessError as e:
        print(e.output)
        raise CompileFailed('Compile Command Failed')
    finally:
        os.chdir(old_path)

def flash(cmd):
    # ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -B build
    # cmd = "./vendors/espressif/esp-idf/tools/idf.py erase_flash flash -B build -p {}".format(port)
    utils.yellow_print(cmd)
    # Flash to device
    try: 
        utils.yellow_print("Flashing to device...")
        subprocess.check_call(shlex.split(cmd))
    except subprocess.CalledProcessError as e:
        print(e.output)
        raise CompileFailed('Flash command failed.')
    utils.yellow_print("Done Flashing")

def read_device_output(port, *, start_flag=None, end_flag=None, crash_flag=None, 
                        pass_flag=None, start_timeout=360, exec_timeout):
    """
    Read output from a board. Will look for `start_flag`, or wait for output if not provided, until
    `start_timeout` expires. If success, continue to read output until `exec_timeout` expires or
    end_flag is detected if provided.
    """
    try:
        utils.yellow_print("Monitoring...")
        with serial.Serial(port, 115200, timeout=comm.SERIAL_TIMEOUT) as port:
            output, final_flag = comm.read_target_output(port, start_flag, crash_flag, end_flag, 
                                                pass_flag, start_timeout, exec_timeout)
    except serial.SerialException as e:
        raise SerialPortError(str(e))
    return output, final_flag

def flash_and_read(port, timeout, flash_command):

    try:
        subprocess.check_call(flash_command, shell=True)
    except subprocess.CalledProcessError as e:
        print(e)
        raise CompileFailed("Failed to compile")

    # # cmake the mutant
    # cmake(vendor, board, compiler)
    # # build and flash
    # build()
    # flash(flash_command)
    final_flag = FLAGS.EndFlag
    # Read device output through serial port
    output, final_flag = read_device_output(port=port, 
                                        start_flag=None,
                                        crash_flag=FLAGS.CrashFlag,
                                        end_flag=FLAGS.EndFlag,
                                        pass_flag=FLAGS.PassFlag,
                                        exec_timeout=timeout)
    os.system('clear')
    return output, final_flag         

def percentage(part, whole):
    """
    Returns percentage with 2 decimals. If `whole` is 0, `whole` is set to 1.
    """
    return format(100 * (float(part) / (float(whole) if whole != 0 else 1)), '.2f')

def _process_file(file_path):
    p = Path(file_path)
    backup = p.with_suffix(p.suffix + '.old')
    shutil.copy(p, backup)
    return fileinput.input(files=file_path, inplace=True), backup

def generate_test_runner(test_groups):
    """
    Given `test_groups`, e.g., ['Full_MQTT', 'Full_TCP'], generate function body
    of void RunTests() from aws_test_runner.c file. Note that values in `test_groups`
    must match the exact UNITY test group names.
    """
    indent = ' ' * 4
    is_generating = False
    test_runner_path = os.path.join(root_path, 'tests/common/aws_test_runner.c')
    fi, backup = _process_file(test_runner_path)
    for line in fi:
        if is_generating:
            if line.startswith('}'):
                sys.stdout.write('{\n')
                for test_group in test_groups:
                    sys.stdout.write(f'{indent}RUN_TEST_GROUP( {test_group} );\n')
                sys.stdout.write(line)
                is_generating = False
        else:
            sys.stdout.write(line)
            if line.startswith('static void RunTests'):
                is_generating = True
    return backup
    

def to_csv(csv_path, headers, dict):
    """
    Create a csv file at `csv_path` with `headers` using data in `dict` where each key in `dict` is
    equal to one of the `headers` and `dict[key]` will be written.
    """
    directory = os.path.dirname(csv_path)
    Path(directory).mkdir(parents=True, exist_ok=True)
    with open(csv_path, 'w') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=headers)
        writer.writeheader()
        for row in dict:
            writer.writerow(row)
        utils.yellow_print("Successfully wrote to CSV - {}".format(csv_path))

def create_jobfile(path=os.path.join(dir_path, 'jobfile'), **kwargs):
    """
    Creates a jobfile at `path`. Contains a dictionary string type. 
    """
    with open(path, 'w') as f:
        f.write(str(kwargs))

class MutationTestError(Exception):
    def __init__(self, message):
        self.message = message
        self.enhanced_message = 'MT Source: {}. Exception: {}'.format(root_path, message)
        super(MutationTestError, self).__init__(self.enhanced_message)

class SerialPortError(MutationTestError):
    pass

class CompileFailed(MutationTestError):
    pass

class LineOutOfRange(MutationTestError):
    pass

def run_custom(task, args, config):
    vendor, board, compiler = config['vendor'], config['board'], config['compiler']
    flash_command = config['flash_command']

    port = args.port if args.port else utils.get_default_serial_port()
    os.environ['PORT'] = port
    timeout = int(args.timeout)

    mutation = mutator.Mutator(src=task['src'], mutation_patterns=task['patterns'])
    mutant_patterns = mutation.getPatterns()

    searched_patterns = set()

    data_record = []
    # outer try is for finally generating csv if automation stops early
    try:
        for mp in mutant_patterns:
            if mp.pattern in searched_patterns:
                continue
            searched_patterns.add(mp.pattern)
            utils.yellow_print("Searching for pattern: {}".format(mp.pattern))
            occurences_with_mp = mutation.findOccurrences(mutation_pattern=mp)
            for o in occurences_with_mp:
                utils.yellow_print(o)
            failures = 0
            total = 0
            times_per_pattern = task['total_times']
            for occurrence in occurences_with_mp:
                # if completed times_per_pattern times, exit loop
                if times_per_pattern == 0:
                    break
                # mutate the code
                utils.yellow_print(occurrence)
                mutation.mutate(occurrence)
                # try is for catching compile failure to continue execution
                try:
                    # cmake, build, flash, and read
                    output, final_flag = flash_and_read(port, timeout, flash_command)
                    if final_flag == FLAGS.PassFlag:
                        utils.red_print("Mutant is Alive")
                    else:
                        failures += 1
                        utils.green_print("Mutant is Killed")
                    total += 1
                except CompileFailed:
                    utils.yellow_print("Cannot compile, discard and move on")
                finally:
                    mutation.restore()
                times_per_pattern -= 1
            data_record.append(
                {'pattern': "{} => {}".format(mp.pattern, mp.transformation),
                 'failures': failures,
                 'total': total,
                 'percentage': float(percentage(failures, total)) * 0.01 if total > 0 else 2})
    except:
        traceback.print_exc()
        raise
    finally:
        mutation.cleanup()
        if args.csv:
            csv_path = os.path.join(
                dir_path, "csvs/{}/{}/{}_mutant_comparison.csv".format(current_date, current_time, task['name']))
            to_csv(csv_path, ['pattern', 'failures', 'total', 'percentage'], data_record)

def run_randomized(task, args, config):
    # config
    flash_command = config['flash_command']
    # args
    port = args.port if args.port else utils.get_default_serial_port()
    os.environ['PORT'] = port

    mutant_cnt = int(args.mutants)
    timeout = int(args.timeout)
    csv = args.csv
    seed = args.seed if args.seed else None
    if args.jobfile:
        with open(args.jobfile, 'r') as f:
            jobfile = ast.literal_eval(f.read())
            if not args.port:
                port = jobfile['port']
            if not args.mutants:
                mutant_cnt = int(jobfile['mutant_cnt'])
            if not args.timeout:
                timeout = int(jobfile['timeout'])
            if not args.csv:
                csv = jobfile['csv']
            if not args.seed:
                seed = jobfile['seed']
    
    # create a rng for this run
    if not seed:
        seed = random.randrange(sys.maxsize)
    utils.yellow_print("Current test seed is: {}".format(seed))
    rng = random.Random(seed)

    ### BEGIN MUTATION TESTING ###
    run_cnt = 0
    failures = 0
    nc = 0
    trials = []
    test_to_kills = {}

    # Create a mutator object
    mutation = mutator.Mutator(src = task['src'], mutation_patterns = task['patterns'], rng=rng)
    try:
        while run_cnt < mutant_cnt:
            try:
                _, occurrence, original_line, mutated_line = mutation.generateRandomMutant()
                file_changed = occurrence.file
                line_number = occurrence.line
                # cmake, build, flash, and read
                output, final_flag = flash_and_read(port, timeout, flash_command)
                run_cnt += 1

                # Analyze the results
                results = re.findall(TestRegEx, output)
                for group, test, result in results:
                    if (group, test) not in test_to_kills:
                        test_to_kills[(group, test)] = (1, 1) if result == 'FAIL' else (0, 1)
                    else:
                        kills, total = test_to_kills[(group, test)]
                        test_to_kills[(group, test)] = ((kills + 1, total + 1) if result == 'FAIL' 
                                                        else (kills, total + 1))

                # mutant_status can either be "FAIL", "PASS", "CRASH", "TIMEOUT"
                mutant_status = "FAIL"
                if final_flag == FLAGS.PassFlag:
                    utils.red_print("Mutant is Alive")
                    mutant_status = "PASS"
                else:
                    failures += 1
                    utils.green_print("Mutant is Killed")
                    
                if final_flag == FLAGS.CrashFlag:
                    mutant_status = "CRASH"
                elif final_flag == "TIMEOUT":
                    mutant_status = "TIMEOUT"
                # Add result to CSV queue
                trials.append({'file': file_changed,'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                                'result':"{}/KILLED".format(mutant_status) if mutant_status != "PASS" else "PASS/LIVE"})
                utils.yellow_print("Successful Mutant Runs: {}/{}".format(run_cnt, mutant_cnt))
            except CompileFailed:
                utils.yellow_print("Cannot compile, discard and move on")
                # Include no-compile in csv results?
                # results.append({'file': file_changed, 'line':line_number, 'original':original_line, 
                #                   'mutant':mutated_line, 'result':"No-Compile"})
                nc += 1
            finally:
                # restore mutant
                # os.chdir(src_path)
                # create a copies of the original
                mutation.restore()
                utils.yellow_print("Source code restored")
    except:
        traceback.print_exc()
        raise
    finally:
        # calculate mutant score
        score = percentage(failures, run_cnt)
        utils.yellow_print(score)
        utils.yellow_print("Alive: {} Killed: {} Mutants: {} No-Compile: {} Attempted Runs: {}"
                .format(run_cnt - failures, failures, run_cnt, nc, run_cnt + nc))
        trials.append({'file': "RESULTS:", 'line': "{} NO-COMPILE".format(nc) , 'mutant':"SCORE", 
                    'original':"{} KILLED/{} MUTANTS".format(failures, run_cnt),'result':"{}%".format(score)})
        
        # aggregate pass/fail counts
        aggregates = []
        for group, test in test_to_kills:
            kills, total = test_to_kills[(group, test)]
            aggregates.append({'Group': group,
                               'Test': test,
                               'Kills': kills,
                               'Passes': total - kills,
                               'Total': total})

        # log to csv
        if args.csv:
            os.chdir(dir_path)
            csv_path = os.path.join('csvs', current_date)
            utils.mkdir(csv_path)
            csv_path = os.path.join(csv_path, current_time)
            utils.mkdir(csv_path)
            trials_csv = os.path.join(csv_path, "{}-{}_{}_mutants.csv".format(current_date, current_time, mutant_cnt))
            per_test_csv = os.path.join(csv_path, "{}-{}_tests.csv".format(current_date, current_time)) 
            to_csv(trials_csv, ['file', 'line', 'original', 'mutant', 'result'], trials)
            to_csv(per_test_csv, ['Group', 'Test', 'Kills', 'Passes', 'Total'], aggregates)

        create_jobfile(mutant_cnt=mutant_cnt,
                port=port,
                timeout=timeout,
                csv=csv,
                seed=seed)

def coverage_main(args, config):
    """ Function that is called when user specified `mutation_runner.py coverage` from cmd
    """
    flash_command = config['flash_command']

    port = args.port if args.port else utils.get_default_serial_port()
    os.environ['PORT'] = port
    timeout = int(args.timeout)

    os.chdir(root_path)

    for task in config['tasks']:
        utils.yellow_print("Running task: {}".format(task['name']))

        # Generate test runner to run only on those test groups
        utils.yellow_print("Generating test runner based on supplied test groups...")
        backup = generate_test_runner(task['test_groups'])
        try:
            for s in task['src']:
                shutil.copyfile(s, "{}.old".format(s))
                with open(s) as f:
                    text = f.read()
                    funcs = re.findall(coverage.FuncRegEx, text, re.MULTILINE)
                    coverage.write_line_prints(s, text, funcs)
                # run once
                output, _ = flash_and_read(port, timeout, flash_command)
                # TODO : Process the output which should contain FUNCTION and LINE macro output
        except Exception as e:
            traceback.print_exc()
            raise Exception(e)
        finally:
            # restore aws_test_runner.c
            shutil.copy(backup, os.path.splitext(backup)[0])
            os.remove(backup)
            for s in task['src']:
                shutil.copyfile("{}.old".format(s), s)
                os.remove("{}.old".format(s))
                utils.yellow_print("Source code restored")

def mutation_main(args, config):
    """ Function that is called when user specifies `mutation_runner.py start` from cmd
    """
    # change to root path
    os.chdir(root_path)

    utils.yellow_print("Running tasks")
    for task in config['tasks']:
        utils.yellow_print("Running task: {}".format(task['name']))

        # Generate test runner to run only on those test groups
        utils.yellow_print("Generating test runner based on supplied test groups...")
        backup = generate_test_runner(task['test_groups'])

        # set default mutations if patterns is equal to "all"
        if task['patterns'] == "all": task['patterns'] = mutator.default_patterns
        elif task['patterns'] == "custom_patterns": task['patterns'] = mutator.custom_patterns
        try:
            if "custom" in task:
                # build your own run setups here
                run_custom(task, args, config)
            else:
                run_randomized(task, args, config)
        except:
            traceback.print_exc()
            raise
        finally:
            # restore aws_test_runner.c
            shutil.copy(backup, os.path.splitext(backup)[0])
            os.remove(backup)

def main():
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers()

    # create parser for coverage tool
    parser_coverage = subparsers.add_parser('coverage')
    parser_coverage.set_defaults(func=coverage_main)
    parser_coverage.add_argument(
        '--src_config', '-s',
        help='Select the test config file to use eg. -s wifi',
        required=True
    )
    parser_coverage.add_argument(
        '--port', '-p',
        help="Serial port to read from",
        default=None
    )
    parser_coverage.add_argument(
        '--timeout', '-t',
        help='Timeout for each mutant in seconds',
        default=300
    )

    # create parser for mutation tool
    parser_mutation = subparsers.add_parser('start')
    parser_mutation.set_defaults(func=mutation_main)
    parser_mutation.add_argument(
        '--src_config', '-s',
        help='Select the test config file to use eg. -s wifi',
        required=True
    )
    parser_mutation.add_argument(
        '--mutants', '-m',
        help='Max number of mutants to create from all tasks (cap)',
        default=10
    )
    parser_mutation.add_argument(
        '--port', '-p',
        help="Serial port to read from",
        default=None
    )
    parser_mutation.add_argument(
        '--timeout', '-t',
        help='Timeout for each mutant in seconds',
        default=300
    )
    parser_mutation.add_argument(
        '--csv', '-c',
        help='Store to csv',
        action='store_true',
        default=False
    )
    parser_mutation.add_argument(
        '--seed',
        help='Random seed to generate mutants with',
        default=None
    )
    parser_mutation.add_argument(
        '--jobfile', '-j',
        help='Specify path to a jobfile to repeat a test; will replace all arguments provided',
        default=None
    )

    args = parser.parse_args()

    # configs
    with open(os.path.join(dir_path, os.path.join('configs', '{}.json'.format(args.src_config)))) as f:
        config = json.load(f)

    args.func(args, config)
    print('Done')
    sys.exit()

# Capture SIGINT (usually Ctrl+C is pressed) and SIGTERM, and exit gracefully.
for s in (signal.SIGINT, signal.SIGTERM):
    signal.signal(s, lambda sig, frame: sys.exit())

if __name__ == "__main__":
    main()