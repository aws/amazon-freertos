# Presigned S3 URLs Generator 

**presigned_url_gen.py** generates pre-signed URLs for S3 GET and PUT HTTP request access.  

### Dependencies
* Python 2.7+ or Python 3+
* boto3
* argparse

### Pre-requisites
1. Install the dependencies.  
   ```
   pip install boto3 argparse
   ```

1. You will need an AWS Account with S3 access before beginning. You must install and configure the AWS CLI in order to 
   use this script.  
   For information on AWS S3 please see: https://docs.aws.amazon.com/AmazonS3/latest/dev/Welcome.html  
   For AWS CLI installation information please see: https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-install.html  
   For AWS CLI configuration information please see: https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-configure.html  
   ```
   aws configure
   ```

### Usage
1. Run the presigned_url_gen.py with your s3 **bucket-name** and s3 object key **object-key**.  
   ```
    python presigned_urls_gen.py --bucket <YOUR BUCKET NAME> --key <YOUR OBJECT KEY> 
   ```  
   An example expected output:  
   ```
   #define IOT_DEMO_HTTPS_PRESIGNED_GET_URL    https://aws-s3-endpoint/object-key.txt?AWSAccessKeyId=AAAAAAAAAAAAAAAAAAAA&Expires=1560555644&Signature=SomeHash12345UrlABcdEFgfIjK%3D  
   #define IOT_DEMO_HTTPS_PRESIGNED_PUT_URL    https://aws-s3-endpoint/object-key.txt?AWSAccessKeyId=ABABABABABABABABABAB&Expires=1560555644&Signature=SomeHash12345UrlLMnmOPqrStUvW%3D
   ```
1. Copy and paste the output to **iot_config.h** for macro **IOT_DEMO_HTTPS_PRESIGNED_GET_URL** and **IOT_DEMO_HTTPS_PRESIGNED_PUT_URL**.  
   ```
   #define IOT_DEMO_HTTPS_PRESIGNED_PUT_URL     https://aws-s3-endpoint/object-key.txt?AWSAccessKeyId=AAAAAAAAAAAAAAAAAAAA&Expires=1560555644&Signature=SomeHash12345UrlABcdEFgfIjK%3D
   #define IOT_DEMO_HTTPS_PRESIGNED_GET_URL     https://aws-s3-endpoint/object-key.txt?AWSAccessKeyId=AAAAAAAAAAAAAAAAAAAA&Expires=1560555644&Signature=SomeHash12345UrlABcdEFgfIjK%3D
   ```

### Parameter Info
#### --bucket  
    The name of your S3 bucket where you will download or upload.  
#### --key  
    The name of the existing object you wish to download (GET).  
    Or the name of the object you wish to upload (PUT).  
