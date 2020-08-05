#!/usr/bin/env python3

import argparse
import os.path
import ntpath
import re
import sys
from datetime import datetime
from pathlib import PurePath
from collections import namedtuple
from collections import defaultdict
from xml.dom import minidom
from xml.etree import ElementTree

GRP_UNFILTERED = "unf:/tered*"

REMOVE_QUOTE_REGEX = re.compile(r"^\"|\"$")

# cmsisdata.temp file format
# ------------------------------------#
# AppName
# Device
# Core
# Linker script
# Defines (Comma separated list)
# Include Paths (Comma separated list)
# .c Source Files (Comma separated list)
# .S Asm Files (Comma separated list)
# .h Header files (Comma separated list)
# .a Lib files (Comma separated list)
# --------------------------------------

def parseCmsisData(fileName):
    """
    Parses a .cmsisdata file to extract the information necessary for project connection file creation

    Returns a tuple of (project_name device_name core linker_script defineList includePathList cSrcList asmSrcList headersList libsList)
    """
    ProjectData = namedtuple("ProjectData", "projectName deviceName core linkerScript defineList includePathList cSrcList asmSrcList headersList libsList")

    with open(fileName, 'r') as fp:
        projectName = fp.readline().strip()
        device = fp.readline().strip()
        core = fp.readline().strip()
        linkerScript = fp.readline().strip()
        # filter() returns an iteration which can only be used once
        # hence converting it back to a list to able to iterate multiple times.
        defineList = list(filter(None, fp.readline().strip().split(',')))
        includePathList = list(filter(None, fp.readline().strip().split(',')))
        cSrcList = list(filter(None, fp.readline().strip().split(',')))
        asmSrcList = list(filter(None, fp.readline().strip().split(',')))
        headersList = list(filter(None, fp.readline().strip().split(',')))
        libsList = list(filter(None, fp.readline().strip().split(',')))

    return ProjectData(projectName, device, core, linkerScript, defineList, includePathList, cSrcList, asmSrcList, headersList, libsList)

def getDuplicateFiles(sourceList):
    seen = set()
    dups = []
    for file in sourceList:
        fileNameExt = os.path.basename(cleanUpPath(file))
        fileName = os.path.splitext(fileNameExt)[0]
        if fileName not in seen:
            seen.add(fileName)
        else:
            dups.append(fileNameExt)
    return dups

def cleanUpPath(path):
    """
    This function:
    1) Removes extra beginning/trailing quotes, spaces and newlines.
    2) Normalizes the paths.
    3) Appends './' to all paths. All paths are assumed to be relative to the Makefile.

    Returns a clean up path.
    """
    # Remove extra quotes and spaces
    cleanPath = path.strip()
    cleanPath = cleanPath.strip("\"")
    # The normalize operation needs to happen before prepend project directory 
    # variable operation. After appending the placeholder variable to the output 
    # is not a standard path so the normalize operation does not work correctly
    cleanPath = ntpath.normpath(cleanPath)
    # Append project dir
    if cleanPath != ".":
        cleanPath = ntpath.join('./', cleanPath)
    # Replace backslashes with slashes
    cleanPath = cleanPath.replace('\\', '/')
    return cleanPath

def cleanUpDefine(define):
    """
    This function:
    1) Removes extra beginning/trailing quotes, spaces and newlines.
    2) Changes to Windows style slashes for paths in defines.

    Returns a clean up path.
    """
    # Remove extra quotes and trailing spacess
    cleanDefine = define.strip()
    cleanDefine = REMOVE_QUOTE_REGEX.sub("", cleanDefine)
    # Normalize paths in defines if any present.
    cleanDefine = ntpath.normpath(cleanDefine)
    return cleanDefine

def printPretty(root):
    ugly = ElementTree.tostring(root, encoding='utf8', method='xml')
    parsed = minidom.parseString(ugly) 
    return parsed.toprettyxml(indent="  ", encoding='utf8')

def filterFiles(groupDict, fileList, category):
    """
    Filters the files into groups. This function currently only support MTB project structure.
    It assumes a libs directory that contains all libaries used by the project and  creates a 
    group for each folder under libs directory. All application source is added at the top level
    project under Keil uVision.
    """
    for fileName in fileList:
        cleanFile = cleanUpPath(fileName)
        # 'include' path should end with slash
        if category == 'include' and not cleanFile.endswith('/'):
            cleanFile += '/'
        dirsList = PurePath(cleanFile).parts
        # uVision with GCC toolchain creates *.o files in the project root
        # Exclude generated object files (auto-discovered in incremental exports)
        if cleanFile.endswith('.o') and len(dirsList) == 1:
            print("WARNING: object file {0} was not added to the generated .cpdsc, add manually as needed".format(cleanFile))
            continue
        try:
            # Find the first libs directory.
            index = dirsList.index("libs")
            # Any child of libs directory is a group.
            grp = dirsList[index + 1]
            groupDict[grp].append((cleanFile, category))
        except (ValueError, IndexError) as e:
            groupDict[GRP_UNFILTERED].append((cleanFile, category))

def createFileElement(groupElem, fileName, category):
    ElementTree.SubElement(groupElem, 'file',
        {'category': category, 'name': fileName})

def getRootElement(projectName):
    """
    Create root XML tree with mandatory root elements
    """
    # Need PACK.xsd version >= 1.6.0 for Pre_Include_Global_h
    root = ElementTree.Element('package', {
        'xmlns:xs': 'http://www.w3.org/2001/XMLSchema-instance',
        'schemaVersion': "1.6.0",
        'xs:noNamespaceSchemaLocation': 'PACK.xsd'})

    # Add mandatory root elements
    # https://www.keil.com/pack/doc/CMSIS/Pack/html/cpdsc_pg.html#element_package_cpdsc
    ElementTree.SubElement(root, 'vendor').text = "Cypress"
    ElementTree.SubElement(root, 'name').text = projectName
    ElementTree.SubElement(root, 'description').text = "Generated by ModusToolbox"
    ElementTree.SubElement(root, 'url').text = "http://cypress.com/modustoolbox"
    releasesElem = ElementTree.SubElement(root, 'releases')
    releaseElem = ElementTree.SubElement(releasesElem, 'release', { 'version': "1.0.0" })
    releaseElem.text = datetime.now().strftime("Generated by ModusToolbox at %d/%m/%Y %H:%M:%S")

    return root

def generatePdsc(inFile, cpdscFile, gpdscFile):
    """
    Generates the CMSIS Description files based on data from Make Build system.
    """
    projectData = parseCmsisData(inFile)
    if projectData.deviceName == None:
        raise Exception("Invalid file format for input file.\n")

    # Combine input file lists by groups
    groupDict = defaultdict(list)
    # Using CMSIS category 'source' for all firmware file types:
    # https://www.keil.com/pack/doc/CMSIS/Pack/html/pdsc_components_pg.html#FileCategoryEnum
    filterFiles(groupDict, projectData.includePathList, 'include')
    filterFiles(groupDict, projectData.headersList, 'source')
    filterFiles(groupDict, projectData.cSrcList, 'source')
    filterFiles(groupDict, projectData.asmSrcList, 'source')
    filterFiles(groupDict, projectData.libsList, 'library')

    cSrcDups = getDuplicateFiles(projectData.cSrcList)
    asmSrcDups = getDuplicateFiles(projectData.asmSrcList)
    if len(cSrcDups) != 0 or len(asmSrcDups) != 0:
        print("WARNING: Keil uVision does not support files with the same "
              "name even if they have different extensions. There were multiple source "
              "files detected with name(s):", end = ' ')
        print(*cSrcDups, sep = ";", end=' ')
        print(*asmSrcDups, sep = ";")

    generateCpdsc(projectData, groupDict, cpdscFile)
    generateGpdsc(projectData, groupDict, gpdscFile)

def generateCpdsc(projectData, groupDict, cpdscFile):
    """
    Generates CPDSC file
    """
    # Create root XML tree with mandatory root elements
    root = getRootElement(projectData.projectName)

    # Set package/compiler/language requirements
    requirementsElem = ElementTree.SubElement(root, 'requirements')
    packagesElem = ElementTree.SubElement(requirementsElem, 'packages')
    ElementTree.SubElement(packagesElem, 'package', { 'vendor': "Cypress", 'name': "PSoC6_DFP" })
    compilersElem = ElementTree.SubElement(requirementsElem, 'compilers')
    ElementTree.SubElement(compilersElem, 'compiler', { 'name': "ARMCC", 'version': "6.11.0" })
    languagesElem = ElementTree.SubElement(requirementsElem, 'languages')
    ElementTree.SubElement(languagesElem, 'language', { 'name': "C", 'version': "99" })

    # Determine CPU Pname
    processorName = ""
    if projectData.core == "CM0P":
        processorName = 'Cortex-M0p'
    elif projectData.core == "CM4":
        processorName = 'Cortex-M4'
    else:
        raise Exception("Core %s not supported by this export mechanism.\n" % projectData.core)

    # Set target device
    createElem = ElementTree.SubElement(root, 'create')
    projectElem = ElementTree.SubElement(createElem, 'project')
    targetElem = ElementTree.SubElement(projectElem, 'target',
        { 'Dname': projectData.deviceName, 'Pname': processorName, 'Dvendor': "Cypress:19"})

    # Add the application sources to the 'Source' group
    # The library sources are added by GPDSC
    filesElem = ElementTree.SubElement(projectElem, 'files')
    groupElem = ElementTree.SubElement(filesElem, "group", { 'name': "Source" })
    for fileName, category in groupDict[GRP_UNFILTERED]:
        createFileElement(groupElem, fileName, category)

    # Add the linker script from BSP (cannot be added by GPDSC)
    createFileElement(groupElem, projectData.linkerScript, 'linkerScript')

    # The printPretty function takes care of encoding hence the file is
    # opened in binary mode.
    with open(cpdscFile, 'wb') as opf:
        opf.write(printPretty(root))

    print("Generated CMSIS Project Description file: %s" % cpdscFile)

def generateGpdsc(projectData, groupDict, gpdscFile):
    """
    Generates GPDSC file
    """
    # Create root XML tree with mandatory root elements
    root = getRootElement(projectData.projectName)

    # Collect the list of global preincluded symbols
    includeText = ""
    # Exclude -DDEVICE from the list of defines - already added by CMSIS DFP
    deviceDefine = projectData.deviceName.replace('-', '_')
    for define in projectData.defineList:
        cleanDefine = cleanUpDefine(define).replace('=', ' ')
        if cleanDefine != deviceDefine:
            includeText += "\n#define {}".format(cleanDefine)
    includeElemAdded = False

    # Add the library sources
    componentsElem = ElementTree.SubElement(root, 'components')
    for grp in sorted(groupDict.keys()):
        if grp != GRP_UNFILTERED:
            componentElem = ElementTree.SubElement(componentsElem, 'component',
                {'Cvendor': "Cypress", 'Cclass': grp, 'Cgroup': "ModusToolbox" })
            if grp.startswith("TARGET") and not includeElemAdded:
                # Add Pre_Include_Global_h element to the first TARGET component
                includeElem = ElementTree.SubElement(componentElem, 'Pre_Include_Global_h')
                includeElem.text = includeText
                includeElemAdded = True
            filesElem = ElementTree.SubElement(componentElem, 'files')
            for fileName, category in groupDict[grp]:
                createFileElement(filesElem, fileName, category)

    if not includeElemAdded:
        print("WARNING: CMSIS exporter was not able to find the BSP library.")
        print("Preprocessor symbol definitions were NOT added to the generated project:")
        print(' '.join([cleanUpDefine(define) for define in projectData.defineList]))

    # The printPretty function takes care of encoding hence the file is
    # opened in binary mode.
    with open(gpdscFile, 'wb') as opf:
        opf.write(printPretty(root))

    print("Generated CMSIS Generator Package Description file: %s" % gpdscFile)

def is_valid_file(parser, arg):
    if not os.path.isfile(arg):
        parser.error("The file %s does not exist!" % arg)
    return arg

def main():
    argParser = argparse.ArgumentParser()
    argParser.add_argument("-i", dest="inFile", required=True,
                    help="Project data file generated by `make TOOLCHAIN=ARM uvision5`", metavar="FILE",
                    type=lambda x: is_valid_file(argParser, x))
    argParser.add_argument("-c", dest="cpdscFile", required=True, 
                    help="CMSIS Project Description file. By default specified by make build system: (APPNAME).cpdsc", metavar="FILE")
    argParser.add_argument("-g", dest="gpdscFile", required=True, 
                    help="CMSIS Generator Package Description file. By default specified by make build system: (APPNAME).gpdsc", metavar="FILE")

    args = argParser.parse_args()
    try:
        generatePdsc(args.inFile, args.cpdscFile, args.gpdscFile)
    except Exception as error:
        print("ERROR: %s" % error)
        sys.exit(1)

if __name__ == '__main__':
    main()
