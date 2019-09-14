#!/usr/bin/env python3
#
# Generation of common Makefile for CBMC proofs.
#
# Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import json
import sys
import re
import os
import argparse

def cleanup_whitespace(string):
    return re.sub(r'\s+', ' ', string.strip())

################################################################
# Operating system specific values

platform_definitions = {
    "linux": {
        "platform": "linux",
        "separator": "/",
        "define": "-D",
        "include": "-I",
    },
    "macos": {
        "platform": "darwin",
        "separator": "/",
        "define": "-D",
        "include": "-I",
    },
    "windows": {
        "platform": "win32",
        "separator": "\\",
        "define": "/D",
        "include": "/I",
    },
}


def patch_path_separator(opsys, string):
    from_separator = '/'
    to_separator = platform_definitions[opsys]["separator"]

    def escape_separator(string):
        return string.split(from_separator + from_separator)

    def change_separator(string):
        return string.replace(from_separator, to_separator)

    return from_separator.join([change_separator(escaped)
                                for escaped in escape_separator(string)])


def patch_compile_output(opsys, line, key, value):
    if opsys != "windows":
        return line

    if key in ["COMPILE_ONLY", "COMPILE_LINK"] and value is not None:
        if value[-1] == '/Fo':
            return re.sub(r'/Fo\s+', '/Fo', line)
        if value[-1] == '/Fe':
            return re.sub(r'/Fe\s+', '/Fe', line)
    return line


################################################################
# Makefile generation

def construct_definition(opsys, key_prefix, value_prefix, key, definitions):
    values = definitions.get(key)
    if not values:
        return ""
    if key in ["INC", "DEF"]:
        values = [patch_path_separator(opsys, value)
                  for value in values]

    lines = ["%s_%s = \\" % (key_prefix, key)]
    lines.extend(["\t{}{} \\".format(value_prefix, value) for value in values])
    lines.append("\t# empty")
    lines.append("")
    return lines


def get_define(define, defines):
    value = defines.get(define)
    if value:
        return value
    return []


def makefile_from_template(opsys, template, makefile_control):
    ret = []
    for line in template.splitlines():
        line = line.rstrip()
        line = patch_path_separator(opsys, line)
        keys = re.findall(r'@(\w+)@', line)
        values = [get_define(key, makefile_control["substitutions"])
                  for key in keys]
        for key, value in zip(keys, values):
            if value is not None:
                line = line.replace('@{}@'.format(key), value)
                line = patch_compile_output(opsys, line, key, value)
        ret.append(line)
    return ret


def cbmc_batch_yaml_target():
    return """
################################################################
# Build configuration file to run cbmc under cbmc-batch in CI

define encode_options
'=$(shell echo $(1) | sed 's/ ,/ /g' | sed 's/ /;/g')='
endef

cbmc-batch.yaml:
	@echo "Building $@"
	@$(RM) $@
	@echo "jobos: ubuntu16" >> $@
	@echo "cbmcflags: $(call encode_options,$(CBMCFLAGS) --unwinding-assertions)" >> $@
	@echo "goto: $(ENTRY).goto" >> $@
	@echo "expected: $(EXPECTED)" >> $@

################################################################
""".splitlines()


def get_makefile_choices(makefile_control, rules):
    ret = {}
    for k, v in rules["switches"].items():
        if k in makefile_control and makefile_control[k]:
            ret[k] = rules["switches"][k]["yes"]
        else:
            ret[k] = rules["switches"][k]["no"]
    return ret


def get_makefile(opsys, makefile_control):
    ret = []

    with open("conditional-rules.json") as handle:
        rules = json.load(handle)
    choices = get_makefile_choices(makefile_control, rules)

    with open("Makefile.template") as handle:
        template = handle.read()
    template = template.format(**choices)

    ret.extend(makefile_from_template(opsys, template, makefile_control))
    if opsys != "windows":
        ret.extend(cbmc_batch_yaml_target())

    return ret
