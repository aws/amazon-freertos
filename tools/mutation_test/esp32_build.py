#!/Library/Frameworks/Python.framework/Versions/3.8/bin/python3

import utils
import subprocess
import os
import shlex

dir_path = os.path.dirname(os.path.realpath(__file__))
os.chdir(dir_path)
os.chdir('../..')
root_path = os.getcwd()

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
    subprocess.check_call(["make", "-j", "14", "all"])
    os.chdir(old_path)

def flash(port):
    os.chdir(root_path)
    # ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -B build
    cmd = "./vendors/espressif/esp-idf/tools/idf.py erase_flash flash -B build -p {}".format(port)
    utils.yellow_print(cmd)
    # Flash to device
    utils.yellow_print("Flashing to device...")
    subprocess.check_output(shlex.split(cmd))
    utils.yellow_print("Done Flashing")

def main():
    os.chdir(root_path)
    port = os.environ['PORT'] if os.environ['PORT'] else utils.get_default_serial_port()
    vendor = "espressif"
    board =  "esp32_wrover_kit"
    compiler =  "xtensa-esp32"
    cmake(vendor, board, compiler)
    build()
    flash(port)

if __name__ == "__main__":
    main()