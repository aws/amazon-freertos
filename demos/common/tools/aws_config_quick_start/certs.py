import boto3
import json

from iot_abstract import IotAbstract

def list_certificates():
    client = boto3.client('iot')
    certificates = client.list_certificates(pageSize=100)
    certificates = certificates['certificates']
    return certificates


class Certificate(IotAbstract):

    def __init__(self, certId = ''):
        self.client = boto3.client('iot')
        self.id = certId
        self.arn = ''
        if (self.id != ''):
            result = self.client.describe_certificate(certificateId=self.id)
            self.arn = result['certificateDescription']['certificateArn']
        self.certs = list_certificates()

    def create(self):
        assert self.exists() == False, "Cert already exists"
        cert = self.create_keys_and_certificate()
        self.id = cert["certificateId"]
        self.arn = cert["certificateArn"]
        self.certs = list_certificates()
        return cert

    def create_keys_and_certificate(self):
        result = self.client.create_keys_and_certificate(setAsActive=True)
        return result

    def delete(self):
        cert_not_found = True
        for cert in self.certs:
            if cert['certificateId'] == self.id:
                # Detach Policies attached to the cert
                policies_attached = self.list_policies()
                for policy in policies_attached:
                    self.detach_policy(policy['policyName'])

                # Detach Things attached to the cert
                things_attached = self.list_things()
                for thing in things_attached:
                    self.detach_thing(thing)
                # Update the status of the certificate to INACTIVE
                self.client.update_certificate(certificateId=self.id,
                    newStatus='INACTIVE')
                # Delete the certificate
                self.client.delete_certificate(certificateId=self.id)
                self.id = ''
                cert_not_found = False
                break
        return cert_not_found

    def exists(self):
        if self.id == '':
            return False
        else:
            return True

    def get_arn(self):
        return self.arn

    def list_policies(self):
        policies = self.client.list_principal_policies(principal=self.arn)
        policies = policies['policies']
        return policies

    def attach_policy(self, policy_name):
        self.client.attach_policy(policyName=policy_name,
            target=self.arn)

    def detach_policy(self, policy_name):
        self.client.detach_policy(policyName=policy_name,
            target=self.arn)

    def list_things(self):
        things = self.client.list_principal_things(principal=self.arn)
        things = things['things']
        return things

    def attach_thing(self, thing_name):
        self.client.attach_thing_principal(thingName=thing_name,
            principal=self.arn)

    def detach_thing(self, thing_name):
        self.client.detach_thing_principal(thingName=thing_name,
            principal=self.arn)
