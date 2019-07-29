#!/bin/bash

# CREATING DEVICE CERTIFICATE
echo "Creating Device Certificate"
openssl x509 -req -in deviceCert.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out deviceCert.crt -days 365 -sha256

echo "Creating File Containing Device Certificate and CA Certificate"
cat deviceCert.crt rootCA.pem > deviceCertAndCACert.crt

