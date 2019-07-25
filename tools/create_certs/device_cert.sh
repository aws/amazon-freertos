#!/bin/bash

# CREATING DEVICE CERTIFICATE
echo "Creating Device Certificate"
openssl x509 -req -in deviceCert.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out deviceCert.crt -days 365 -sha256

echo "Creating File Containing Device Certificate and CA Certificate"
cat deviceCert.crt rootCA.pem > deviceCertAndCACert.crt

# echo "Connecting to AWS IoT Core Using MQTT Mosquitto"
# mosquitto_pub --cafile root.cert --cert deviceCertAndCACert.crt --key deviceCert.key -h a2hzqyax9o1q8r.iot.us-west-1.amazonaws.com -p 8883 -q 1 -t foo/bar -I anyclientID --tls-version tlsv1.2 -m "Hello" -d
# # how to get device public key here?