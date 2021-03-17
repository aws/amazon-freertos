# Introduction
This script is intended for deletion of AWS S3 Buckets that are created by runs
of the IoT Device Tester (IDT) in our Continuous Integration (CI) system. These
Buckets are not routinely cleaned up and can cause problems when we exceed our
AWS Bucket quota.

# Prerequisites
- [AWS CLI 2](https://docs.aws.amazon.com/cli/latest/userguide/install-cliv2.html)
- [Python 3.7+](https://www.python.org/downloads/)
- [pip](https://pip.pypa.io/en/stable/installing/)

# Setup
In order to run this script, you will need to have the AWS CLI 2 installed and
configured. To configure the CLI, follow [these
instructions](https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html#cli-configure-quickstart-config).
To prepare your environment for running this script, use the following command
in this directory:
```
pip install -r requirements.txt
```

# Usage
The script operates on an input file containing one Bucket identifer per line.
Bucket identifiers can be found via the AWS CLI command '[aws s3
ls](https://docs.aws.amazon.com/cli/latest/reference/s3/ls.html)'. As a
guideline, only Buckets with the 'afr-ota-idt' should be selected for deletion.
Other Buckets are in use for personal test testing of various features.

Run the script with this command:
```
python bucketcleanup.py <path to file>
```

The output on success will appear like this:
```
FyallIan@ISCNPF1PMK8Y MINGW32 ~/afr/gitlab/amazon-freertos/cypress_internal_test/idtcleanup (cy_P064_ES100.3)
$ python bucketcleanup.py ../../../../../buckets2delete.txt
Bucket to delete ' afr-ota-idt-4404754610712529574 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-1808820473822354985 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-3068516386360819756 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-4993858175973188459 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-8850953913180417017 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-1262902484341913008 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-6801163509187260314 '.
Bucket deleted.
Bucket to delete ' afr-ota-idt-4082445695424983336 '.
Bucket deleted.
Bucket cleanup complete.
```

Errors reported by AWS S3 will be printed on standard out:
```
FyallIan@ISCNPF1PMK8Y MINGW32 ~/afr/gitlab/amazon-freertos/cypress_internal_test/idtcleanup (cy_P064_ES100.3)
$ python bucketcleanup.py ../../../../../buckets2delete.txt
Bucket to delete ' afr-ota-idt-3221364499465752114 '.
Failed to delete objects from current bucket.
Error message: The specified bucket does not exist
```

The script will run through the full list of Buckets in the input file,
regardless of any reported errors.