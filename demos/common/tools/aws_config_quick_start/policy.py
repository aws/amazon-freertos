import boto3
import json

from iot_abstract import IotAbstract

def list_policies():
    client = boto3.client('iot')
    policies = client.list_policies()
    policies = policies['policies']
    return policies

class Policy(IotAbstract):
    def __init__(self, name, policy=''):
        self.name = name
        self.policy = policy

    def create(self):
        try:
            client = boto3.client('iot')
            client.create_policy(policyName=self.name, policyDocument=self.policy)
            duplicate = False
        except Exception:
            duplicate = True
        return duplicate

    def delete(self):
        try:
            client = boto3.client('iot')
            client.delete_policy(policyName=self.name)
            err = False
        except Exception:
            err = True
        return err

    def get(self):
        client = boto3.client('iot')
        policies = client.list_policies()
        for policy in policies:
            if self.name == policy['policyName']:
                return policy
        return False

    def exists(self):
        pass