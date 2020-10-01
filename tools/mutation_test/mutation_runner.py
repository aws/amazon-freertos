#!/usr/bin/env python3

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
<<<<<<< HEAD
=======
import time
>>>>>>> master
from pathlib import Path
from typing import List

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
    """ Executes a subprocess by calling `flash_command` and reads serial output from `port`.

    `flash_command` is a str shell command that will be fed into a subprocess.

    `port` is the port to read serial output from.

    `timeout` is the max time before ending read process.
    """
<<<<<<< HEAD
=======
    # begin recording time
    time_begin = time.time()
>>>>>>> master
    try:
        subprocess.check_call(flash_command, shell=True)
    except subprocess.CalledProcessError as e:
        print(e)
        raise CompileFailed("Failed to compile")
<<<<<<< HEAD
=======
    time_finish_build = time.time()
    utils.yellow_print(f"Build Time: {time_finish_build - time_begin:.2f} seconds")
>>>>>>> master

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
<<<<<<< HEAD
    os.system('clear')
=======
    time_finish_read = time.time()
    utils.yellow_print(f"Serial Monitor Time: {time_finish_read - time_finish_build:.2f} seconds")
    utils.yellow_print(f"Build and Read Time: {time_finish_read - time_begin:.2f} seconds")
>>>>>>> master
    return output, final_flag         

def percentage(part, whole):
    """
    Returns percentage with 2 decimals. If `whole` <= 0, returns -1.00.
    """
    if whole <= 0:
        return -1.00
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

def get_expected_catch(test_to_lines: dict, line: int) -> List[str]:
    """ Returns a list of test that is expected to cover `line`.

    `test_to_lines`: dictionary containing mapping from test to line ranges that it covers

    `line`: the line to check.
    """
    tests_expected_to_catch = []
    for test in test_to_lines:
        for line_range in test_to_lines[test]:
            if line in range(line_range[0], line_range[1]):
                tests_expected_to_catch.append(test)
    return tests_expected_to_catch

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

def run_task(task, args, config):
    """ Runs a mutation testing task with settings in `args` and `config`.

    Collects results and produces CSV data.
    """
    flash_command = config['flash_command']

    port = args.port if args.port else utils.get_default_serial_port()
    os.environ['PORT'] = port
    timeout = int(args.timeout)
    csv = args.csv
    rng = random.Random(args.seed)

    mutation = mutator.Mutator(src=task['src'], mutation_patterns=task['patterns'], rng=rng)
    mutations_list = mutation.generateMutants(mutants_per_pattern=
                (task['mutants_per_pattern'] if 'mutants_per_pattern' in task else None),
                                              random=args.randomize)

    data_record = []
    trials = []
    test_to_kills = {}
    run_cnt = 0
    nc = 0
    total_failures = 0
    mutant_cnt = int(args.mutants)

    failures_per_pattern = {}
    total_per_pattern = {}
    for mp in mutation.getPatterns():
        if mp not in failures_per_pattern:
            failures_per_pattern[mp] = 0
        if mp not in total_per_pattern:
            total_per_pattern[mp] = 0

    # outer try is for finally generating csv if automation stops early
    try:
        for occurrence in mutations_list:
            if run_cnt == mutant_cnt:
                break
<<<<<<< HEAD
=======

>>>>>>> master
            mp = occurrence.pattern
            # mutate the code
            utils.yellow_print(occurrence)
            original_line, mutated_line = mutation.mutate(occurrence)
<<<<<<< HEAD
            file_changed = occurrence.file
=======
            file_changed = occurrence.file.rstrip(".old")
>>>>>>> master
            line_number = occurrence.line
            # try is for catching compile failure to continue execution
            try:
                # cmake, build, flash, and read
                output, final_flag = flash_and_read(port, timeout, flash_command)

                # reaching here means success, so change counters
                run_cnt += 1
                total_per_pattern[mp] += 1

                # tests expected to catch
                tests_expected_to_catch = "N/A"
                if args.line_coverage:
                    tests_expected_to_catch = ",".join(
                        get_expected_catch(args.line_coverage, int(line_number)))

                # mutant_status can either be "FAIL", "PASS", "CRASH", "TIMEOUT"
                mutant_status = "FAIL"
                if final_flag == FLAGS.PassFlag:
                    utils.red_print("Mutant is Alive")
                    utils.red_print("Tests that are expected to catch this mutant are: \n{}"
                                        .format(tests_expected_to_catch))
                    mutant_status = "PASS"
                else:
                    failures_per_pattern[mp] += 1
                    total_failures += 1
                    utils.green_print("Mutant is Killed")
                if final_flag == FLAGS.CrashFlag:
                    mutant_status = "CRASH"
                elif final_flag == "TIMEOUT":
                    mutant_status = "TIMEOUT"

                # Analyze the output to count per test failures
                results = re.findall(TestRegEx, output)
                for group, test, result in results:
                    if (group, test) not in test_to_kills:
                        test_to_kills[(group, test)] = (1, 1) if result == 'FAIL' else (0, 1)
                    else:
                        kills, total = test_to_kills[(group, test)]
                        test_to_kills[(group, test)] = ((kills + 1, total + 1) if result == 'FAIL' 
                                                        else (kills, total + 1))           

                # Add result to CSV queue
                trials.append({'file': file_changed,'line':line_number, 'original':original_line, 'mutant':mutated_line, 
                                'result':"{}/KILLED".format(mutant_status) if mutant_status != "PASS" else "PASS/LIVE", 
                                'expected_catch': tests_expected_to_catch})
                utils.yellow_print("Successful Mutant Runs: {}/{}".format(run_cnt, mutant_cnt))
            except CompileFailed:
                utils.yellow_print("Cannot compile, discard and move on")
                nc += 1
            finally:
                mutation.restore()
    except:
        traceback.print_exc()
        raise
    finally:
        mutation.cleanup()
        # calculate mutant score
        score = percentage(total_failures, run_cnt)
        utils.yellow_print("Score: {}%".format(score))
        utils.yellow_print("Alive: {} Killed: {} Mutants: {} No-Compile: {} Attempted Runs: {}"
                .format(run_cnt - total_failures, total_failures, run_cnt, nc, run_cnt + nc))
        trials.append({'file': "RESULTS:", 'line': "{} NO-COMPILE".format(nc) , 'mutant':"SCORE", 
                    'original':"{} KILLED/{} MUTANTS".format(total_failures, run_cnt),'result':"{}%".format(score)})

        # aggregate pass/fail counts for each found test in test group
        aggregates = []
        for group, test in test_to_kills:
            kills, total = test_to_kills[(group, test)]
            aggregates.append({'Group': group,
                               'Test': test,
                               'Fails': kills,
                               'Passes': total - kills,
                               'Total': total})
        
        # pattern comparison
        for mp in total_per_pattern:
            data_record.append(
                {'pattern': "{} => {}".format(mp.pattern, mp.transformation),
                 'failures': failures_per_pattern[mp],
                 'total': total_per_pattern[mp],
                 'percentage': float(percentage(failures_per_pattern[mp], total_per_pattern[mp])) 
                                        * 0.01 if total_per_pattern[mp] > 0 else 2})

        # log to csv
        if csv:
            csv_path = os.path.join(dir_path, "csvs/{}/{}".format(current_date, current_time))
            pattern_csv = os.path.join(csv_path, "{}_pattern_comparison.csv".format(task['name']))
            trials_csv = os.path.join(csv_path, "{}_mutants_created.csv".format(task['name']))
            per_test_csv = os.path.join(csv_path, "{}_test_aggregates.csv".format(task['name']))
            to_csv(pattern_csv, ['pattern', 'failures', 'total', 'percentage'], data_record)
            to_csv(trials_csv, ['file', 'line', 'original', 'mutant', 'result', 'expected_catch'], trials)
            to_csv(per_test_csv, ['Group', 'Test', 'Fails', 'Passes', 'Total'], aggregates)

def coverage_main(args, config):
    """ Function that is called when user specified `mutation_runner.py coverage` from cmd

    Runs coverage tool from coverage.py
    """
    coverage.run_coverage(args, config)

def mutation_main(args, config):
    """ Function that is called when user specifies `mutation_runner.py start` from cmd

    Runs every task in configuration json.
    """
    os.chdir(dir_path)
    if args.jobfile:
        with open(args.jobfile, 'r') as f:
            jobfile = ast.literal_eval(f.read())
            args.port = jobfile['port']
            args.mutants = int(jobfile['mutants'])
            args.timeout = int(jobfile['timeout'])
            args.csv = jobfile['csv']
            args.seed = jobfile['seed']
            args.randomize = jobfile['randomize']
    if args.line_coverage:
        with open(args.line_coverage) as f:
            args.line_coverage = json.loads(f.read())

    # change to root path
    os.chdir(root_path)
    # create a rng for this run
    if not args.seed:
        args.seed = random.randrange(sys.maxsize)
    utils.yellow_print("Current test seed is: {}".format(args.seed))

    time_begin = time.time()
    utils.yellow_print("Running tasks")
    for task in config['tasks']:
        time_task_begin = time.time()
        utils.yellow_print("Running task: {}".format(task['name']))

        # Generate test runner to run only on those test groups
        utils.yellow_print("Generating test runner based on supplied test groups...")
        backup = generate_test_runner(task['test_groups'])

        # set default mutations if patterns is equal to "all"
        if type(task['patterns']) == str:
             task['patterns'] = mutator.pattern_dict[task['patterns']]
        try:   
            run_task(task, args, config)
        except:
            traceback.print_exc()
            raise
        finally:
            # restore aws_test_runner.c
            shutil.copy(backup, os.path.splitext(backup)[0])
            os.remove(backup)
            time_task_end = time.time()
            utils.yellow_print(f"Task Time: {time_task_end - time_task_begin:.2f} seconds")
    
    time_end = time.time()
    utils.yellow_print(f"Total Time: {time_end - time_begin:.2f} seconds")

    create_jobfile(mutants=args.mutants,
        port=args.port,
        timeout=args.timeout,
        csv=args.csv,
        seed=args.seed,
        randomize=args.randomize)

def main():
    """ main method that is ran during execution

    In charge of parsing commands and arguments.

    Also loads json from /configs/.
    """
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(title='commands',
                                       description='valid commands',
                                       help='additional help')
    subparsers.required = True
    subparsers.dest = 'command'

    # create parser for coverage tool
    parser_coverage = subparsers.add_parser('coverage')
    parser_coverage.set_defaults(func=coverage_main)
    parser_coverage.add_argument(
        '--src_config', '-s',
        help='Select the test config file to use eg. -s wifi',
        required=True
    )
    parser_coverage.add_argument(
        '--output', '-o',
        help="The output path of the line coverage map file (JSON)",
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
        help="Select the test config file to use eg. -s wifi",
        required=True
    )
    parser_mutation.add_argument(
        '--mutants', '-m',
        help="Max number of mutants to create from all tasks (cap)",
        default=-1
    )
    parser_mutation.add_argument(
        '--line_coverage', '-l',
        help="path to JSON file containing a map from line number to list of tests expected to \
                test that line",
        default=None
    )
    parser_mutation.add_argument(
        '--randomize', '-r',
        help="Toggle randomization. Each mutant will be a random pattern at a random line",
        action="store_true",
        default=False
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