"""
Implements XML logs parser. Output format: (:obj:'List' of :obj:'TestSuite').
It shares API with LavaParser except LavaParser is resposible for test run
configurations.
Hermann Yavorskyi <hermann.yavorskyi@cypress.com>
"""
from bs4 import BeautifulSoup
from script_utils.test_report_utils import TestSuite, TestSuiteSection, TestCase


class XMLParser:
    """
    Parase XML test results and to represent them as TestSuite (and inner
    hierarchy). Current version was developed to support IDT tests. Other
    tests may be supported as well if XML file with results of test run
    has next structure: <testsuites><testsuite><testcase><error or failure> If
    test case failed </error or failure></testcase>...</testsuite>...
    </testsuites>

    Args:
        path_to_file (str): Path to local XML file with logs (test results).

    Attributes:
        logs (:obj:'BeautifulSoup'): Container of all logs to further
            processing.
    """

    def __init__(self, path_to_file):
        with open(path_to_file, encoding="utf-8") as file:
            self.logs = file

    @property
    def logs(self):
        """
        logs property getter.

        Returns:
            logs (:obj:'BeautifulSoup'):Container of all logs to further
            processing.
        """
        return self._logs

    @logs.setter
    def logs(self, value):
        """value property setter. Creates BeautifulSoup object (xml) \
            from opened file handler.

        Args:
            value (:obj:File): File handler to file with results of \
                test run.
        """
        self._logs = BeautifulSoup(value, "lxml")

    def create_test_suite_report(self):
        """
        Create and fill in test suite (TestSuite and inner hierarchy).

        Returns:
            (:obj:`List` of :obj:TestSuite): Lists of TestSuites (test suite
                on TestRail). List, because each XML can contain more than one
                <testsuites> tag.
        """
        # Creating test suites group (TestSuites), parsing it`s inner
        # test suites and adding it to the list.
        # Called from the constructor, should not be used externally.
        ts_list = list()
        for test_suites_raw in self.logs.find_all("testsuites"):
            test_suites = TestSuite(test_suites_raw.get("name"),
                                    int(test_suites_raw.get("time")),
                                    int(test_suites_raw.get("tests")),
                                    int(test_suites_raw.get("failures")),
                                    int(test_suites_raw.get("skipped")),
                                    int(test_suites_raw.get("errors")),
                                    int(test_suites_raw.get("disabled")))
            for test_suite in self._test_suite_generator(test_suites_raw):
                test_suites.add_test_suite(test_suite)
            ts_list.append(test_suites)
        return ts_list

    @staticmethod
    def _test_suite_generator(test_suites):
        for test_suite_raw in test_suites.find_all("testsuite"):
            test_suite = TestSuiteSection(test_suite_raw.get("name"),
                                          test_suite_raw.get("package"),
                                          int(test_suite_raw.get("time")),
                                          int(test_suite_raw.get("tests")),
                                          int(test_suite_raw.get("failures")),
                                          int(test_suite_raw.get("skipped")),
                                          int(test_suite_raw.get("errors")),
                                          int(test_suite_raw.get("disabled")))
            for test_case in XMLParser._test_case_generator(test_suite_raw):
                test_suite.add_test_case(test_case)
            yield test_suite

    @staticmethod
    def _test_case_generator(test_suite):
        for test_case_raw in test_suite.find_all("testcase"):
            test_case = TestCase(test_case_raw.get(
                "classname"), test_case_raw.get("name"))
            inner_data = test_case_raw.find("failure")
            if inner_data is not None:
                test_case.set_failure_and_type(
                    inner_data.text, inner_data.get("type"))
            else:
                inner_data = test_case_raw.find("error")
                if inner_data is not None:
                    test_case.set_error(inner_data.text)
            yield test_case
