import boto3
import json

from iot_abstract import IotAbstract

def list_things():
    client = boto3.client('iot')
    things = client.list_things()
    things = things['things']
    return things


class Thing(IotAbstract):
    def __init__(self, name):
        self.client = boto3.client('iot')
        self.name = name
        self.arn = ''

    def create(self):
        already_exists = False

        if self.exists() == True:
            already_exists = True
        else:
            result = self.client.create_thing(thingName=self.name)
            self.arn = result['thingArn']

        return already_exists

    def delete(self):
        principals = self.list_principals()
        for principal in principals:
            self.detach_principal(principal)

        self.client.delete_thing(thingName=self.name)

    def exists(self):
        list_of_things = list_things()
        for thing in list_of_things:
            if thing['thingName'] == self.name:
                return True
        return False

    def attach_principal(self, arn):
        self.client.attach_thing_principal(thingName=self.name,
            principal=arn)


    def detach_principal(self, arn):
        self.client.detach_thing_principal(thingName=self.name,
            principal=arn)


    def list_principals(self):
        principals = self.client.list_thing_principals(thingName=self.name)
        principals = principals['principals']
        return principals
