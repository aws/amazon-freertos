#!/usr/bin/env python
"""
TestRail MISRA compliance report

Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.

Modified by Hermann Yavorskyi <hermann.yavorskyi@cypress.com>.
"""

__version__ = "$Revision: 2.0 $"
__author__ = "Serhiy Kyrychenko <serh@cypress.com>, updated to mbed Bohdan Yaremkiv <yarb@cypress.com>"

import os
import logging
import sys
import time
import argparse
import json
import datetime

import script_utils.testrail as testrail

# TS and TC additional fileds
TS_OS = "os"
TC_IS_FAILED = "is_failed"
TC_IS_SKIPPED = "is_skipped"

TC_DESCR = "description"
TC_AUTHOR = "author"
TC_PRECONDITIONS = "preconditions"
TC_STEPS = "steps"
TC_EXP_OUTPUT = "expected_output"
TC_TR_TEMPLATE = "Cypress_Template"
TC_TYPE = "Automated"
TC_MESS = "mess"

TC_MISRA = {
    TC_DESCR: "MBED-OS GreenTea Tests",
    TC_PRECONDITIONS: "-",
    TC_STEPS: "Automated tests GreenTea",
    TC_EXP_OUTPUT: "GreenTea tests defined by ARM"
}

# Test machines os recognition
TM_NAME_COMMON_TEXT = "_tm-"
TM_OSs = {"windows10": "-w1064-v",
          "windows7": "-w764-v",
          "ubuntu16": "-u1664-v",
          "ubuntu18": "-u1864-v",
          "mac10_13": "-mac13-v",
          "mac10_14": "-mac14-v"}

# TestRail configutation parameters
TR_CONF_GROUP_NAME = "Operating Systems (Autom. Regr.)"

logger = logging.getLogger(__name__)


def logger_init(log_level):
    """
    Configure logger.

    :param log_level: Log level
    :return: None
    """
    logger_log_name = "{0}.{1}".format(
        os.path.splitext(os.path.basename(__file__))[0], 'log')
    logger_log_path = os.path.join(os.path.dirname(
        os.path.realpath(sys.argv[0])), logger_log_name)

    # Clean log file
    with open(logger_log_path, "w"):
        pass

    logging.basicConfig(filename=logger_log_path,
                        filemode='w',
                        format='%(asctime)s,%(msecs)d: %(filename)s:%(lineno)s: %(funcName)s: %(levelname)s: \t\t\t%(message)s',
                        datefmt='%Y-%m-%d %H:%M:%S',
                        level=log_level
                        )


def tr_connent(tr_url, tr_user, tr_password):
    """
    Connect to TestRail.

    :param tr_url: url to TestRail server
    :param tr_user: TestRail user
    :param tr_password: password for TestRail user
    :return: tr_connection: connection object
    """
    tr_connection = testrail.APIClient(tr_url)
    tr_connection.user = tr_user
    tr_connection.password = tr_password

    return tr_connection


def get_prj_data(tr_connection, project_name="", project_id=0):
    """
    Get project data. 
    'project_name' and 'project_id' cannot be defined in one function call.

    :param tr_connection: connection to TestRail
    :param project_name: project name
    :param project_id: project ID
    :return: if 'project_name' is defined - project ID will be returned;
             if 'project_id' is defined - project name will be returned;
             if 'project_name' and 'project_id' is not defined - whole project data will be returned;
             if 'project_name' and 'project_id' is defined - exception raised.
    """

    # Only one project parameter should be defined
    if project_name and project_id:
        raise Exception("Only one project parameter should be defined.")

    project_data = ""

    # Get all projects
    projects = tr_connection.send_get("get_projects")

    if project_name or project_id:
        for project in projects:
            if project_name:
                if project["name"] == project_name:
                    project_data = project["id"]
                    break
            if project_id:
                if project["id"] == project_id:
                    project_data = project["name"]
                    break
    else:
        project_data = projects

    return project_data


def get_ts_id(tr_connection, prj_id, ts_name):
    ts_id = 0

    testsuites = tr_connection.send_get("get_suites/{0}".format(prj_id))
    for testsuite in testsuites:
        if testsuite["name"] == ts_name:
            ts_id = testsuite["id"]
            break

    return ts_id


def add_ts(tr_connection, prj_id, ts_name, ts_desc):

    ts_id = get_ts_id(tr_connection, prj_id, ts_name)

    if not ts_id:
        ts_data = tr_connection.send_post("add_suite/{0}".format(prj_id),
                                          {"name": ts_name,
                                           "description": ts_desc})
        ts_id = ts_data["id"]

    return ts_id


def get_section_id(tr_connection, prj_id, ts_id, section_name):
    section_id = 0

    sections = tr_connection.send_get(
        "get_sections/{0}&suite_id={1}".format(prj_id, ts_id))
    for section in sections:
        if section["name"] == section_name:
            section_id = section["id"]
            break

    return section_id


def get_section(tr_connection, prj_id, ts_id, section_name=""):
    section_data = []

    sections = tr_connection.send_get(
        "get_sections/{0}&suite_id={1}".format(prj_id, ts_id))

    if section_name:
        for section in sections:
            if section["name"] == section_name:
                section_data = section
                break
    else:
        section_data = sections

    return section_data


def add_section(tr_connection, prj_id, ts_id, section_name, section_desc=""):
    section_id = get_section_id(tr_connection, prj_id, ts_id, section_name)

    if not section_id:
        section_data = tr_connection.send_post("add_section/{0}".format(prj_id),
                                               {"description": section_desc,
                                                "suite_id": ts_id,
                                                "name": section_name})
        section_id = section_data["id"]

    return section_id


def get_tc_templates_ids(tr_connection, prj_id):
    testcase_templates_ids = {}

    templates = tr_connection.send_get("get_templates/{0}".format(prj_id))
    for template in templates:
        testcase_templates_ids[template["name"]] = template["id"]

    return testcase_templates_ids


def get_tc_types_ids(tr_connection):
    testcase_types_ids = {}

    testcase_types = tr_connection.send_get("get_case_types")
    for type in testcase_types:
        testcase_types_ids[type["name"]] = type["id"]

    return testcase_types_ids


def get_tc_id(tr_connection, prj_id, ts_id, section_id, tc_name):
    tc_id = 0

    testcases = tr_connection.send_get(
        "get_cases/{0}&suite_id={1}&section_id={2}".format(prj_id, ts_id, section_id))
    for testcase in testcases:
        if testcase["title"] == tc_name:
            tc_id = testcase["id"]
            break

    return tc_id


def get_tc_tr_data(tr_connection, prj_id, ts_id, section_id, tc_name=""):
    tc_tr_data = {}

    testcases = tr_connection.send_get(
        "get_cases/{0}&suite_id={1}&section_id={2}".format(prj_id, ts_id, section_id))

    if tc_name:
        for testcase in testcases:
            if testcase["title"] == tc_name:
                tc_tr_data = testcase
                break
    else:
        tc_tr_data = testcases

    return tc_tr_data


def add_tc(tr_connection, ts_id, section_id,
           tc_name, tc_template_id, tc_tc_type_id, tc_description, tc_preconditions, tc_steps, tc_exp_output,
           tc_tr_data="get_from_api"):

    # Get testcase info from testrail in not passed as argument
    if tc_tr_data == "get_from_api":
        tc_tr_data = get_tc_tr_data(
            tr_connection, prj_id, ts_id, section_id, tc_name)

    if tc_tr_data:
        tc_id = tc_tr_data["id"]
    else:
        tc_id = 0

    if not tc_id:
        tc_data = tr_connection.send_post("add_case/{0}".format(section_id),
                                          {"title": tc_name,
                                           "template_id": tc_template_id,
                                           "type_id": tc_tc_type_id,
                                           "custom_description": tc_description,
                                           "custom_preconditions": tc_preconditions,
                                           "custom_steps": tc_steps,
                                           "custom_expected": tc_exp_output})
        tc_id = tc_data["id"]
        # if str(args.log) == "yes":
        # logging.info("Testcase {} added".format(tc_name))
    else:
        fields_to_update = {}
        if tc_template_id and tc_template_id != tc_tr_data["template_id"]:
            fields_to_update["template_id"] = tc_template_id

        if tc_tc_type_id and tc_tc_type_id != tc_tr_data["type_id"]:
            fields_to_update["type_id"] = tc_tc_type_id

        if tc_description and tc_description != tc_tr_data["custom_description"]:
            fields_to_update["custom_description"] = tc_description

        if tc_preconditions and tc_preconditions != tc_tr_data["custom_preconditions"]:
            fields_to_update["custom_preconditions"] = tc_preconditions

        if tc_steps and tc_steps != tc_tr_data["custom_steps"]:
            fields_to_update["custom_steps"] = tc_steps

        if tc_exp_output and tc_exp_output != tc_tr_data["custom_expected"]:
            fields_to_update["custom_expected"] = tc_exp_output

        if fields_to_update:
            tc_data = tr_connection.send_post(
                "update_case/{0}".format(tc_id), fields_to_update)

    return tc_id


def get_tp_id(tr_connection, prj_id, tp_name, is_completed=0):
    tp_id = 0

    testplans = tr_connection.send_get(
        "get_plans/{0}&is_completed{1}".format(prj_id, is_completed))
    for testplan in testplans:
        if testplan["name"] == tp_name:
            tp_id = testplan["id"]

    if not tp_id:
        tc_data = tr_connection.send_post(
            "add_plan/{0}".format(prj_id), {"name": tp_name})
        tp_id = tc_data["id"]

    return tp_id


def get_tp_entry(tr_connection, tp_id, tp_entry_name=""):
    tp_entry = 0

    testplan_entries = tr_connection.send_get("get_plan/{0}".format(tp_id))

    if tp_entry_name:
        for entry in testplan_entries["entries"]:
            if entry["name"] == tp_entry_name:
                tp_entry = entry
                break
    else:
        tp_entry = testplan_entries

    return tp_entry


def add_tp_entry(tr_connection, tp_id, ts_id, tp_entry_name, config_ids, runs):
    # Modified by yavo@cypress.com
    tp_entry = get_tp_entry(tr_connection, tp_id, tp_entry_name)

    if not tp_entry:
        if config_ids:
            tp_entry = tr_connection.send_post("add_plan_entry/{0}".format(tp_id),
                                               {"suite_id": ts_id,
                                                "name": tp_entry_name,
                                                "config_ids": config_ids,
                                                "runs": runs})
        else:
            tp_entry = tr_connection.send_post("add_plan_entry/{0}".format(tp_id),
                                               {"suite_id": ts_id,
                                                "name": tp_entry_name})
    # Handling new configuration
    configuration_extists = False
    for run in tp_entry["runs"]:
        if set(run['config_ids']) == set(config_ids):
            configuration_extists = True
            break
    if not configuration_extists:
        if config_ids:
            tp_entry = tr_connection.send_post("add_plan_entry/{0}".format(tp_id),
                                               {"suite_id": ts_id,
                                                "name": tp_entry_name,
                                                "config_ids": config_ids,
                                                "runs": runs})
        else:
            tp_entry = tr_connection.send_post("add_plan_entry/{0}".format(tp_id),
                                               {"suite_id": ts_id,
                                                "name": tp_entry_name})

    tplan_conf_run_ids = {}
    for run in tp_entry["runs"]:
        for config_id in run["config_ids"]:
            tplan_conf_run_ids[config_id] = run["id"]

    return tplan_conf_run_ids


def get_conf_group_id(tr_connection, prj_id, conf_gr_name):
    conf_group_id = 0

    configurations = tr_connection.send_get("get_configs/{0}".format(prj_id))
    for config_group in configurations:
        if config_group["name"] == conf_gr_name:
            conf_group_id = config_group["id"]
            break

    return conf_group_id


def add_conf_group(tr_connection, prj_id, conf_gr_name):
    conf_group_id = get_conf_group_id(tr_connection, prj_id, conf_gr_name)

    if not conf_group_id:
        conf_group_data = tr_connection.send_post("add_config_group/{0}".format(prj_id),
                                                  {"name": conf_gr_name})
        conf_group_id = conf_group_data["id"]

    return conf_group_id


def get_conf(tr_connection, prj_id, conf_group_id, conf_name):
    conf_id = 0

    configurations = tr_connection.send_get("get_configs/{0}".format(prj_id))
    for config_group in configurations:
        if config_group["id"] == conf_group_id:
            for config in config_group["configs"]:
                if isinstance(config, dict):
                    if config["name"] == conf_name:
                        conf_id = config["id"]
                        break

    return conf_id


def add_conf(tr_connection, prj_id, conf_group_id, conf_name):
    conf_id = get_conf(tr_connection, prj_id, conf_group_id, conf_name)

    if not conf_id:
        config_data = tr_connection.send_post("add_config/{0}".format(conf_group_id),
                                              {"name": conf_name})
        conf_id = config_data["id"]

    return conf_id


def add_result_for_cases(tr_connection, run_id, tc_results):
    # if str(args.log) == "yes":
    # print('####', run_id, tc_results)
    tr_connection.send_post("add_results_for_cases/{0}".format(run_id),
                            {"results": tc_results})


def get_users_ids(tr_connection):
    users_ids = {}

    users = tr_connection.send_get("get_users")
    for user in users:
        users_ids[user["name"]] = user["id"]
    return users_ids


def parse_junit_xml(file_report):
    """
    Function to get test execution information.  

    :param xml_file_path: the path to .xml file to update test case name
    :return: parsed_content: dictionary with test cases parameters
    """
    ts_parsed_content = {}
    tc_parsed_content = {}

    # Test report is not generated if build/compile errors are appeared
    try:
        file = open(file_report, 'r')
    except:
        user_readable_error('Check hardware connection(3)')
        # Exit from script in case of hw issue
        exit(1)

    all_tests = []
    test_suits = {}

    count = 0
    test_case_count = 0
    failed_test_case_count = 0
    for line in file.readlines():
        count += 1
        words = line.split('|')
        if len(words) == 10:
            if (line.find('target') < 0) and (line.find('-----') < 0):
                new_words = []
                final_words = [None, []]
                for word in words:
                    new_words.append(word.replace(" ", ""))
                test_case_count += 1
                if new_words[7] == 'OK':
                    final_words[0] = 'PASS'
                    final_words[1].append('PASS: ' + new_words[4])
                elif new_words[7] == 'ERROR':
                    failed_test_case_count += 1
                    final_words[0] = 'FAIL'
                    final_words[1].append('FAIL: ' + new_words[4])
                elif new_words[7] == 'FAIL':
                    final_words[0] = 'FAIL'
                    final_words[1].append('FAIL: ' + new_words[4])
                elif new_words[7] == 'SKIPPED':
                    final_words[0] = 'SKIPPED'
                    final_words[1].append('SKIPPED: ' + new_words[4])
                else:
                    final_words[0] = 'FAIL'
                    final_words[1].append('FAIL: ' + new_words[4])

                test_suits[str(new_words[4]) + ' -- ' +
                           str(new_words[1])] = final_words

    data = test_suits

    # Check case when all test cases are failed, it may cased by serious hw or sw issue.
    # Fail pipeline job
    if test_case_count == failed_test_case_count:
        user_readable_error(
            'Check HW or SW, reason - all test cases are failed')
        exit(1)

    # It means that testreport is empty, exit from script
    if count == 5:
        user_readable_error('Check hardware connection(1)')
        exit(1)

    # Output: {'name': 'Bob', 'languages': ['English', 'Fench']}
    if str(args.log) == "yes":
        print(data)

    tc_names = []
    for k, v in data.items():
        tc_names.append(k)
        for kitem in v[1]:
            if str(args.log) == "yes":
                print(kitem)

    # Testsuite
    # Remove test machine name from testcase name
    ts_os = "windows10"
    ts_name = 'sut_mbed_greentea'
    if str(args.log) == "yes":
        logging.info('Working with "{0}" test suite'.format(ts_name))

    # Init dict with parsed results
    ts_parsed_content[ts_name] = {}
    tc_parsed_content[ts_name] = {}

    for tc_name in tc_names:

        ts_parsed_content[ts_name][TS_OS] = ts_os
        tc_parsed_content[ts_name][tc_name] = {}
        tc_parsed_content[ts_name][tc_name][TC_IS_FAILED] = (
            data[tc_name][0] == 'FAIL')
        tc_parsed_content[ts_name][tc_name][TC_IS_SKIPPED] = (
            data[tc_name][0] == 'SKIPPED')

        tc_parsed_content[ts_name][tc_name][TC_AUTHOR] = ''
        # TC_MISRA[TC_DESCR] # Fix BSP-857
        tc_parsed_content[ts_name][tc_name][TC_DESCR] = 'http://git-ore.aus.cypress.com/repo/bsp_csp/-/jobs/' + \
            str(args.job_id)
        tc_parsed_content[ts_name][tc_name][TC_PRECONDITIONS] = TC_MISRA[TC_PRECONDITIONS]
        tc_parsed_content[ts_name][tc_name][TC_STEPS] = TC_MISRA[TC_STEPS]
        tc_parsed_content[ts_name][tc_name][TC_EXP_OUTPUT] = TC_MISRA[TC_EXP_OUTPUT]

        tc_parsed_content[ts_name][tc_name][TC_MESS] = ' '.join(
            data[tc_name][1])
        if str(args.log) == "yes":
            print('***', tc_parsed_content[ts_name][tc_name][TC_MESS])

    return ts_parsed_content, tc_parsed_content


def user_readable_error(message='Default error'):
    print('>>')
    print('>> >>')
    print('>> >> >> >> ' + str(message) + ' << << << <<')
    print('>> >>')
    print('>>')


if __name__ == "__main__":

    argParser = argparse.ArgumentParser()
    argParser.add_argument("root_dir", help="path to the git folder")
    argParser.add_argument("--tprj", help='TestRail project name')
    argParser.add_argument("--tp", help='test plan name')
    argParser.add_argument("--ts", help='test suite name')
    argParser.add_argument(
        "--job_id", help='id (for example: 341199) of GitLab job')
    argParser.add_argument("--log", help='yes or no')

    try:
        args = argParser.parse_args()
        root_dir = args.root_dir
    except Exception as ex:
        print(ex)
        sys.exit()

    if args.tprj:
        PROJECT_NAME = args.tprj
    else:
        PROJECT_NAME = "DEMO_TEST_and_TRY"

    if args.tp:
        TP_NAME = args.tp
    else:
        TP_NAME = "build_001"

    if args.ts:
        TS_NAME = str(args.ts)
    else:
        TS_NAME = "default_suite"

    # Count script execution time
    start_time = time.time()
    logger_init(logging.DEBUG)
    if str(args.log) == "yes":
        logging.info("Start script execution time: {0}".format(start_time))

    if str(args.log) == "yes":
        print('PROJECT_NAME=', PROJECT_NAME)
        print('TP_NAME=', TP_NAME)
        print('TS_NAME=', TS_NAME)

    file_report = os.path.join(root_dir, 'greentea.txt')

    # Verify if test report is exists
    if (os.path.isfile(file_report)) == False:
        user_readable_error('Check hardware connection(2)')
        exit(1)

    TS_DESC = "Automated GreenTea tests"

    # 1. Login to Testrails  now is due to the LDAP credentials
    # Login: your_initials
    # Password: your_ldap_pw
    # https://testrail.aus.cypress.com/
    # 2. Please don't use password in open view in the  scripts (now it’s your ldap credentials).
    # Please  use API key  for that. Creation is available  under ->My setting->API Keys->Add Key and don’t forget  to click `save setting`
    # Login/Password for script:=
    # Login: YourFirstName.YourLastName@cypress.com
    # Password: API_key generated under your account
    # Example:
    # Login: roman.mikhno@cypress.com
    # Password: a8pfakeYZKrwvwoFake-1Wm/fake45FSIp97sWmB
    # 3. Please start to use secure connection https://testrail.aus.cypress.com instead of http://testrail.aus.cypress.com
    # P.S. Any issue (with last update) please contact TestRailHelp@cypress.com or fill in jira against DEVOPS.

    tr_connection = tr_connent("http://testrail.aus.cypress.com",
                               "bohdan.yaremkiv@cypress.com", "siFb3NyD5Zr5GhF1z.95-TeXGjBljaPoqumrEVQ4J")

    # Get project id
    prj_id = get_prj_data(tr_connection, PROJECT_NAME)

    # Get/add testsuite. Get testsuite id.
    ts_id = add_ts(tr_connection, prj_id, TS_NAME, TS_DESC)

    # Get/add testplan. Get testplan id.
    tp_id = get_tp_id(tr_connection, prj_id, TP_NAME)
    if tp_id == 0:
        raise Exception("Testplan is not found.")

    # Create testcases
    results_per_os = {}
    for os_name in TM_OSs:
        results_per_os[os_name] = []

    # Get testcase templates ids
    tc_templates_with_ids = get_tc_templates_ids(tr_connection, prj_id)

    tc_template_id = tc_templates_with_ids[TC_TR_TEMPLATE]
    if not tc_template_id:
        err_message = "Testcase template {0} not found.".format(TC_TR_TEMPLATE)
        if str(args.log) == "yes":
            logging.error(err_message)
        raise Exception(err_message)

    # Get testcase types ids
    tc_types_with_ids = get_tc_types_ids(tr_connection)
    tc_type_id = tc_types_with_ids[TC_TYPE]
    if not tc_type_id:
        err_message = "Testcase type {0} not found.".format(TC_TYPE)
        if str(args.log) == "yes":
            logging.error(err_message)
        raise Exception(err_message)

    # Get users ids
    users_ids = get_users_ids(tr_connection)

    # for filename in [f for f in filenames if f.endswith(".xml")]:
    tsuites, tsuites_with_tcases = parse_junit_xml(file_report)

    for tsuite in tsuites_with_tcases:
        # Create testsuite (section in TestRail)
        try:
            if str(args.log) == "yes":
                print('###', 'p', prj_id, 'id', ts_id, 't', tsuite)
            sect_id = add_section(tr_connection, prj_id, ts_id, tsuite)
        except testrail.APIError as e:
            if str(args.log) == "yes":
                logging.error("Error adding {0} section.".format(tsuite))
                logging.error("Error message: {0}.".format(str(e)))
            time.sleep(1)
            sect_id = add_section(tr_connection, prj_id, ts_id, tsuite)

        # Get section testcases with info
        section_tcases_data = []
        section_tcases_data = get_tc_tr_data(
            tr_connection, prj_id, ts_id, sect_id)

        # Create testcases
        for tcase in tsuites_with_tcases[tsuite]:
            if str(args.log) == "yes":
                print('### tcase = ', tcase,
                      tsuites_with_tcases[tsuite][tcase][TC_MESS])
            # Get testcase data
            tcase_data = {}
            for testcase in section_tcases_data:
                if testcase["title"] == tcase:
                    tcase_data = testcase
                    break

            # Create testcase
            try:
                tc_id = add_tc(tr_connection, ts_id, sect_id, tcase, tc_template_id, tc_type_id,
                               tsuites_with_tcases[tsuite][tcase][TC_DESCR],
                               tsuites_with_tcases[tsuite][tcase][TC_PRECONDITIONS],
                               tsuites_with_tcases[tsuite][tcase][TC_STEPS],
                               tsuites_with_tcases[tsuite][tcase][TC_EXP_OUTPUT],
                               tcase_data)
            except testrail.APIError as e:
                if str(args.log) == "yes":
                    logging.error(
                        "Error adding {0} testcase from testsuite {1}.".format(tcase, tsuite))
                    logging.error("Error message: {0}.".format(str(e)))
                time.sleep(5)
                tc_id = add_tc(tr_connection, ts_id, sect_id, tcase, tc_template_id, tc_type_id,
                               tsuites_with_tcases[tsuite][tcase][TC_DESCR],
                               tsuites_with_tcases[tsuite][tcase][TC_PRECONDITIONS],
                               tsuites_with_tcases[tsuite][tcase][TC_STEPS],
                               tsuites_with_tcases[tsuite][tcase][TC_EXP_OUTPUT],
                               tcase_data)

            # Define TestRail testcase status
            if tsuites_with_tcases[tsuite][tcase][TC_IS_FAILED]:
                tc_status = 5  # Retest
            elif tsuites_with_tcases[tsuite][tcase][TC_IS_SKIPPED]:
                tc_status = 6  # Skipped
            else:
                tc_status = 1  # Pass

            # "status_id": 2, - Blocked
            # "status_id": 5, - Fail
            # "status_id": 1, - Pass
            #             4, - Retest
            #             6, - Skipped

            # Get User
            user_name = tsuites_with_tcases[tsuite][tcase][TC_AUTHOR]
            if user_name in users_ids:
                user_id = users_ids[user_name]
            else:
                user_id = ""
                if str(args.log) == "yes":
                    logging.warning("Testcase {0} script from testsuise {1} does not have an author.".format(tcase,
                                                                                                             tsuite))

            # Add testcase results
            results_per_os[tsuites[tsuite]["os"]].append({"case_id": tc_id,
                                                          "status_id": tc_status,
                                                          "comment": tsuites_with_tcases[tsuite][tcase][TC_MESS],
                                                          "defects": "",
                                                          "assignedto_id": user_id
                                                          })

    logging.basicConfig(level=logging.INFO)
    if str(args.log) == "yes":
        logging.info("Testcase")

    # Remove OS without results
    for os_name in [os_name for os_name in results_per_os if not results_per_os[os_name]]:
        results_per_os.pop(os_name)

    # Create configurations for automated regression if not exists. Get configuration ids.
    conf_gr_id = add_conf_group(tr_connection, prj_id, TR_CONF_GROUP_NAME)
    conf_ids = {}
    for os_name in TM_OSs:
        conf_id = add_conf(tr_connection, prj_id, conf_gr_id, os_name)
        conf_ids[os_name] = conf_id

    # Create run configurations per OS
    run_configs = []
    for os_name in results_per_os:
        case_ids = []
        for result in results_per_os[os_name]:
            case_ids.append(result["case_id"])
        run_configs.append({"include_all": False,
                            "case_ids": case_ids,
                            "config_ids": [conf_ids[os_name]]})

    # Get/add testplan run
    conf_ids_values = [v for v in conf_ids.values()]
    try:
        tp_conf_run_ids = add_tp_entry(
            tr_connection, tp_id, ts_id, TS_NAME, conf_ids_values, run_configs)
    except testrail.APIError as e:
        if str(args.log) == "yes":
            logging.error("Error adding testplan runs.")
            logging.error("Error message: {0}.".format(str(e)))
        time.sleep(5)
        tp_conf_run_ids = add_tp_entry(
            tr_connection, tp_id, ts_id, TS_NAME, conf_ids_values, run_configs)

    # Add result for testcase
    for os_name in results_per_os:
        try:
            add_result_for_cases(
                tr_connection, tp_conf_run_ids[conf_ids[os_name]], results_per_os[os_name])
        except testrail.APIError as e:
            if str(args.log) == "yes":
                logging.error("Error adding testcases results.")
                logging.error("Error message: {0}.".format(str(e)))
            time.sleep(5)
            add_result_for_cases(
                tr_connection, tp_conf_run_ids[conf_ids[os_name]], results_per_os[os_name])

    # Add link for results on Test Rail
    user_readable_error('Testrail reporting complete.')

    # Add link for test results on Test Rail
    print('>>')
    print('>> >>')
    print('>> >> >> >> Test results link: http://testrail.aus.cypress.com/index.php?/runs/view/' +
          str(tp_conf_run_ids[conf_ids[os_name]]))
    print('>> >>')
    print('>>')

    exit(0)
