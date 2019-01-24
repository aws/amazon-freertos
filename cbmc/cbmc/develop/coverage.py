#!/usr/bin/env python

"""CBMC coverage analysis."""

import argparse
import re
from pprint import pprint

def command_line_parser():
    """Create the command line parser."""

    parser = argparse.ArgumentParser(
        description='CBMC coverage analysis'
    )
    parser.add_argument(
        'files',
        nargs=argparse.REMAINDER
    )
    return parser

def extract_coverage_data_from_line(line):
    """Extract coverage data from a line of cbmc coverage data."""

    match = re.search(r'block [0-9]+ \(lines ([^ ]*)\).*(SATISFIED|FAILED)',
                      line)
    if match:
        (data, string) = match.groups()
        kind = 'hit' if string == 'SATISFIED' else 'miss'

        # data is block;block;block
        # block is file:function:lines
        # lines is 1,2,3,4,5

        # Ignore leftmost : in a block c:\foo\bar:baz:1,2,3 from Windows
        (filename, funcname, lineset) = data.rsplit(':', 2)
        lines = [int(line) for line in lineset.split(',')]
        return (kind, filename, funcname, lines)
    return None

def extract_coverage_data_from_file(datafile):
    """Extract coverage data from cbmc coverage data."""

    coverage = {}

    def insert_coverage_data(kind, filename, funcname, lines):
        """Insert coverage data into aggregated coverage data"""

        if coverage.get(filename) is None:
            coverage[filename] = {}
        if coverage[filename].get(funcname) is None:
            coverage[filename][funcname] = {'lines': set(), 'hit': set()}

        line_set = set(lines)
        # take union of line sets
        coverage[filename][funcname]['lines'].update(line_set)
        if kind == 'hit':
            # take union of hit line sets
            coverage[filename][funcname]['hit'].update(line_set)

    with open(datafile) as lines:
        for line in lines:
            data = extract_coverage_data_from_line(line)
            if data:
                (kind, filename, funcname, lines) = data
                insert_coverage_data(kind, filename, funcname, lines)

    return coverage

def extract_coverage_data_from_files(datafiles):
    """Extract coverage data from a list of cbmc coverage files."""

    # Function name longer than 32 characters
    # pylint: disable=invalid-name

    coverage_data = []
    for datafile in datafiles:
        data = extract_coverage_data_from_file(datafile)
        coverage_data.append(data)
        #print "\none extracted data"
        #pprint(data)

    return merge_coverage_data(coverage_data)

def merge_coverage_data(coverage_data):
    results = {}

    for data in coverage_data:
        # data: filename -> funcname -> {"lines", "hit"} -> line set
        for filename in data:
            if results.get(filename) is None:
                results[filename] = {}
            for funcname in data[filename]:
                if results[filename].get(funcname) is None:
                    results[filename][funcname] = {'lines': set(), 'hit': set()}

                #print "\ndata"
                #pprint(data)

                # take union of line sets
                (results[filename][funcname]['lines']
                 .update(data[filename][funcname]['lines']))
                # take union of hit line sets
                (results[filename][funcname]['hit']
                 .update(data[filename][funcname]['hit']))

                #print "\nresults"
                #pprint(results)

    for filename in results:
        for funcname in results[filename]:
            # missed lines = lines - hit lines
            miss = (results[filename][funcname]['lines']
                    .difference(results[filename][funcname]['hit']))
            results[filename][funcname]['miss'] = miss

    #print "\nmerged results"
    #pprint(results)

    return results

def coverage_lists(coverage_data):
    coverage = {}
    for filename in coverage_data:
        coverage[filename] = {}
        for funcname in coverage_data[filename]:
            coverage[filename][funcname] = {}
            for kind in coverage_data[filename][funcname]:
                coverage[filename][funcname][kind] = sorted(coverage_data[filename][funcname][kind])
    return coverage

def main():
    "Do property checking."

    parser = command_line_parser()
    args = parser.parse_args()

    coverage = extract_coverage_data_from_files(args.files)
    coverage = coverage_lists(coverage)
    #print "\ncoverage"
    #pprint(coverage)

    total = {'lines':0, 'hit':0, 'miss':0, 'func':0}
    print "Coverage"
    for filename in coverage:
#        print filename
        for funcname in coverage[filename]:
            data = coverage[filename][funcname]
            lines = len(data['lines'])
            hit = len(data['hit'])
            miss = len(data['miss'])
            total['lines'] += lines
            total['hit'] += hit
            total['miss'] += miss
            total['func'] += 1
            print ("  {}: {:.2f}: {} lines, {} hit, {} miss"
                   .format(funcname, float(hit)/float(lines), lines, hit, miss))

    print ("Summary: {:.2f}: {} lines, {} hit, {} miss, {} functions"
           .format(float(total['hit'])/float(total['lines']), total['lines'], total['hit'], total['miss'], total['func']))

if __name__ == "__main__":
    main()
