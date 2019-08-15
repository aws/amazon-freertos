/*
 * Amazon FreeRTOS V201906.00 Major
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUVXl0YWO1KmGoEP3vSraLd9rzIVIwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDgxNTIwNTYy\n"\
"M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL6+FVyRXkTzwJ2vV3OH\n"\
"HHNDZeGC5OhDL1Wx63VNvX0g991NGmaoVaoBZirn8GgU9hgUabKxEFY3AKkiRToh\n"\
"vaMKxcx9EIkY07zo8NLcp8SYKTr2ZYC5qo8r1Ohr5Ms4iU6yiG0kztNTpbwA4auv\n"\
"hfIvnpIq6dX1vbHVUKuAfx19zCWASkC6iSRsl9F8yvDGme2Lh5f0jVhdnxHdAJ3a\n"\
"DY+1VY1qxC/rQtVr37dsK0xT5RLa31Pwf7bENXTzK4BidGfnHa2sA3YZLKmO0lEF\n"\
"JjZpum+jsgx5f15AecjOvw2ZI4Hk4mkR/okRBm7MxbP4d0f9LPSx8upGmaxvyD76\n"\
"yOsCAwEAAaNgMF4wHwYDVR0jBBgwFoAU7+iDu/6SxNO7ZwsoMLUJ0NRhkTEwHQYD\n"\
"VR0OBBYEFB192oAAsMekuqtHc73wrOYl9+E3MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBaKkGcRFyW4iBo81BXS6dizztj\n"\
"V3ykUdkoFn/IvThbQlp2eFv2kEUH3z/6ghGD5lRdrm6wwO3nBFeDWZd+FGYQEogr\n"\
"how33E2BeE7G2FVTv7fDZSM63AShx5dDdF8N7uP6t0IZqyo7GnAA64y1I/6pLLKJ\n"\
"ttDRxH51IKw/SST4iOXOSuUbk3i8x+aUaR7NTQTg1rVfdclmEaUBQfhuSpEWGu7o\n"\
"zmCEAzZpT17nVvsYsK2acDcgokSav3eP1/MwirILt1HBijCA7Db97GECL59hELPQ\n"\
"Za7BhHu8TO6/OMg1xs7nsBvy2yZNDVwmWZ03UhMhYotAS1yMKrVM43+8iobx\n"\
"-----END CERTIFICATE-----\n"

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate 
 * Authority) of the client certificate is used by the server for routing the 
 * device's initial request. (The device client certificate must always be 
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html, 
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 * 
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpAIBAAKCAQEAvr4VXJFeRPPAna9Xc4ccc0Nl4YLk6EMvVbHrdU29fSD33U0a\n"\
"ZqhVqgFmKufwaBT2GBRpsrEQVjcAqSJFOiG9owrFzH0QiRjTvOjw0tynxJgpOvZl\n"\
"gLmqjyvU6GvkyziJTrKIbSTO01OlvADhq6+F8i+ekirp1fW9sdVQq4B/HX3MJYBK\n"\
"QLqJJGyX0XzK8MaZ7YuHl/SNWF2fEd0AndoNj7VVjWrEL+tC1Wvft2wrTFPlEtrf\n"\
"U/B/tsQ1dPMrgGJ0Z+cdrawDdhksqY7SUQUmNmm6b6OyDHl/XkB5yM6/DZkjgeTi\n"\
"aRH+iREGbszFs/h3R/0s9LHy6kaZrG/IPvrI6wIDAQABAoIBACUmKErrFQ0g3DbZ\n"\
"25JokiXwznrpX2PNEf8Cs/UYuvsP3LaBOc8cofKGazuYCL58tqFh+GQREDl6ZOqL\n"\
"/iegQqrqgjOdeAb7iuGUDLNEvngPmpgdPQ+hGZLg2oqZ2OmrjTKCmweGz7Fg7Tp8\n"\
"gXdKrC/LDnsxWHGPdu6XuSpt0FQRwZWfdHtGUR/WuvHRpJYTNJeO1resmcJ2QRb9\n"\
"Eqt7w1d+/Gw04HzpI2Q8dAwLzEFll+lj8518wAZTKP88WXLR0vMiIDWI7RkBm9PX\n"\
"WgGe++APJ4BgpZOz1Mt2P1/wDkvVUOf2BnzfKPM3h9++LMVOJ9fDdWr+hyrNvmmi\n"\
"VcJ//gECgYEA9NOrkLtN2FVe3ZKQEbm22+R9DzUgWF7xBwB8r21th87n+OxWUpBk\n"\
"9LIQ8JXS2HTJgOgv4CSNN25KSsNIKx+cNs8pYkNIpZPTHs7bY/w6RPCoU6LChr9z\n"\
"ni+MGknsUkbZrwy+2sA7mXWOeofl7DJ226O4nsbDbGkxQwKNkqXrxUECgYEAx3KK\n"\
"y2/eM7GdHXlRMq6unaYmVPTF5eQA8LE4+SukTYeiheGiq4KcXHiO1ZwGvs08EJeJ\n"\
"cTyyrOeUfCzeF2tg9dsQ8HOXYWADXrFSKRO1q57n1qTsmipq36wd0Bauy4ZqKQUY\n"\
"QyD7gLTVhJ2x4/IVmese/HDCwlNqrJkFcMmj5ysCgYEA2mtNEJ5J4MqpC7wJ1AZ/\n"\
"YMYD3GFlLfHlzn1cv5U8toCc67LNgHNZ3gROoJTJulrWuq5qp+l0IxO+K9z0saRN\n"\
"1YgUP5oPYcjz/PRHPnhz87VQNu1p5pU/LMz2jS2OLAkRg943FymRAmOibSZ4tbR/\n"\
"3Yu+1cg16K4gSMwqSb7FDQECgYEAjWNCx0MPlz/TLIDDMhWpTbLqO5Gl9Pm0POy1\n"\
"gdYRTLDZYuV4hvM5W0hPoVhRY9NkQDLcyTyJQY+NsJWx/5Iskz1v/bVD3FDPMsKr\n"\
"htklEXu9yGj0HbnyN4vxOoHJ+hfYjCnDcFsiT7RiJPgRkpEcBRgryDM7rbyqw55a\n"\
"wAEBwy8CgYAhX9wpxlZaMgAqCWaT/MlSic1h6qAm5J0MrqIssssKPXf3K71rhxzA\n"\
"9zFp1Tigb6OfnXK6ICS59vhS9H7AJpei162EshKgM4wdbaO09Ba6t5Rz4wLre98y\n"\
"TMCLuyG3zqXCZrBkVsS6+wzPkiA7/2E+y0al7/b/0Q3FO9AcQv60lg==\n"\
"-----END RSA PRIVATE KEY-----\n"


#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
