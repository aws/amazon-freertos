"""
Implemenatation of classes TestSuite, TestSuiteSection, TestCase. Last two are
used as internal objects of TestSuite class in order to represent the results
of test runs. Created buy parsers from log files/run resultes
(LavaParser/XMLParser). TestSuite should be added to TestReport in order to be
uploaded to the TestRail.
Hermann Yavorskyi <hermann.yavorskyi@cypress.com>
"""
from textwrap import indent


class TestSuite:
    """
    Class represents test suite (XML tag <testsuites> and inner
    content,tags/Lava .log file) and provides suitable representation for test
    report.
    TestRail representation is Test Suite

    Args:
        name (str): Name of the testsuites (attribute name of tag).
        time (int): Time elapsed on this group of test suites.
        tests_num (int): Number of test cases
        failures (int): Number of failed test cases.
        skipped (int): Number of skipped test cases.
        errors (int): Number of test cases with wrong results.
        disabled (int): Number of disabled test cases.

    Attributes:
        *Defined in Args*
        test_suite_list (:obj:'List' of :obj:'TestSuiteSection'): List of test
        suite sections in this test suite.
    """

    def __init__(self, name,
                 time, tests_num, failures, skipped, errors, disabled):
        self.name = name
        self.time = time
        self.tests_num = tests_num
        self.failures = failures
        self.skipped = skipped
        self.errors = errors
        self.disabled = disabled
        self.test_suite_list = list()

    def add_test_suite(self, test_suite):
        """
        Adds test suite section to this test suite.

        Args:
            test_suite (:obj:'TestSuteSection'): Test suite section to be
            added.
        """
        self.test_suite_list.append(test_suite)

    def __str__(self):
        result = f"Test suites {self.name}:\n"
        test_suite_lst_str = "\n\n".join(
            [indent(str(test_suite), "\t")
             for test_suite in self.test_suite_list])

        result += "\n".join((f"Time elapsed: {self.time}",
                             f"Number of tests: {self.tests_num}",
                             f"Failures: {self.failures}",
                             f"Skipped: {self.skipped}",
                             f"Errors: {self.errors}",
                             f"Disabled: {self.disabled}"))

        result += "\n"+test_suite_lst_str
        return result


class TestSuiteSection:
    """
    Class represent test suite section (XML tag <testsuite>, and it`s inner
    tags/definition in terms of LAVA) and provides suitable representation for
    test report.
    TestRail representation is Test Suite`s Section.

    Args:
        name (str): Name of the testsuite (attribute name of tag).
        time (int): Time elapsed on this test suite.
        tests_num (int): Number of test cases.
        failures (int): Number of failed test cases.
        skipped (int): Number of skipped test cases.
        errors (int): Number of test cases with wrong results.
        disabled (int): Number of disabled test cases.

    Attributes:
        *Defined in Args*
        test_cases (:obj:'List' of :obj:'TestCase'): List of test
        cases in this test suite.
    """

    def __init__(self, name, package, time,
                 tests_num, failures, skipped, errors, disabled):
        self.name = name
        self.package = package
        self.time = time
        self.tests_num = tests_num
        self.failures = failures
        self.skipped = skipped
        self.errors = errors
        self.disabled = disabled
        self.test_cases = list()

    def add_test_case(self, test_case):
        """Add test cae to this test suite.

        Args:
            test_case (:obj:'TestCase'): Test case to be added.
        """
        self.test_cases.append(test_case)

    def __str__(self):
        test_cases_str = "\n".join([str(case) for case in self.test_cases])
        result = "\n".join((f"Time elapsed: {self.time}",
                            f"Number of tests: {self.tests_num}",
                            f"Failures: {self.failures}",
                            f"Skipped: {self.skipped}",
                            f"Errors: {self.errors}",
                            f"Disabled: {self.disabled}"))
        result += "\n" + test_cases_str
        return result


class TestCase:
    """Class represent test case (XML tag, and it`s inner tags) and provides
        suitable representation for test report.
        In the moment of implementation Inner tag can be error or failure tags
        (since only IDT tests supported).

        TestRail representation is Test Case.

        Errors and failures are treated the same on TestRail.
    Args:
        classname (str): Name of the test cases`s class (attribute .classname
        of the XML tag).
        name (str): Name of the test case (attribute name of the XML tag).

    Attributes:
        *Defined in Args*
        failute (str): Failure message (None if test passed).
        failure_type (str): Type of the failure (None if test passed).
        error (str): Error message (None if test passed).
    """

    def __init__(self, classname, name):
        self.classname = classname
        self.name = name
        self.failure, self.failure_type = (None, None)
        self.error = None

    def set_failure_and_type(self, failure, failure_type):
        """Failure message and type setter.

        Args:
            failure (str): Failure message.
            failure_type (str): Type of the failure. Empty string if no type.
        """
        self.failure = failure
        self.failure_type = failure_type

    def set_error(self, error):
        """Error setter.

        Args:
            error (str): Error message.
        """
        self.error = error

    def inner_info(self):
        """Get Error/Failure message and failure_type.

        Returns:
            (:obj:'Tuple' of str): Error/Failure, Error/Failure message,
            failure_type (only for failures). 

            None: If test case passed
        """
        if self.failure is not None:
            return "Failure:", self.failure, self.failure_type
        if self.error is not None:
            return "Error:", self.error
        return None

    def __str__(self):
        result = f"Class: {self.classname}\t" + f"Name: {self.name}\n"
        info = self.inner_info()
        if info is not None:
            result += " ".join(info)
        else:
            result += "Pass"
        return result
