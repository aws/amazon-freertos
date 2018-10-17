#!/usr/bin/python

import os.path
import re
import subprocess
import sys
import tempfile

def main(): # pragma: no cover
    files = sys.argv[1:]
    if commit_is_ready(files):
        sys.exit(0)
    # Todo: compress file names and be able to decompress file names
    if files:
        print("You may run the following command to repeat the check:")
        print("Hint: You may need to be at the repository's root directory.")
        print("\n    " + " ".join(sys.argv) + "\n")
    print('Aborting Commit.')

    sys.exit(1)


def commit_is_ready(file_names=""):
    """Return False if not ready.  Return True if commit is ready"""
    if not file_names:
        file_names = get_modified_files()

    existing_files = filter(file_exists, file_names)
    source_file_names = filter(file_is_checkable, existing_files)

    source_file_names = ' '.join(source_file_names)
    source_file_names = source_file_names.strip()
    if "" == source_file_names:
        return True

    return not (
        check_hungarian_notation(source_file_names) or
        check_uncrustify(source_file_names) or
        check_whitespace()
    )


def get_modified_files():
    changed_files = subprocess.check_output(
        "git diff-index --name-only --cached HEAD", shell=True)
    if type(changed_files) is not str:
        changed_files = changed_files.decode('utf-8')
    file_names = changed_files.split('\n')
    return file_names


def file_is_checkable(file_name):
    return (
        is_source_file(file_name)
        and is_in_checked_directory(file_name)
        and not is_in_ignored_directory(file_name)
    )


def file_exists(file_name):
    return os.path.isfile(file_name)


def is_source_file(file_name):
    if re.findall(r'\.[ch]$', file_name):
        return True
    return False


def is_in_checked_directory(file_name):
    checked_dirs = [
        "demos/common/",
        "lib/",
        "tests/common/"
        "tools/",
    ]
    for checked_dir in checked_dirs:
        if checked_dir in file_name:
            return True
    return False


def is_in_ignored_directory(file_name):
    ignored_dirs = [
        "lib/FreeRTOS-Plus-TCP/",
        "lib/FreeRTOS/",
        "lib/third_party/",
        "lib/pkcs11/portable",
        "lib/wifi/portable",
        "lib/secure_sockets/portable",
    ]
    for ignored_dir in ignored_dirs:
        if ignored_dir in file_name:
            return True
    return False


def check_whitespace():
    """Return True if check failed.  Return False if check passed"""
    if subprocess.call("git diff-index --check --cached HEAD", shell=True):
        return True
    return False


def check_hungarian_notation(changed_files):
    """Return True if check failed.  Return False if check passed"""
    if subprocess.call(
        (
            "python tools/checks/style/hn_check/src/hn_check.py " +
            changed_files
        ), shell=True
    ):
        return True
    return False


def check_uncrustify(changed_files):
    """Return True if check failed.  Return False if check passed"""
    if subprocess.call(
        (
            "uncrustify --check -q -c .uncrustify.cfg " +
            changed_files
        ), shell=True
    ):
        patch = patch_uncrustify(changed_files)
        if patch: # pragma: no cover
            # Nothing but side effects in write_patch
            write_patch(patch)
        return True
    return False


def patch_uncrustify(changed_files):
    """Creates patch to fix formatting in a set of files"""
    files = changed_files.split(" ")
    patch = ''
    for file in files:
        format_call = (
            'uncrustify -q -c .uncrustify.cfg -f {}'.format(file)
            + '| git --no-pager diff --color=always --no-index -- "{}" - '.format(file)
            + '| tail -n+3'
        )
        diff_result = subprocess.check_output(format_call, shell=True)
        if type(diff_result) is not str:
            diff_result = diff_result.decode("utf-8")

        diff_result = re.sub(r'---.*', '--- "a/{}"'.format(file), diff_result)
        diff_result = re.sub(r'\+\+\+.*', '+++ "b/{}"'.format(file), diff_result)
        patch += diff_result
    return patch


def write_patch(patch): # pragma: no cover
    """Writes patch to temporary file and prints instructions to stdout"""
    ansi_escape = re.compile(r'\x1B\[[0-?]*[ -/]*[@-~]')
    colorless_patch = ansi_escape.sub('', patch)
    tmp_dir = tempfile.mkdtemp()
    patch_file_name = os.path.join(tmp_dir, 'uncrustify_patch')
    with open(patch_file_name, 'w') as patch_file:
        patch_file.write(colorless_patch)
    print("The staged files are not formatted correctly.")
    print(patch)
    print("you may apply the patch using the following command:")
    print("")
    print("        git apply {}".format(patch_file_name))
    print("")


if __name__ == "__main__":  # pragma: no cover
    main()
