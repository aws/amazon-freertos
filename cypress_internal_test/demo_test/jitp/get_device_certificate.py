
import os
import sys
from cysecuretools import CySecureTools
from cysecuretools.execute.sys_call import get_prov_details
from cysecuretools.execute.programmer.programmer import ProgrammingTool
from cysecuretools.execute.provisioning_lib.cyprov_pem import PemKey
from OpenSSL import crypto, SSL

boardID = sys.argv[1]

print ("boardID: %s" % boardID)

cytools = CySecureTools('cy8ckit-064b0s2-4343w')

def read_device_certificate():
    tool = ProgrammingTool.create(cytools.PROGRAMMING_TOOL)
    # Read Device certificate and save
    if tool.connect(cytools.target_name, probe_id=boardID):
        print('Reading device certificate from device')
        result, key = get_prov_details(tool, cytools.register_map, 0x200)
        if not result:
            print('Error: Cannot read device certificate.')
            return
        dev_cert_json = 'device_cert.json'
        with open(dev_cert_json, 'w') as json_file:
            json_file.write(key)
    print('Device certificate has been read successfully.')

if __name__ == "__main__":
    read_device_certificate()

