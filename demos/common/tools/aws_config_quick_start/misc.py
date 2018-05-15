import os
import boto3
import shutil
import sys
import json


def describe_endpoint():
    client = boto3.client('iot')
    endpoint = client.describe_endpoint()
    return endpoint['endpointAddress']

def get_account_id():
    client = boto3.client('sts')
    aws_account_id = client.get_caller_identity()['Account']
    return aws_account_id.strip('\n')

def get_aws_region():
    my_session = boto3.session.Session()
    aws_region = my_session.region_name
    return aws_region.strip('\n')

def create_policy_document():
    this_file_directory = os.getcwd()
    policy_document = os.path.join(this_file_directory, 'policy_document.templ')
    region_name = str(get_aws_region())
    aws_account_id  = str(get_account_id())
    with open(policy_document) as policy_document_file:
        policy_document_text = policy_document_file.read()

    # Replace Account ID and AWS Region
    policy_document_text = policy_document_text.replace('<aws-region>', region_name)
    policy_document_text = policy_document_text.replace('<aws-account-id>', aws_account_id)

    return policy_document_text

def format_credential_text(credentialText):
    credentialTextLines = credentialText.split('\n')
    formattedCredentialTextLines = []

    for credentialTextLine in credentialTextLines:
        if credentialTextLine.strip():
            formattedCredentialTextLine = '"' + credentialTextLine + '\\n"'
            formattedCredentialTextLines.append(formattedCredentialTextLine)

    formattedCredentialText = '\n'.join(formattedCredentialTextLines)
    return formattedCredentialText

def client_credential(**kwargs):
    filename = ''
    new_text = ''
    file_to_modify = ''
    this_file_directory = os.getcwd()
    common_directory = os.path.dirname(os.path.dirname(this_file_directory))


    if (kwargs["credentials_or_keys"] == "client_credential"):
        assert 5 >= len(kwargs),"Less than 5 arguments"
        filename = "aws_clientcredential.templ"
        with open(filename,'r') as file:
            file_text = file.read()
        new_text = file_text.replace("<WiFiSSID>", "\"" + kwargs["wifi_ssid"] + "\"")
        new_text = new_text.replace("<WiFiPasswd>", "\"" + kwargs["wifi_passwd"] + "\"")
        new_text = new_text.replace("<WiFiSecurity>", kwargs["wifi_security"])
        new_text = new_text.replace("<IOTEndpoint>", "\"" + describe_endpoint() + "\"")
        new_text = new_text.replace("<IOTThingName>", "\""+ kwargs["thing_name"] + "\"")
        file_to_modify = os.path.abspath(os.path.join(common_directory,'include','aws_clientcredential.h'))

    elif (kwargs["credentials_or_keys"] == "client_keys"):
        assert 3 >= len(kwargs),"Less than 3 arguments"
        filename = "aws_clientcredential_keys.templ"
        with open(filename,'r') as file:
            file_text = file.read()
        new_text = file_text.replace("<ClientCertificatePEM>", format_credential_text(kwargs["client_certificate_pem"]))
        new_text = new_text.replace("<ClientPrivateKeyPEM>", format_credential_text(kwargs["clientprivate_key_pem"]))
        file_to_modify = os.path.abspath(os.path.join(common_directory,'include','aws_clientcredential_keys.h'))
    else:
        print("Invalid option")
        sys.exit(1)

    file = open(str(file_to_modify),'w')
    file.write(new_text)
    file.close()

