#!/usr/bin/env python

import sys
import botocore
import boto3

# usage: deletes a list of buckets provided by an input file. Each bucket must
# be on its own line.
#
# example:
#   python bucketcleanup.py file.txt

filename = sys.argv[1]

with open(filename, "r") as inputFile:
	bucketList = inputFile.readlines()

s3 = boto3.resource('s3')

for targetBucketDirty in bucketList:
    targetBucket = targetBucketDirty.strip()
    print("Bucket to delete '", targetBucket, "'.")
    bucket = s3.Bucket(targetBucket)

    try:
        deletedDict = bucket.object_versions.delete()
    
    # the return type for object_versions.delete() return type is documented
    # here:
    # https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/s3.html#S3.Bucket.delete
    except botocore.exceptions.ClientError as error:
        print("Failed to delete objects from current bucket.")
        print("Error message: {}".format(error.response['Error']['Message']))
    else:
        try:
            bucket.delete()
        except botocore.exceptions.ClientError as error:
            print("Failed to delete objects from current bucket.")
            print("Error message: {}".format(error.response['Error']['Message']))
        else:        
            print("Bucket deleted.")
        
print("Bucket cleanup complete.")        