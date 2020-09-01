"""
Implements TestReport which is in-between representation of run results
(XML or Lava .log files) and their TestRail representation. It can contain
multiple test suites at the same time (both Lava and XML). TestRailUploader
supports multiple test suites beiing combined in 1 test report, but it assumes
them all being in a single run and allows only 1 configuration set for 1 test
report.
Hermann Yavorskyi <hermann.yavorskyi@cypress.com>
"""


class TestReport:
    """
    Test report container for TestSuite (supports multiple) obtained from
    [XML/Lava]Parseer (during the initial development. Also support other
    parsers as soon as they fill the TestSuite and it`s hierarchy in the same
    way as these 2 does). It is used to report results to TestRail with the
    TestRailUploader class or to represent results of the test run as a
    formatted string.

    Attributes:
        test_suites_list (:obj:`List` of :obj:`TestSuite`): Inner container of
            TestSuite objects.
    """

    def __init__(self):
        self.test_suites_list = []

    def add_test_suites(self, test_suites):
        """
        Add a List of the existing TestSuite objects to this TestReport.

        Args:
        tesst_suites (:obj:`List` of :obj:`TestSuite`): List of test suites
            which will be added to other testsuites in this report
        """
        self.test_suites_list.extend(test_suites)

    def __str__(self):
        report = "Test Report:\n\n"
        test_suits_str = [str(test_suits)
                          for test_suits in self.test_suites_list]
        report += "\n\n".join(test_suits_str)
        return report
