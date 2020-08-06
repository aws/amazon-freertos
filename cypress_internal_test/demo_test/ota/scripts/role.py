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

# The variables below are roles and policies that are used if you are evaluating Role alone. Update values of <account_id> and<role_name accordingly.
role_name='TestRole'
description='A test Role'

trust_policy={
  "Version": "2012-10-17",
  "Statement": [
    {
      "Sid": "",
      "Effect": "Allow",
      "Principal": {
        "Service": "iot.amazonaws.com"
      },
      "Action": "sts:AssumeRole"
    }
  ]
}

Policy_Arn='arn:aws:iam::aws:policy/service-role/AmazonFreeRTOSOTAUpdate'

policy_name = 'testinlinepolicy'
policy_json = {
    "Version": "2012-10-17",
    "Statement": [
      {
            "Effect": "Allow",
            "Action": [
                "iam:GetRole",
                "iam:PassRole"
            ],
            "Resource": "arn:aws:iam::<account_id>:role/<role_name>"
      }
    ]
}

class Role():
    def __init__(self, role_name):
        self.client = boto3.client('iam')
        self.name = role_name

    def create(self,trust_policy):
        assert self.exists() == False, "Role already exists"
        try:
            self.client.create_role(RoleName=self.name,AssumeRolePolicyDocument=json.dumps(trust_policy))
        except Exception as e:
            logging.error(e)
            return False
        return True

    def exists(self):
        response = self.client.list_roles()
        if 'Roles' in response:
                for index in range(len(response['Roles'])):
                    for key in response['Roles'][index]:
                        if key == 'RoleName':
                            if response['Roles'][index][key] == self.name:
                                return True
        return False

    def attach_managed_policy(self,Policy_Arn):
        self.client.attach_role_policy(RoleName=self.name,PolicyArn=Policy_Arn)

    def attach_inline_policy(self,policy_name,policy_json):
        self.client.put_role_policy(RoleName=self.name, PolicyName=policy_name, PolicyDocument=json.dumps(policy_json))

# Creates and attaches policies for creating an OTA role
    def init_role_policies(self,account_id,bucket_name):
        with open('RoleTrustPolicy.json',"r") as file:
            json_text = json.load(file)
        if(True == self.create(json_text)):
            self.attach_managed_policy(Policy_Arn)
            with open('RolePolicy.json',"r") as file:
                json_text = json.load(file)
            json_text['Statement'][0]['Resource'] = "arn:aws:iam::"+account_id+":role/"+self.name
            self.attach_inline_policy('RolePolicy',json_text)
            with open('RolePolicy.json',"w") as file:
                json.dump(json_text,file)
            with open('RoleS3Policy.json',"r") as file:
                json_text = json.load(file)
            json_text['Statement'][0]['Resource'][0] = "arn:aws:s3:::"+bucket_name+"/*"
            json_text['Statement'][0]['Resource'][1] = "arn:aws:s3:::"+bucket_name
            self.attach_inline_policy('RoleS3Policy',json_text)
            with open('RoleS3Policy.json',"w") as file:
                json.dump(json_text,file)
            return True
        else:
            return False

def main(argv):
    test=Role(role_name)
    test.create(trust_policy)
    test.attach_managed_policy(Policy_Arn)
    test.attach_inline_policy(policy_name,policy_json)

if __name__ == "__main__":
    main(sys.argv[1:])
