import argparse
import json
import thing
import policy
import certs
import misc

def setup():
    with open('configure.json') as file:
        json_text = json.load(file)

    # Create a Thing
    thing_name = json_text['thing_name']
    thing_obj = thing.Thing(thing_name)
    if not thing_obj.create():

        # Create a Certificate
        cert_obj = certs.Certificate()
        result = cert_obj.create()

        # Store cert_pem
        cert_pem = result['certificatePem']

        # Store Private key PEM
        private_key_pem = result['keyPair']['PrivateKey']

        wifi_ssid = json_text['wifi_ssid']
        wifi_passwd = json_text['wifi_password']
        wifi_security = json_text['wifi_security']

        # Modify 'aws_clientcredential.h' file
        misc.client_credential(
            wifi_ssid = wifi_ssid,
            wifi_passwd = wifi_passwd,
            wifi_security = wifi_security,
            thing_name = thing_name,
            credentials_or_keys = "client_credential")

        # Modify 'aws_clientcredential_keys.h' file
        misc.client_credential(
            client_certificate_pem = cert_pem,
            clientprivate_key_pem = private_key_pem,
            credentials_or_keys = "client_keys")

        # Create a Policy
        policy_document = misc.create_policy_document()
        policy_name = thing_name + '_amazon_freertos_policy'
        policy_obj = policy.Policy(policy_name, policy_document)
        policy_obj.create()

        # Attach certificate to Thing
        cert_obj.attach_thing(thing_name)

        # Attach policy to certificate
        cert_obj.attach_policy(policy_name)

def cleanup():
    certIdlength = 64
    with open('configure.json') as file:
        json_text = json.load(file)

    thing_name = json_text['thing_name']
    thing_obj = thing.Thing(thing_name)

    principals = thing_obj.list_principals()

    # Delete certificates and policies attached to thing
    for eachPrincipal in principals:
        certId = eachPrincipal[-certIdlength:]
        certarn = eachPrincipal
        cert_obj = certs.Certificate(certId)

        # Get policies attached to certificate
        policies_attached = cert_obj.list_policies()

        # Delete certificate
        cert_obj.delete()

        # Delete policies attached to the certificate
        for each_policy in policies_attached:
            policy_obj = policy.Policy(each_policy['policyName'])
            policy_obj.delete()

    # Delete Thing
    thing_obj.delete()

def list_certificates():
    print(certs.list_certificates())

def list_things():
    print(thing.list_things())

def list_policies():
    print(policy.list_policies())

if __name__ == "__main__":

    arg_parser = argparse.ArgumentParser()
    sub_arg_parser = arg_parser.add_subparsers(help='Available commands',
        dest='command')
    setup_parser = sub_arg_parser.add_parser('setup', help='setup aws iot')
    clean_parser = sub_arg_parser.add_parser('cleanup', help='cleanup aws iot')
    list_cert_parser = sub_arg_parser.add_parser('list_certificates',
        help='list certificates')
    list_thing_parser = sub_arg_parser.add_parser('list_things',
        help='list things')
    list_policy_parser = sub_arg_parser.add_parser('list_policies',
        help='list policies')
    args = arg_parser.parse_args()

    if args.command == 'setup':
        setup()
    elif args.command == 'cleanup':
        cleanup()
    elif args.command == 'list_certificates':
        list_certificates()
    elif args.command == 'list_things':
        list_things()
    elif args.command == 'list_policies':
        list_policies()
    else:
        print("Command does not exist")




