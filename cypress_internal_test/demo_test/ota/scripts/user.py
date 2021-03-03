# (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# A copy of the License is located at
#     http://www.apache.org/licenses/LICENSE-2.0
# or in the "license" file accompanying this file. This file is distributed 
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
# express or implied. See the License for the specific language governing 
# permissions and limitations under the License.
#

import logging
import boto3
from botocore.exceptions import ClientError
import json
import sys

# The variables below are test user and policies that are used if you are evaluating User module alone. Update values of <account_id>,<bucket_name> and<role_name accordingly.
user_name='User1'
policy_arn='arn:aws:iam::aws:policy/service-role/AmazonFreeRTOSOTAUpdate'
policy_name='test_usr_policy'
policy_doc={
    "Version": "2012-10-17",
    "Statement": [
        {
            "Effect": "Allow",
            "Action": [
                "s3:ListBucket",
                "s3:ListAllMyBuckets",
                "s3:CreateBucket",
                "s3:PutBucketVersioning",
                "s3:GetBucketLocation",
                "s3:GetObjectVersion",
                "acm:ImportCertificate",
                "acm:ListCertificates",
                "iot:*",
                "iam:ListRoles",
                "freertos:ListHardwarePlatforms",
                "freertos:DescribeHardwarePlatform"
            ],
            "Resource": "*"
        },
        {
            "Effect": "Allow",
            "Action": [
                "s3:GetObject",
                "s3:PutObject"
            ],
            "Resource": "arn:aws:s3:::<bucket_name>/*"
        },
        {
            "Effect": "Allow",
            "Action": "iam:PassRole",
            "Resource": "arn:aws:iam::<account_id>:role/<role_name>"
        }
    ]
}

class User():
    def __init__(self,user_name):
        self.client = boto3.client('iam')
        self.name = user_name

    def create(self):
        assert self.exists() == False, "User already exists"
        response =self.client.create_user(UserName=self.name)
        print(response)
   
    def exists(self):
        response = self.client.list_users()
        if 'Users' in response:
                for index in range(len(response['Users'])):
                    for key in response['Users'][index]:
                        if key == 'UserName':
                            if response['Users'][index][key] == self.name:
                                return True
        return False

    def attach_managed_policy(self,policy_arn):
        response = self.client.attach_user_policy(UserName=self.name, PolicyArn=policy_arn)
        print(response)

    def attach_inline_policy(self,policy_name,policy_doc):
        response = self.client.put_user_policy( PolicyDocument=json.dumps(policy_doc),
        PolicyName=policy_name, UserName=self.name)
        print(response)

def main(argv):
    test=User(user_name)
    test.create()
    test.attach_managed_policy(policy_arn)
    test.attach_inline_policy(policy_name,policy_doc)

if __name__ == "__main__":
    main(sys.argv[1:])