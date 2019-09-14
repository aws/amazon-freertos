# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

""" DUT for IDF applications """
import os
import re
import subprocess
import functools
import random
import tempfile
import subprocess

# python2 and python3 queue package name is different
try:
    import Queue as _queue
except ImportError:
    import queue as _queue

from serial.tools import list_ports

import DUT
import Utility


class IDFToolError(OSError):
    pass


class IDFDUTException(RuntimeError):
    pass


class IDFRecvThread(DUT.RecvThread):

    PERFORMANCE_PATTERN = re.compile(r"\[Performance]\[(\w+)]: ([^\r\n]+)\r?\n")
    EXCEPTION_PATTERNS = [
        re.compile(r"(Guru Meditation Error: Core\s+\d panic'ed \([\w].*?\))"),
        re.compile(r"(abort\(\) was called at PC 0x[a-fA-F\d]{8} on core \d)"),
        re.compile(r"(rst 0x\d+ \(TG\dWDT_SYS_RESET|TGWDT_CPU_RESET\))")
    ]
    BACKTRACE_PATTERN = re.compile(r"Backtrace:((\s(0x[0-9a-f]{8}):0x[0-9a-f]{8})+)")
    BACKTRACE_ADDRESS_PATTERN = re.compile(r"(0x[0-9a-f]{8}):0x[0-9a-f]{8}")

    def __init__(self, read, dut):
        super(IDFRecvThread, self).__init__(read, dut)
        self.exceptions = _queue.Queue()

    def collect_performance(self, comp_data):
        matches = self.PERFORMANCE_PATTERN.findall(comp_data)
        for match in matches:
            Utility.console_log("[Performance][{}]: {}".format(match[0], match[1]),
                                color="orange")

    def detect_exception(self, comp_data):
        for pattern in self.EXCEPTION_PATTERNS:
            start = 0
            while True:
                match = pattern.search(comp_data, pos=start)
                if match:
                    start = match.end()
                    self.exceptions.put(match.group(0))
                    Utility.console_log("[Exception]: {}".format(match.group(0)), color="red")
                else:
                    break

    def detect_backtrace(self, comp_data):
        start = 0
        while True:
            match = self.BACKTRACE_PATTERN.search(comp_data, pos=start)
            if match:
                start = match.end()
                Utility.console_log("[Backtrace]:{}".format(match.group(1)), color="red")
                # translate backtrace
                addresses = self.BACKTRACE_ADDRESS_PATTERN.findall(match.group(1))
                translated_backtrace = ""
                for addr in addresses:
                    ret = self.dut.lookup_pc_address(addr)
                    if ret:
                        translated_backtrace += ret + "\n"
                if translated_backtrace:
                    Utility.console_log("Translated backtrace\n:" + translated_backtrace, color="yellow")
                else:
                    Utility.console_log("Failed to translate backtrace", color="yellow")
            else:
                break

    CHECK_FUNCTIONS = [collect_performance, detect_exception, detect_backtrace]


def _tool_method(func):
    """ close port, execute tool method and then reopen port """
    @functools.wraps(func)
    def handler(self, *args, **kwargs):
        self.close()
        ret = func(self, *args, **kwargs)
        self.open()
        return ret
    return handler


class IDFDUT(DUT.SerialDUT):
    """ IDF DUT, extends serial with ESPTool methods """

    CHIP_TYPE_PATTERN = re.compile(r"Detecting chip type[.:\s]+(.+)")
    # if need to erase NVS partition in start app
    ERASE_NVS = True
    RECV_THREAD_CLS = IDFRecvThread
    TOOLCHAIN_PREFIX = "xtensa-esp32-elf-"

    def __init__(self, name, port, log_file, app, allow_dut_exception=False, **kwargs):
        self.download_config, self.partition_table = app.process_app_info()
        super(IDFDUT, self).__init__(name, port, log_file, app, **kwargs)
        self.allow_dut_exception = allow_dut_exception

    @classmethod
    def get_chip(cls, app, port):
        """
        get chip id via esptool

        :param app: application instance (to get tool)
        :param port: comport
        :return: chip ID or None
        """
        try:
            output = subprocess.check_output(["python", app.esptool, "--port", port, "chip_id"])
        except subprocess.CalledProcessError:
            output = bytes()
        if isinstance(output, bytes):
            output = output.decode()
        chip_type = cls.CHIP_TYPE_PATTERN.search(output)
        return chip_type.group(1) if chip_type else None

    @classmethod
    def confirm_dut(cls, port, app, **kwargs):
        return cls.get_chip(app, port) is not None

    @_tool_method
    def start_app(self, erase_nvs=ERASE_NVS):
        """
        download and start app.

        :param: erase_nvs: whether erase NVS partition during flash
        :return: None
        """
        if erase_nvs:
            address = self.partition_table["nvs"]["offset"]
            size = self.partition_table["nvs"]["size"]
            nvs_file = tempfile.NamedTemporaryFile()
            nvs_file.write(chr(0xFF) * size)
            nvs_file.flush()
            download_config = self.download_config + [address, nvs_file.name]
        else:
            download_config = self.download_config

        retry_baud_rates = ["921600", "115200"]
        error = IDFToolError()
        try:
            for baud_rate in retry_baud_rates:
                try:
                    subprocess.check_output(["python", self.app.esptool,
                                             "--port", self.port, "--baud", baud_rate]
                                            + download_config)
                    break
                except subprocess.CalledProcessError as error:
                    continue
            else:
                raise error
        finally:
            if erase_nvs:
                nvs_file.close()

    @_tool_method
    def reset(self):
        """
        reset DUT with esptool

        :return: None
        """
        subprocess.check_output(["python", self.app.esptool, "--port", self.port, "run"])

    @_tool_method
    def erase_partition(self, partition):
        """
        :param partition: partition name to erase
        :return: None
        """
        address = self.partition_table[partition]["offset"]
        size = self.partition_table[partition]["size"]
        with open(".erase_partition.tmp", "wb") as f:
            f.write(chr(0xFF) * size)

    @_tool_method
    def dump_flush(self, output_file, **kwargs):
        """
        dump flush

        :param output_file: output file name, if relative path, will use sdk path as base path.
        :keyword partition: partition name, dump the partition.
                            ``partition`` is preferred than using ``address`` and ``size``.
        :keyword address: dump from address (need to be used with size)
        :keyword size: dump size (need to be used with address)
        :return: None
        """
        if os.path.isabs(output_file) is False:
            output_file = os.path.relpath(output_file, self.app.get_log_folder())
        if "partition" in kwargs:
            partition = self.partition_table[kwargs["partition"]]
            _address = partition["offset"]
            _size = partition["size"]
        elif "address" in kwargs and "size" in kwargs:
            _address = kwargs["address"]
            _size = kwargs["size"]
        else:
            raise IDFToolError("You must specify 'partition' or ('address' and 'size') to dump flash")
        subprocess.check_output(
            ["python", self.app.esptool, "--port", self.port, "--baud", "921600",
             "--before", "default_reset", "--after", "hard_reset", "read_flash",
             _address, _size, output_file]
        )

    def lookup_pc_address(self, pc_addr):
        cmd = ["%saddr2line" % self.TOOLCHAIN_PREFIX,
               "-pfiaC", "-e", self.app.elf_file, pc_addr]
        ret = ""
        try:
            translation = subprocess.check_output(cmd)
            ret = translation.decode()
        except OSError:
            pass
        return ret

    def get_exceptions(self):
        """ Get exceptions detected by DUT receive thread. """
        exceptions = []
        if self.receive_thread:
            while True:
                try:
                    exceptions.append(self.receive_thread.exceptions.get(timeout=0))
                except _queue.Empty:
                    break
        return exceptions

    def close(self):
        super(IDFDUT, self).close()
        if not self.allow_dut_exception and self.get_exceptions():
            Utility.console_log("DUT exception detected on {}".format(self), color="red")
            raise IDFDUTException()
