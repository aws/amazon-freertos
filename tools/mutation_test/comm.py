import asyncio
import time
import serial

import utils

SERIAL_TIMEOUT = 3
SUBPROC_TIMEOUT = 1


# ----------------------------------------------------------------------------------------
def event_loop():  # pragma: no cover
    """
    Return the asyncio event loop.
    """
    return _EVENT_LOOP


def wait_for(future, timeout=None):
    """
    Run until the future or coroutine is complete with a timeout.
    """
    if timeout:
        future = asyncio.wait_for(future, timeout)
    return _EVENT_LOOP.run_until_complete(future)


def subproc_run(command, timeout=300):
    """
    Execute a given subprocess command with a timeout. Print its output at real time.
    """
    from asyncio.subprocess import PIPE

    proc = wait_for(asyncio.create_subprocess_shell(command, stdout=PIPE, stderr=PIPE))
    _read_until_complete(proc, None, None, timeout)
    return wait_for(proc.wait())


def read_target_output(target, start_flag, stop_flags, pass_flag, start_timeout, exec_timeout):
    """
    Read output from a serial port or a subprocess. Output is printed at real time.
    """
    is_serial = isinstance(target, serial.Serial)
    wait_for_output = _wait_for_serial_output if is_serial else _wait_for_subproc_output
    final_output = ''
    alive = False
    if start_flag is None:
        final_output = wait_for_output(target, start_timeout)
        if final_output:
            output, alive = _read_until_complete(target, stop_flags, pass_flag, exec_timeout)
            final_output += output
    else:
        output, alive = _read_until_complete(target, start_flag, None, start_timeout)
        if output.endswith(start_flag):
            curr_output, alive = _read_until_complete(target, stop_flags, pass_flag, exec_timeout)
            final_output = output + curr_output

    return final_output, alive


# ----------------------------------------------------------------------------------------
# Private functions
def _wait_for_subproc_output(subproc, timeout):
    try:
        c = wait_for(subproc.stdout.read(1), timeout).decode()
    except asyncio.TimeoutError:
        c = ''
    utils.raw_print(c)
    return c


def _wait_for_serial_output(serial_port, timeout):
    serial_port.timeout = timeout
    try:
        c = _serial_read(serial_port).decode()
    except (UnicodeDecodeError, serial.SerialException):
        c = ' '
    utils.raw_print(c)
    serial_port.timeout = SERIAL_TIMEOUT
    return c


def _read_until_complete(target, stop_flags, pass_flag, timeout):
    is_serial = isinstance(target, serial.Serial)
    target_read = _serial_read if is_serial else _subproc_read
    target_terminated = (lambda: False) if is_serial else target.stdout.at_eof

    final_output = ''
    alive = False
    timeout_end = time.time() + timeout
    # Read until timeout or stop_flag is detected.
    while time.time() < timeout_end and not target_terminated():
        try:
            c = target_read(target).decode()
        except (UnicodeDecodeError, asyncio.TimeoutError, serial.SerialException):
            time.sleep(1)
        else:
            if not c:
                continue
            utils.raw_print(c)
            final_output += c
            if stop_flags is not None and any(flag in final_output for flag in stop_flags):
                utils.raw_print('\n')
                break
            if pass_flag is not None and pass_flag in final_output:
                alive = True

    return final_output, alive


def _subproc_read(subproc):
    return wait_for(subproc.stdout.read(1), SUBPROC_TIMEOUT) \
        or (wait_for(subproc.stderr.read(1), SUBPROC_TIMEOUT) if subproc.stderr else '')


def _serial_read(serial_port):
    return serial_port.read_until()


# ----------------------------------------------------------------------------------------
# Set up async event loop.
if utils.get_host_os() == 'win':
    _EVENT_LOOP = asyncio.ProactorEventLoop()  # pragma: no cover
    asyncio.set_event_loop(_EVENT_LOOP)        # pragma: no cover
else:
    _EVENT_LOOP = asyncio.get_event_loop()     # pragma: no cover
