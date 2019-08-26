xxd -r -ps DevicePublicKeyAsciiHex.txt DevicePublicKeyDer.bin
openssl ec -inform der -in DevicePublicKeyDer.bin -pubout -outform pem -out DevicePublicKey.pem

https://docs.aws.amazon.com/iot/latest/developerguide/device-certs-your-own.html

openssl genrsa -out securityOfficer.key 2048
openssl req -new -key securityOfficer.key -out deviceCert.csr
openssl x509 -req -in deviceCert.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out deviceCert.pem -days 500 -sha256 -force_pubkey DevicePublicKey.pem
