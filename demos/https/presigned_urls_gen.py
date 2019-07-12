#!/usr/bin/python

import boto3
import argparse

def get_presigned_urls(bucket_name, key_name):
    # Get the service client.
    s3 = boto3.client('s3')

    client_method_dict = {
        'GET': 'get_object',
        'PUT': 'put_object'
    }

    # Generate the URL to get 'key-name' from 'bucket-name'
    for method in client_method_dict.keys():
        url = s3.generate_presigned_url(
            ClientMethod=client_method_dict[method],
            Params={
                'Bucket': bucket_name,
                'Key': key_name
            }
        )
        print('#define IOT_DEMO_HTTPS_PRESIGNED_' + method + '_URL' + '    ' + '\"' + url + '\"')

def main():
    parser = argparse.ArgumentParser(description='S3 Presigned URL Generator. See README.md')
    parser.add_argument('--bucket', action='store', required=True, dest='bucket_name', help='The name of the S3 bucket of interest.')
    parser.add_argument('--key', action='store', required=True, dest='key_name', help='The name of the S3 Object in the bucket. This is referred to as a \'key\'')
    args = parser.parse_args()

    get_presigned_urls(args.bucket_name, args.key_name)

if __name__ == '__main__':  # pragma: no cover
    main()
