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

class S3Bucket():
    def __init__(self,name,region=None):

        self.client = boto3.client('s3')
        self.name = name
        self.region = region

    def create(self):
        #assert self.exists() == False, "Bucket already exists"
        try:
            if self.region == 'us-east-1' or self.region is None:
                self.client.create_bucket(Bucket = self.name)
            else:
                location = {'LocationConstraint': self.region}
                self.client.create_bucket(Bucket = self.name,CreateBucketConfiguration=location)
            self.client.put_bucket_versioning(Bucket=self.name,VersioningConfiguration={'MFADelete':'Disabled', 'Status':'Enabled'})
        except ClientError as e:
            logging.error(e)
            return False
        return True

    def delete(self):
        #assert self.exists() == True, "Bucket does not exist"
        obj_details=[]
        temp_key=''
        temp_version=''
        data = self.client.list_object_versions(Bucket=self.name)
        try:
            if 'Versions' in data:
                for index in range(len(data['Versions'])):
                    for key in data['Versions'][index]:
                        if key == 'Key':
                            temp_key = data['Versions'][index][key]
                        if key == 'VersionId':
                            temp_version = data['Versions'][index][key]
                        if temp_key != '' and temp_version !='':
                            obj_details.append({'Key':temp_key,'VersionId':temp_version})
                            temp_key =''
                            temp_version=''
                print(obj_details)
                self.client.delete_objects(Bucket=self.name, Delete={'Objects': obj_details})
            else:
                print("No objects in the bucket \n")
            self.client.delete_bucket(Bucket=self.name)
        except ClientError as e:
            logging.error(e)
            return False
        return True

        
    def exists(self):
        list_of_buckets = self.client.list_buckets()['Buckets']
        for bucket in list_of_buckets:
            if bucket['Name'] == self.name:
                return True
        return False

def main(argv):
    s3bucket_test = S3Bucket('sarthakpytestbucket')
    if True == s3bucket_test.create():
        print("Created Bucket")
    else:
        print("Failed to create bucket")
    if True == s3bucket_test.delete():
        print("Deleted bucket")
    else:
        print("Failed to delete bucket")

if __name__ == "__main__":
    main(sys.argv[1:])
