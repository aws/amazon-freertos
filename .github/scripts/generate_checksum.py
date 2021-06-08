# -*- coding: utf-8 -*-
import argparse
import hashlib
import json
import os

# FreeRTOS now supports two types of qualifications:
# 1. Minimal Qualification - Validation that the vendor did not modify any
#                            of 1st party tests.
# 2. Complete Qualification - Validation that vendor did not modify anything
#                             other than their port layer.
#
# To support the above two type of qualification source files are divided in two
# sets:
# 1. Minimal
# 2. Exhaustive
#
# Minimal Qualification only verifies that Minimal files are intact while
# Complete Qualification verifies that both Minimal and Exhaustive files are
# intact.

# Checksums are calculated for the following files:
#
# Required:
#   demos                       - 1st party code.
#   freerots_kernel             - 1st party code.
#   libraries/3rdparty          - Not 1st party code but vendor must not modify.
#   libraries/abstractions      - 1st party code.
#   libraries/c_sdk             - 1st party code.
#   libraries/freertos_plus     - 1st party code.
#   tests                       - 1st party code.
#
# Optional:
#   freerots_kernel/portable    - 1st party code.

ALL_REQUIRED_COMPONENTS = ["demos", "freertos_kernel", "libraries", "tests"]

ALL_OPTIONAL_COMPONENTS = [os.path.join("freertos_kernel", "portable")]

MINIMAL_REQUIRED_COMPONENTS = [
    "demos",
    "tests",
    os.path.join("libraries", "abstractions", "ble_hal", "test"),
    os.path.join("libraries", "abstractions", "pkcs11", "test"),
    os.path.join("libraries", "abstractions", "secure_sockets", "test"),
    os.path.join("libraries", "abstractions", "wifi", "test"),
    os.path.join("libraries", "c_sdk", "standard", "ble", "test"),
    os.path.join("libraries", "freertos_plus", "standard", "tls", "test"),
]

MINIMAL_OPTIONAL_COMPONENTS = []

# Current checksum schema version.
CURRENT_SCHEMA = 3

WINDOWS_LINE_ENDING = b"\r\n"
UNIX_LINE_ENDING = b"\n"


def generate_checksum_for_files(afr_path, list_of_files):
    """
    Generate SHA1 checksums for the given files.

    Parameters
    ----------
    afr_path
        The location of the AFR code.
    list_of_files
        List of file paths for which to calculate SHA1. The paths are relative
        to AFR root.

    Returns
    -------
    dict
        A dictionary with keys as file paths and values as SHA1 checksums:
        {
            "file_1": "checksum_1",
            "file_2": "checksum_2"
        }
    """
    checksums = {}
    for file in list_of_files:
        print("Generating checksum for {}...".format(file))
        file_abs_path = os.path.join(afr_path, file)
        if not os.path.exists(file_abs_path):
            continue

        sha256 = hashlib.sha256()
        with open(file_abs_path, "rb") as f:
            content = f.read()
            # This ensures that the calculated checksum is same whether the file
            # is on a Windows machine or on a Unix machine.
            content = content.replace(WINDOWS_LINE_ENDING, UNIX_LINE_ENDING)
            sha256.update(content)

        # JSON file should have forward slashes in the pathname regardless of
        # which OS the script is run on.
        file = file.replace("\\", "/")
        checksums[file] = sha256.hexdigest()
    return checksums


def get_list_of_files(afr_path, search_path, include_portable):
    """
    Returns a list of files for which we want to calculate SHA1 checksums.

    It excludes hidden files.

    Parameters
    ----------
    afr_path
        The location of the AFR code.
    search_path
        Path relative to the AFR root which needs to be searched for files.
    include_portable
        Whether or not to include the subfolders named 'portable'.

    Returns
    -------
    list
        List of file paths relative to AFR root.
    """
    list_of_files = []
    search_path = os.path.join(afr_path, search_path)

    for root, dirs, files in os.walk(search_path, topdown=True):
        # Do not search portable folder if the caller says so
        if include_portable == False:
            dirs[:] = [d for d in dirs if d not in ["portable"]]

        # Do not include hidden files and folders.
        dirs[:] = [d for d in dirs if not d[0] == "."]
        files = [f for f in files if not f[0] == "."]

        for f in files:
            list_of_files.append(os.path.join(os.path.relpath(root, afr_path), f))

    return list_of_files


def get_required_files(afr_path, search_path):
    """
    Returns a list of required files at search_path for which we want to
    calculate SHA1 checksums.

    It excludes hidden files.

    Parameters
    ----------
    afr_path
        The location of the AFR code.
    search_path
        Path relative to the AFR root which needs to be searched for files.

    Returns
    -------
    list
        List of file paths relative to AFR root.
    """
    return get_list_of_files(afr_path, search_path, False)


def get_optional_files(afr_path, search_path):
    """
    Returns a list of optional files at search_path for which we want to
    calculate SHA1 checksums.

    It excludes hidden files.

    Parameters
    ----------
    afr_path
        The location of the AFR code.
    search_path
        Path relative to the AFR root which needs to be searched for files.

    Returns
    -------
    list
        List of file paths relative to AFR root.
    """
    return get_list_of_files(afr_path, search_path, True)


def generate_checksums_file_schema_v1(required_files, afr_path, output_file_path):
    """
    Generates the checksum file for version 1 schema.

    Parameters
    ----------
    required_files
        List of files which must be present and un-modifed.
    afr_path
        The location of the AFR code.
    output_file_path
        Path where to generate the checksum file.
    """
    # Calculate checksums for all the required files.
    checksums = generate_checksum_for_files(afr_path, required_files)

    # Write calculated checksums to JSON.
    with open(output_file_path, "w") as out:
        json.dump(checksums, out, sort_keys=True, indent=4)


def generate_checksums_file_schema_v2(required_files, optional_components, afr_path, output_file_path):
    """
    Generates the checksum file for version 2 schema.
    https://w.amazon.com/bin/view/AWS/IoTDevicesEcosystem/design/checksum_schema_enhancements

    Parameters
    ----------
    required_files
        List of files which must be present and un-modifed.
    optional_components
        Components which can be missing but if present, must not be modified.
        It is a dict of the following format:
        {
            "component1_location" : [file1, file2],
            "component2_location" : [file3, file4]
        }
    afr_path
        The location of the AFR code.
    output_file_path
        Path where to generate the checksum file.
    """
    checksums = {}

    # Calculate checksums for all the required files.
    checksums_required_files = generate_checksum_for_files(afr_path, required_files)
    checksums["required_files"] = checksums_required_files

    # Calculate checksums for all the optional components.
    checksums["optional_components"] = []
    for optional_component_location in optional_components:
        optional_component_checksum = {}
        optional_component_checksum["location"] = optional_component_location
        optional_component_checksum["checksums"] = generate_checksum_for_files(
            afr_path, optional_components[optional_component_location]
        )
        checksums["optional_components"].append(optional_component_checksum)

    # Write calculated checksums to JSON.
    with open(output_file_path, "w") as out:
        json.dump(checksums, out, sort_keys=True, indent=4)


def generate_checksums_file_schema_v3(
    exhaustive_required_files,
    exhaustive_optional_components,
    minimal_required_files,
    minimal_optional_components,
    afr_path,
    output_file_path,
):
    """
    Generates the checksum file for version 3 schema.

    Parameters
    ----------
    exhaustive_required_files
        List of files which must be present and un-modifed for exhaustive check.
    exhaustive_optional_components
        Components which can be missing but if present, must not be modified for
        exhaustive check.
        It is a dict of the following format:
        {
            "component1_location" : [file1, file2],
            "component2_location" : [file3, file4]
        }
    minimal_required_files
        List of files which must be present and un-modifed for minimal check.
    minimal_optional_components
        Components which can be missing but if present, must not be modified for
        minimal check.
        It is a dict of the following format:
        {
            "component1_location" : [file1, file2],
            "component2_location" : [file3, file4]
        }
    afr_path
        The location of the AFR code.
    output_file_path
        Path where to generate the checksum file.
    """
    checksums = {}
    checksums["exhaustive"] = {}
    checksums["minimal"] = {}

    # Calculate checksums for exhaustive required files.
    checksums_exhaustive_required_files = generate_checksum_for_files(afr_path, exhaustive_required_files)
    checksums["exhaustive"]["required_files"] = checksums_exhaustive_required_files

    # Calculate checksums for exhaustive optional components.
    checksums["exhaustive"]["optional_components"] = []
    for optional_component_location in exhaustive_optional_components:
        optional_component_checksum = {}
        optional_component_checksum["location"] = optional_component_location
        optional_component_checksum["checksums"] = generate_checksum_for_files(
            afr_path, exhaustive_optional_components[optional_component_location]
        )
        checksums["exhaustive"]["optional_components"].append(optional_component_checksum)

    # Calculate checksums for minimal required files.
    checksums_minimal_required_files = generate_checksum_for_files(afr_path, minimal_required_files)
    checksums["minimal"]["required_files"] = checksums_minimal_required_files

    # Calculate checksums for minimal optional components.
    checksums["minimal"]["optional_components"] = []
    for optional_component_location in minimal_optional_components:
        optional_component_checksum = {}
        optional_component_checksum["location"] = optional_component_location
        optional_component_checksum["checksums"] = generate_checksum_for_files(
            afr_path, minimal_optional_components[optional_component_location]
        )
        checksums["minimal"]["optional_components"].append(optional_component_checksum)

    # Write calculated checksums to JSON.
    with open(output_file_path, "w") as out:
        json.dump(checksums, out, sort_keys=True, indent=4)


def parse_arguments():
    """
    Parses the command line arguments.
    """
    parser = argparse.ArgumentParser(description="FreeRTOS Checksum Generator")

    parser.add_argument("--root", required=True, help="Location of the root directory of amazon-freertos.")
    parser.add_argument("--out", default="checksums.json", help="Name of the output JSON file.")
    parser.add_argument("--schema", default=CURRENT_SCHEMA, type=int, help="Schema of checksums.json to generate.")

    args = parser.parse_args()
    return vars(args)


def main():
    """
    Main entry point.
    """
    args = parse_arguments()

    afr_path = args["root"]
    schema_version = args["schema"]
    output_file_name = args["out"]
    output_file_path = os.path.join(afr_path, output_file_name)

    print("AFR Code: {}".format(afr_path))
    print("Output File: {}".format(output_file_path))
    print("Schema Version: {}".format(schema_version))

    # Get all the required files for which we need to generate checksum.
    all_required_files = []
    for required_component in ALL_REQUIRED_COMPONENTS:
        all_required_files.extend(get_required_files(afr_path, required_component))

    # Get the required file for minimal set.
    minimal_required_files = []
    for required_component in MINIMAL_REQUIRED_COMPONENTS:
        minimal_required_files.extend(get_required_files(afr_path, required_component))

    # Exhaustive = All - Required.
    exhaustive_required_files = [
        required_file for required_file in all_required_files if required_file not in minimal_required_files
    ]

    # Get minimal optional components for which we need to generate checksum.
    # Dict of form:
    #   {
    #       "component1_location" : [file1, file2],
    #       "component2_location" : [file3, file4]
    #   }
    minimal_optional_components = {}
    minimal_optional_files = []
    for optional_component_location in MINIMAL_OPTIONAL_COMPONENTS:
        files_in_optional_component = get_optional_files(afr_path, optional_component_location)
        minimal_optional_files.extend(files_in_optional_component)
        optional_component_location = optional_component_location.replace("\\", "/")
        minimal_optional_components[optional_component_location] = files_in_optional_component

    # Get all the optional components for which we need to generate checksum.
    # Dict of form:
    #   {
    #       "component1_location" : [file1, file2],
    #       "component2_location" : [file3, file4]
    #   }
    all_optional_components = {}
    exhaustive_optional_components = {}
    for optional_component_location in ALL_OPTIONAL_COMPONENTS:
        files_in_optional_component = get_optional_files(afr_path, optional_component_location)
        # If a file is included in minimal_optional_components it won't be in
        # exhaustive_optional_components.
        files_in_exhaustive_optional_component = [
            optional_component_file
            for optional_component_file in files_in_optional_component
            if optional_component_file not in minimal_optional_files
        ]

        optional_component_location = optional_component_location.replace("\\", "/")
        all_optional_components[optional_component_location] = files_in_optional_component
        if len(files_in_exhaustive_optional_component) > 0:
            exhaustive_optional_components[optional_component_location] = files_in_exhaustive_optional_component

    if schema_version == 1:
        # Schema version 1 only contains all required files.
        generate_checksums_file_schema_v1(all_required_files, afr_path, output_file_path)
    elif schema_version == 2:
        # Schema version 2 contains all required files and all optional files.
        generate_checksums_file_schema_v2(all_required_files, all_optional_components, afr_path, output_file_path)
    elif schema_version == 3:
        # Schema version 3 contains exhaustive and minimal files separately.
        generate_checksums_file_schema_v3(
            exhaustive_required_files,
            exhaustive_optional_components,
            minimal_required_files,
            minimal_optional_components,
            afr_path,
            output_file_path,
        )
    else:
        raise ValueError("Incorrect schema version number!!")

    print("Generated {}.".format(output_file_path))


if __name__ == "__main__":
    main()
