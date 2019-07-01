# Presigned S3 URL Generator 

**presigned_url_gen.py** generates a pre-signed URL for S3 GET and PUT HTTP request access. 

### Dependencies
* Python 2.7+ or Python 3+
* boto3
* argparse

### Pre-requisites
1. Install the dependencies.
   ```
   pip install boto3 arparse
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
   python presigned_url_gen.py --bucket my-bucket --key object-key
   ```  
   An example expected output:
   ```
   https://my-bucket.s3.amazonaws.com/object-key.txt?AWSAccessKeyId=AAAAAAAAAAAAAAAAAAAA&Expires=1560555644&Signature=SomeHash12345UrlABcdEFgfIjK%3D
   ```
1. Copy and paste the output to **iot_config.h** for macro **IOT_DEMO_HTTPS_PRESIGNED_URL**.
   ```
   #define IOT_DEMO_HTTPS_PRESIGNED_URL     https://my-bucket.s3.amazonaws.com/object-key.txt?AWSAccessKeyId=AAAAAAAAAAAAAAAAAAAA&Expires=1560555644&Signature=SomeHash12345UrlABcdEFgfIjK%3D
   ```