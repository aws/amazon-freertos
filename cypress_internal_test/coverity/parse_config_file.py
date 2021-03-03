import yaml
import sys
import os

config_data = {}
output_variables = []
file = sys.argv[1]
identifier = sys.argv[2]

# Read the configuration file
with open(file, 'r') as stream:
    try:
        config_data = yaml.safe_load(stream)
    except yaml.YAMLError as exc:
        print(exc)

# Generate variables for each of the configuration items
for k,v in config_data.items():
    if type(v) is str:
        output_variables.append('{}{}="{}"'.format(identifier, k, v))
    elif type(v) is list:
        if k == "ENABLE_CHECKERS":
            enable_list = ["-en {}".format(x) for x in v]
            output_variables.append("{}{}=\"{}\"".format(identifier, k, " ".join(enable_list)))
        elif k == "DISABLE_CHECKERS":
            disable_list = ["-n {}".format(x) for x in v]
            output_variables.append("{}{}=\"{}\"".format(identifier, k, " ".join(disable_list)))
        elif k == "CUSTOM_CHECKER":
            output_variables.append("{}{}=\"{}\"".format(identifier, k, " ".join(v)))
        elif k == "SOURCE_FILTER":
            output_variables.append("{}{}=\"{}\"".format(identifier, k, " && ".join(v)))
        elif k == "ANALYZE_OPTS":
            output_variables.append("{}{}=\"{}\"".format(identifier, k, " ".join(v)))

# Show the generated variables.  parse_config_file.py is run from bash using eval.  This
# will create bash variables of all generated variables.
for output_var in output_variables:
    print(output_var)
