"""
Implements TestRailUploader class which upploads TestReport to TestRail.
Command line argumentes described in help message. You have to provide
USERNAME, API_KEY (Creation is available  under ->My setting->API Keys
->Add Key and don’t forget  to click `save setting`).
"""
import time
import argparse
import sys
import os
import json
from pathlib import Path
from script_utils.lava_log_scrapper import get_lava_log_by_job_id

from script_utils.testrail_gt import tr_connent, get_prj_data, \
    add_ts, get_tp_id, \
    add_section, get_tc_templates_ids, \
    get_users_ids, get_tc_types_ids, get_tc_tr_data, \
    add_tc, add_conf_group, add_conf, add_tp_entry, \
    add_result_for_cases

import script_utils.testrail as testrail
from script_utils.test_report import TestReport
from parsers.lava_parser import LavaParser
from parsers.xml_parser import XMLParser
from script_utils.lava_log_scrapper import get_lava_log_by_job_id

DOMAIN = "http://testrail.aus.cypress.com"

# Command line argument has higher priority than this constant, but might be
# useful to set it and then no need to specify it as argument.
PROJECT_NAME = "TFM_TEST_YAVO"
TC_TR_TEMPLATE = "Cypress_Template"
TC_TYPE = "Automated"


class TestRailUploader:
    """
    Uploader of the results of tests in form of TestReport object to TestRail.

    Args:
        conection_credentials (:obj:'List' of str): TestRail username
        (must be valid TestRail login <name.surname@cypress.com>), API key,
        domain (http://testrail.aus.cypress.com).
        test_report (:obj: TestReport) to be uploaded
        project_name (str): Name of the TestRail project that uploader will work
        with (developed for TFM).
        tcase_template (str): Template of the Test Case content on TestRail (
        developed for Cypress_Template).
        tsuite_type (str): Test Suite name should contain it`s type (TESTRAIL
        USAGE POLICY 002-27268 *A), default is Automated.
        build_id (str): Id of CI build. Definition of specific run, where the
        test were run.

    Attributes:
        connection (:obj:'APIClient'): Connection to TestRail.

        project_id (str): TestRail id of the project with name project_name.
        test_suites_ids (:obj:`Dict` of str:str): Mapping of test suites` names
            to TestRail ids.
        test_plan_id (str): Id of the project`s test plan on TestRail.
        tcase_template_id (str): Id of the testcase template on TestRail.

        Described in Args section:
        project_name(str)
        tsuite_type (str)
        build_id (str)
    """

    def __init__(self, connection_credentials, test_report, build_id, version,
                 project_name,
                 tcase_template="Cypress_Template",
                 tsuite_type="Automated"):
        self.version = version
        self.connection = connection_credentials
        self.build_id = build_id
        self.project_name = project_name
        self.test_report = test_report
        self.test_suites_type = tsuite_type
        # Obtaining ids of objects we will work with
        self.project_id, self.test_suites_ids,\
            self.test_plan_id = self._get_testrail_ids(self.test_suites_type)

        self.tcase_template_id = tcase_template
        self.tsuites_type_id = tsuite_type

        self.assignee = connection_credentials[0]

    @property
    def assignee(self):
        """
        assignee property getter.

        Returns:
            (str): Test case assignee (obtained from the uploader`s username)
        """
        return self._assignee

    @assignee.setter
    def assignee(self, user_name):
        """
        Sets assignee property.

        Args:
            user_name (str): Username of the reporter (all cases will be
            assigned to this person)
        """
        # Get users ids
        users_ids = get_users_ids(self.connection)

        # Get username from email
        user_name = " ".join(
            map(lambda x: x[0].upper()+x[1:],
                user_name.split("@")[0].split(".")))

        if user_name in users_ids:
            self._assignee = users_ids[user_name]
        else:
            self._assignee = ""

    @property
    def tsuites_type_id(self):
        """
        tsuite_type_id property getter

        Returns:
            (str): Id of specific test suite type (e.g. Automated)
        """
        return self._tsuites_type_id

    @tsuites_type_id.setter
    def tsuites_type_id(self, ts_type):
        """
        Sets tsuite_type_id property.

        Args:
            ts_type (str): Name of the test suite type (e.g. Manual/Automated)
        """
        ts_types_ids = get_tc_types_ids(self.connection)
        tsuite_type_id = ts_types_ids[ts_type]
        if not tsuite_type_id:
            raise ValueError("Testsuite type {ts_type} not found.")
        self._tsuites_type_id = tsuite_type_id

    @property
    def tcase_template_id(self):
        """
        tcase_template_id property getter.

        Returns:
            (str): Id of the template.
        """
        return self._tcase_template_id

    @tcase_template_id.setter
    def tcase_template_id(self, template):
        """
        Sets tcase_template_id property.

        Args:
            (str): Template name.
        """
        template_ids = get_tc_templates_ids(self.connection, self.project_id)
        tc_template_id = template_ids[template]
        if not tc_template_id:
            raise ValueError(f"Template {template} not found!")
        self._tcase_template_id = tc_template_id

    def _get_testrail_ids(self, ts_type):
        """
        For inner use. Gets the TestRail ids for test suites, test plan and project.

        Args:
            ts_type (str): test suite type.
        """
        try:
            project_id = get_prj_data(self.connection, self.project_name)
        except Exception as err:
            print("Error: Check your connection, VPN and project name")
        test_suites_ids = dict()
        for test_suites in self.test_report.test_suites_list:
            test_suites_ids[test_suites.name] = add_ts(
                self.connection, project_id,
                f"{test_suites.name}_{self.version}_{ts_type}",
                "This test suite created "
                "automatically based on the test "
                "suite from the test run results. ")
        test_plan_id = get_tp_id(
            self.connection, project_id, f"{self.build_id}")
        if test_plan_id == 0:
            raise Exception(
                f"Testplan {self.build_id} is not found and could"
                " not be created.")
        return project_id, test_suites_ids, test_plan_id

    @property
    def connection(self):
        """connection attribute getter.

        Returns:
            (:obj:'APIClient'): connection to TestRail.
        """
        return self._connection

    @connection.setter
    def connection(self, credentials):
        """
        Connection attribute setter.

        Args:
            domain (str): link to TestRail.
            username (str): username of responsible for test report uploading.
                (will be displayed on TestRail, must be valid TestRail login
                <name.surname@cypress.com>).
            key (str): API key, can be generated directly on
                TestRail (user settings).
        """
        username, key, domain = credentials
        try:
            self._connection = tr_connent(domain, username, key)
        except Exception as connection_error:
            raise RuntimeError(
                f"Error occured whe trying to connect to {domain}:"
                f"\n{connection_error}")

    def upload_results(self, configurations):
        """
        Upploads current TestReport to TestRail. All test suites in the test
        report will have the same configurations.

        Args:
            configurations (:obj:`Dict` of str:str): Mapping configuration
                group names to actucal configurations from this groups for this
                specific run.
        """
        configuration_ids = []
        for conf_group in configurations:
            group_id = add_conf_group(
                self.connection, self.project_id, conf_group)
            conf_id = add_conf(
                self.connection, self.project_id, group_id,
                configurations[conf_group])
            configuration_ids.append(conf_id)
        for test_suites in self.test_report.test_suites_list:
            results_per_suite = []
            for section in test_suites.test_suite_list:
                results_per_suite.extend(
                    self._create_sections_with_tcases(test_suites.name, section))
            # Create run configurations
            run_configs = []
            case_ids = [case["case_id"] for case in results_per_suite]
            run_configs.append({"include_all": True,
                                "case_ids": case_ids,
                                "config_ids": configuration_ids})
            config_identifier = "_".join(configurations.values())
            tp_conf_run_ids = add_tp_entry(
                self.connection, self.test_plan_id,
                self.test_suites_ids[test_suites.name],
                f"{test_suites.name}_{self.version}__{self.test_suites_type}_{config_identifier}",
                configuration_ids,
                run_configs)
            for id_of_run in set(tp_conf_run_ids.values()):
                try:
                    add_result_for_cases(
                        self.connection,
                        id_of_run,
                        results_per_suite)
                except testrail.APIError as e:
                    print(e, "\nRetrying...")
                    time.sleep(5)
                    add_result_for_cases(
                        self.connection,
                        id_of_run,
                        results_per_suite)

    def _create_sections_with_tcases(self, test_suites_name, section):
        """
        For inner use. Create/Update test suite`s section. Create the
        dictionary for uploading results into this section.

        Args:
            test_suites_name (str): Name of the test suite.
            section (str): Name of the test suite section.

        Returns:
            (:obj:`List` of :obj:`Dict`): List of section`s results.
        """
        try:
            sect_id = add_section(
                self.connection, self.project_id,
                self.test_suites_ids[test_suites_name],
                f"{test_suites_name}_{self.test_suites_type}_{section.name}")
        except testrail.APIError as e:
            print(e)
            time.sleep(1)
            sect_id = add_section(
                self.connection, self.project_id,
                self.test_suites_ids[test_suites_name],
                f"{test_suites_name}_{self.test_suites_type}_{section.name}")

        # Get section testcases with info
        section_tcases_data = []
        section_tcases_data = get_tc_tr_data(
            self.connection, self.project_id,
            self.test_suites_ids[test_suites_name], sect_id)
        tsuite_section_results = []
        for test_case in section.test_cases:
            test_case_name = f"{self.project_name}_" + \
                f"{test_suites_name}_{section.name}_{test_case.name}"
            tcase_data = {}
            for testcase in section_tcases_data:
                if testcase["title"] == test_case_name:
                    tcase_data = testcase
                    break
            if tcase_data:
                description = tcase_data["custom_description"]
                preconditions = tcase_data["custom_preconditions"]
                steps = tcase_data["custom_steps"]
                expected = tcase_data["custom_expected"]
            else:
                description = "No descrpition provided"
                preconditions = "No preconditions provided"
                steps = "No TC steps provided"
                expected = "No expected output provided"
            try:
                tc_id = add_tc(
                    self.connection,
                    self.test_suites_ids[test_suites_name],
                    sect_id,
                    test_case_name, self.tcase_template_id, self.tsuites_type_id,
                    description,
                    preconditions,
                    steps,
                    expected,
                    tcase_data)
            except testrail.APIError as e:
                print(e)
                time.sleep(5)
                print(test_case.name)
                tc_id = add_tc(
                    self.connection,
                    self.test_suites_ids[test_suites_name],
                    sect_id,
                    test_case_name, self.tcase_template_id, self.tsuites_type_id,
                    "No descrpition provided",
                    "No preconditions provided",
                    "No TC steps provided",
                    "No expected output provided",
                    tcase_data)
            # "status_id": 2, - Blocked
            # "status_id": 5, - Fail
            # "status_id": 1, - Pass
            #              4, - Retest
            #              6, - Skipped
            tc_status = 1 if test_case.inner_info() is None else 5
            tsuite_section_results.append(
                {"case_id": tc_id,
                 "status_id": tc_status,
                 "comment": "\n".join(test_case.inner_info()) if tc_status != 1
                 else "",
                 "defects": "",
                 "assignedto_id": self.assignee
                 })
        return tsuite_section_results


class DefaultHelpParser(argparse.ArgumentParser):
    """
    Child of ArgumentParser that prints help on error.
    """

    def error(self, message):
        sys.stderr.write('error: %s\n' % message)
        self.print_help()
        sys.exit(2)


if __name__ == "__main__":
    # lava = LavaParser(path_to_file="lava.log")
    arg_parser = DefaultHelpParser(
        "Uploads results of test run (in form of XML or 'Lava' .log)"
        f" to TestRail.\nDefault project is {PROJECT_NAME}.")
    arg_parser.add_argument(
        "path_to_log", help="Path to the results folder of the test"
        "run. Lava job id if the source is remote logs from Lava server.")
    arg_parser.add_argument("format", help="Foramt of input file "
                            "with the results.",
                            choices=["lava_remote", "idt_xml", "lava_local"])
    arg_parser.add_argument("version", help="Version of product."
                            "(For naming purposes)")
    arg_parser.add_argument(
        "build_id", help="Id of the CI build, used as test plan`s name")
    arg_parser.add_argument("-p", "--project-name",
                            help="TestRail project name. Can "
                            "be modified directly in testrail_uploader.py "
                            "(PROJECT_NAME).")
    arg_parser.add_argument(
        "-u", "--username",
        help="TestRail username <name.surname@cypress.com>.")
    arg_parser.add_argument(
        "-k", "--api_key", help="TestRail API key.Creation is available"
        "under -> My setting -> API Keys -> Add Key and don’t forget  to click"
        "`save setting`")
    arg_parser.add_argument(
        "-c", "--configurations", help="Test run configurations JSON. Example: "
        "{\"TR_CONF_GROUP_NAME\": \"ubuntu18\",\"Compiler\":\"GCC\"}"
    )
    try:
        arguments = arg_parser.parse_args()
    except Exception as ex:
        print(ex)
        exit(1)

    report = TestReport()
    if arguments.format == "lava_local":
        parser = LavaParser(path_to_file=arguments.path_to_log)

    elif arguments.format == "lava_remote":
        content_of_lava_log = get_lava_log_by_job_id(arguments.path_to_log)
        parser = LavaParser(content=content_of_lava_log)

    elif arguments.format == "idt_xml":
        if arguments.configurations:
            configurations = json.loads(arguments.configurations)
        else:
            raise ValueError(
                "--configurations argument is required for IDT tests. E.g. "
                "--configurations \'{\"TR_CONF_GROUP_NAME\": \"ubuntu18\","
                "\"Compiler\":\"GCC\"}\'")
        path = Path(arguments.path_to_log)
        directories = os.listdir(path)
        print(len(directories))
        if len(directories) != 1:
            raise RuntimeError("Results folder supposed to contain IDT tests"
                               " result only for this run!")
        path = path.joinpath(directories[0])
        parser = XMLParser(path.joinpath("FRQ_Report.xml"))
    report.add_test_suites(parser.create_test_suite_report())

    if arguments.format != "idt_xml":
        configurations = parser.config

    if arguments.project_name:
        project_name = arguments.project_name
    else:
        project_name = PROJECT_NAME
    TestRailUploader([arguments.username, arguments.api_key, DOMAIN],
                     report,
                     arguments.build_id,
                     arguments.version,
                     project_name).upload_results(configurations)
