"""
Implements lava logs parser. Output format: (:obj:'List' of :obj:'TestSuite').
It shares API with XMLParser except LavaParser is resposible for test run
configurations.
Hermann Yavorskyi <hermann.yavorskyi@cypress.com>
"""
import json
from functools import reduce
from script_utils.test_report_utils import TestSuite, TestSuiteSection, TestCase


class LavaParser:
    """
    Class implements paraser for logs stored on Lava.
    This particular parser also resposible for parsing configurations from logs.
    It is possible to use it both with local files, or with `str` content read
    with lava_log_scrapper.

    Args:
        path_to_file (str): Mutually exclusive with content. Path to local file
            with logs stored.
        content (str): Mutually exclusive with path_to_file. Content of the log
            file stored on the Lava server and obtained with lava_log_scrapper.

    Attributes:
        logs (:obj:`List` of :obj:`Dict` of str:str): List of preprocessed
            logs. (Each dict - line in file/content)
        config (:obj:`Dict` of str:str): Configurations of the test run.
    """

    def __init__(self, path_to_file=None, content=None):
        if all((path_to_file, content)) or not any((path_to_file, content)):
            raise ValueError("Exactly one argument *path_to_file* or *content*"
                             " has to be specified!")
        if path_to_file:
            with open(path_to_file, encoding="utf-8") as file:
                self.logs = file.read()
        else:
            self.logs = content
        self.test_suite_name, self.config = self._parse_ts_name_and_config()

    def _parse_ts_name_and_config(self):
        """
        Parse test suite name from the path in fifth line of log file.

        Returns:
            (str): Test suite name.
            (:obj:`Dict` of str:str): configurations
        """
        name_and_build_type, compiler = self.logs[4]["msg"].split("/")[-1:-3:-1]
        config = {"Compiler": compiler}
        name_and_build_type = name_and_build_type.split("-")[1:]
        if len(name_and_build_type) == 3:
            name = "_".join(name_and_build_type[0:2])
        else:
            name = name_and_build_type[0]
        config["BuildType"] = name_and_build_type[-1].split(".")[0]
        return name, config

    @property
    def logs(self):
        """
        logs properpy getter.

        Returns:
            logs (:obj:`List` of :obj:`Dict` of str:str): List of preprocessed
                logs. (Each dict - line in file/content)
        """
        return self._logs

    @staticmethod
    def _remove_script(line):
        """
        Lava logs have interactive scripts in it, and theese are not handled
        by json library. This method takes care about them.

        Args:
            line (str): Line of the to be preprocessed.

        Returns:
            line (str): Line without the scripy key-value pair if
                it was present. Original line otherwise.
        """
        def find_second_occurence(string_, character):
            """
            Internal function to find second occurence of character.

            Args:
                string_ (str): String in which search is performed.
                character (str): Seeked symbol.

            Returns:
                (int): Position of the second occurence of character in the
                    string_, -1 if not present.
            """
            # We need 2nd occurence, so we can perform only 2 splittings
            parts = string_.split(character, 2)

            # 2 occurences divide string into 3 parts
            if len(parts) < 3:
                return -1
            return len(string_)-len(parts[-1])-len(character)

        index_of_script = line.find("!!python/")
        if index_of_script != -1:
            line = line[:line[:index_of_script].rindex(",")+1] + line[
                index_of_script+line[
                    index_of_script:].find("level")-1:]
            # +1, because we
            # want to keep comma; -1 to keep double quote
        index_of_script = line.find("!!binary")
        if index_of_script != -1:
            line = line[:line[:index_of_script].rindex(",")] + line[
                index_of_script +
                find_second_occurence(line[index_of_script:], '\"')+1:]
        return line

    @logs.setter
    def logs(self, value):
        """
        logs propery seter.

        Args:
            value (str): Content of the Lava log file.
        """
        logs = []
        value = value.strip().split("\n")
        for line in value:
            try:
                line = line[line.index("{"):].strip()
            except ValueError as error_msg:
                print(f"Warning: {error_msg}.\n Processed string: {line}")
            # Files from lava seem to have escape # sequences which json parser
            # can`t handle
            line = line.replace("\\0", "")

            # We need this since there are "extra" key in result messages,
            # which contain script, that breaks json parser. So we get rid of
            # this entry
            line = self._remove_script(line)
            try:
                logs.append(json.loads(line))
            except json.decoder.JSONDecodeError as error:
                print(line)
                print(error)
                exit(1)
        self._logs = logs

    @staticmethod
    def _fill_in_test_suite(logs_to_report, test_suite):
        test_suite_sections = {}
        i = 0
        while i < len(logs_to_report):
            error = None
            exception = None
            if logs_to_report[i]["lvl"] == "error":
                error = logs_to_report[i]["msg"]
                i += 1
            if logs_to_report[i]["lvl"] == "exception":
                exception = logs_to_report[i]["msg"]
                i += 1
            section_name = logs_to_report[i]["msg"]["definition"]
            if section_name not in test_suite_sections:
                test_suite_sections[section_name] = TestSuiteSection(
                    section_name,
                    logs_to_report[i]["msg"]["namespace"] if "namespace" in
                    logs_to_report[i]["msg"].keys() else "",
                    0, 0,
                    0, 0, 0, 0)
            test_case = TestCase("LavaTest", logs_to_report[i]["msg"]["case"])
            if exception is not None:
                if error is not None:
                    error += f"\n{exception}"
                    test_case.set_error(error)
                else:
                    test_case.set_error(exception)
                test_suite_sections[section_name].errors += 1
                test_suite_sections[section_name].failures += 1
            elif error is not None:
                if "error_msg" in logs_to_report[i]["msg"].keys():
                    error += f"\n{logs_to_report[i]['msg']['error_msg']}"
                if "error_type" in logs_to_report[i]["msg"].keys():
                    test_case.set_failure_and_type(
                        error, "(Type:"
                        f" {logs_to_report[i]['msg']['error_type']})")
                else:
                    test_case.set_error(error)
                test_suite_sections[section_name].errors += 1
                test_suite_sections[section_name].failures += 1
            test_suite_sections[section_name].add_test_case(test_case)
            i += 1
        for section in test_suite_sections.values():
            test_suite.add_test_suite(section)
            test_suite.errors += section.errors
            test_suite.failures += section.failures

    def create_test_suite_report(self):
        """
        Create and fill in test suite (TestSuite and inner hierarchy).

        Returns:
            (:obj:`List` of :obj:TestSuite): Lists of TestSuites (test suite
            on TestRail). Returns list to have the same interface as XMLParser.
        """
        logs_to_report = list(filter(lambda log: log["lvl"] in
                                     ("results", "error", "exception"),
                                     self.logs))
        run_results = list(
            filter(lambda x: x["lvl"] == "results", logs_to_report))
        total_time = reduce(lambda x, y: x+y, map(
            lambda x: float(x["msg"]["duration"]), filter(lambda x:
                                                          "duration" in x["msg"].keys(), run_results)))
        skipped = len(list(
            filter(lambda x: x["msg"]["result"] == "skipped", run_results)))
        disabled = len(list(
            filter(lambda x: x["msg"]["result"] == "disabled", run_results)))

        # Create test suite
        test_suite = TestSuite(self.test_suite_name, total_time, len(run_results),
                               0, skipped, 0, disabled)

        # Fill in test suite
        self._fill_in_test_suite(logs_to_report, test_suite)

        # Return List here, because xml run results have multiple suites inside
        # and interface should be the same for both parsers
        return [test_suite, ]
