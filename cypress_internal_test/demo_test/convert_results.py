# Convert regression aws_demos test result in text format
# to xml format for upload result to Test Rail

from lxml import etree
import argparse
import os

if __name__ == "__main__":
    argParser = argparse.ArgumentParser()
    argParser.add_argument("test_board", help="test board name")
    argParser.add_argument("file_result", help="file with test results")

    try:
        args = argParser.parse_args()
        test_board = args.test_board
        file_result = args.file_result
    except Exception as ex:
        print(ex)
        sys.exit(2)

    # use testrail uploader scripts for idt test
    path_result = os.path.join( os.path.split(file_result)[0], "device_tester/test_results/svt")
    file_result_xml = os.path.join(path_result , "FRQ_Report.xml")

    try:
        if not os.path.exists(path_result):
            os.makedirs(path_result)
    except OSError:
        print ("makedirs %s failed" % path_result)
        sys.exit(2)

    with open(file_result) as f:
        content = f.readlines()
    content = [x.strip() for x in content if 'make' in x]
    content_fail = [x.strip() for x in content if 'passed' not in x]

    # Create the root testsuites element
    root = etree.Element('testsuites',
                          name="SVT",
                          time='1',
                          tests=str(len(content)),
                          failures=str(len(content_fail)),
                          skipped='0',
                          errors='0',
                          disabled='0')

    # Create the testsuite element
    test_suite = etree.SubElement(root, 'testsuite',
                            name=test_board,
                            package="",
                            tests = str(len(content)),
                            failures = str(len(content_fail)),
                            time="1",
                            disabled = "0",
                            errors = "0",
                            skipped = "0",)

    # Make a new document tree
    doc = etree.ElementTree(root)

    for line in content:
        # parse test info line
        test, test_message  = line.split(":")
        test_name = "%s_%s" % (content.index(line), test.replace(' ','_').replace('.','_'))
        
        # Add the subelements
        test_suite_el = etree.SubElement(test_suite, 'testcase', classname='AFR', name=test_name)
        # Failed test cases
        if 'passed' not in line:
            etree.SubElement(test_suite_el, 'failure', type="", name=test_message)

    root.append(test_suite)

    # Save to XML file
    with open(file_result_xml, 'wb') as f:
        doc.write(f, pretty_print=True)
