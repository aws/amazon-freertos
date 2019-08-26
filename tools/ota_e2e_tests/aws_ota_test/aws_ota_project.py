"""
Amazon FreeRTOS
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

http://aws.amazon.com/freertos
http://www.FreeRTOS.org

"""
import fileinput
import sys
import subprocess
import os
from time import sleep


class OtaAfrProject:
    """OtaAfrProject represents the Amazon FreeRTOS code base for OTA.
    This class is used to update the Amazon FreeRTOS project for OTA.
    Attributes:
        _buildConfig(dict): Build configuration from 'build_config' field in board.json.
        _projectRootDir(str): The root of the Amazon FreeRTOS project i.e. $AFR_ROOT/demos or $AFR_ROOT/tests.
        _boardProjectPath(str): the vendor/board/project specific path for building Amazon FreeRTOS.
        _buildProject(str): the name of the build project, which can be either 'demos' or 'tests'.
        _bootloaderSequenceNumber(int): the sequence number of bootloader.
    Methods:
        initializeOtaProject()
        buildProject()
        setClientCredentialForThingName(thingName)
        setClientCredentialKeys(certificate, privateKey)
        setApplicationVersion(major, minor, bugfix)
        setCodesignerCertificate(codesignerCertificatePath)
        __setDemoRunnerForOtaDemo()
        setClientCredentialsForWifi()
        setClientCredentialsForAwsIotEndpoint()
        __setIdentifierInFile(prefixToValue, filePath)
    Example:
        from aws_ota_aws_agent import AwsIoTThing
        otaProject = OtaAfrProject('C:/amazon-freertos', buildConfig)
        otaProject.initializeOtaProject()
        iotThing = AWSIoTThing('boardName')
        otaProject.setClientCredentialForThingName(iotThing.thing_name)
        otaProject.setClientCredentialKeys(iotThing.cert, iotThing.priv_key)
        otaProject.buildProject()
    """

    RUNNER_PATH = None
    CLIENT_CREDENTIAL_PATH = None
    APPLICATION_VERSION_PATH = None
    CLIENT_CREDENTIAL_KEYS_PATH = None
    OTA_CODESIGNER_CERTIFICATE_PATH = None
    OTA_UPDATE_DEMO_PATH = None
    OTA_BOOTLOADER_CONFIG_PATH = None
    OTA_BOOTLOADER_CERTIFICATE_PATH = None
    OTA_FACTORY_IMAGE_GENERATOR_PATH = None

    def __init__(self, boardConfig):
        self._buildConfig = boardConfig['build_config']
        self._projectRootDir = boardConfig['afr_root']
        self._buildProject = boardConfig['demos_or_tests']
        self._boardProjectPath = boardConfig['vendor_board_path'] + '/aws_' +  self._buildProject
        self._bootloaderSequenceNumber = 0

        OtaAfrProject.RUNNER_PATH = self._boardProjectPath + '/config_files/aws_demo_config.h'
        OtaAfrProject.CLIENT_CREDENTIAL_PATH = self._buildProject + '/include/aws_clientcredential.h'
        OtaAfrProject.APPLICATION_VERSION_PATH = self._buildProject + '/include/aws_application_version.h'
        OtaAfrProject.CLIENT_CREDENTIAL_KEYS_PATH = self._buildProject + '/include/aws_clientcredential_keys.h'
        OtaAfrProject.OTA_CODESIGNER_CERTIFICATE_PATH = 'demos/include/aws_ota_codesigner_certificate.h'
        if 'microchip' in self._boardProjectPath:
            OtaAfrProject.OTA_BOOTLOADER_CONFIG_PATH = boardConfig['vendor_board_path'] + '/bootloader/bootloader/utility/user-config/ota-descriptor.config'
            OtaAfrProject.OTA_BOOTLOADER_CERTIFICATE_PATH = boardConfig['vendor_board_path'] + '/bootloader/bootloader/utility/codesigner_cert_utility/aws_ota_codesigner_certificate.pem'
            OtaAfrProject.OTA_FACTORY_IMAGE_GENERATOR_PATH = boardConfig['vendor_board_path'] + '/bootloader/bootloader/utility/factory_image_generator.py'
        # OtaAfrProject.OTA_UPDATE_DEMO_PATH = 'demos/ota/aws_ota_update_demo.c' // TODO: need to figure out the changes for non prod version to work.

    def initializeOtaProject(self):
        """Initialize the Amazon FreeRTOS project for OTA.
        """
        if self._buildProject == 'demos':
            self.__setDemoRunnerForOtaDemo()
        elif self._buildProject == 'tests':
            self.__setTestRunnerForOtaDemo()
        else:
            raise Exception('ERROR: Invalid project root \"{}\". The valid values are \"demos\" and \"tests\".'.format(base))

    def generateFactoryImage(self):
        # If this board uses the Amazon FreeRTOS reference bootlaoder, then we want to
        # build and flash the factory image.
        if self._buildConfig.get('use_reference_bootloader', False) and OtaAfrProject.OTA_FACTORY_IMAGE_GENERATOR_PATH:
            factoryImageGenCommand = \
                'python ' + os.path.join(self._projectRootDir, OtaAfrProject.OTA_FACTORY_IMAGE_GENERATOR_PATH) + ' ' + \
                '-b ' + self._buildConfig['output'] + ' ' + \
                '-p ' + self._buildConfig['bootloader_hardware_platform'] + ' ' + \
                '-k ' + self._buildConfig['bootloader_private_key_path'] + ' ' + \
                '-x ' + self._buildConfig['bootloader_output']
            subprocess.call(
                factoryImageGenCommand,
                shell=True
            )


    def buildProject(self):
        """Build the Amazon FreeRTOS project represented by this object.
        """
        # Update the bootloader sequence number for every new build
        returnCodes = []
        self.__incrementBootloaderSequenceNumber()

        # Save the system path to restore.
        system_path = os.environ['PATH']
        # Add the tool_paths to the system PATH
        for path in self._buildConfig['tool_paths']:
            os.environ['PATH'] = path + os.pathsep + os.environ['PATH']

        buildCommands = self._buildConfig['commands']
        print('Building project {} ...'.format(self._buildConfig['project_dir']))
        for command in buildCommands:
            command = command.format(**self._buildConfig)
            print('====> Executing Command: ' + command)
            proc = subprocess.Popen(command + ' > build.log 2>&1', shell=True)
            proc.wait()
            print('====> Command run completed with the return code: ', proc.returncode)
            returnCodes.append(proc.returncode)

        sleep(5) # added the sleep just to give enough time for the binaries to be generated properly and having unecessary exception.
        output = self._buildConfig['output']
        if not os.path.exists(output):
            print("ERROR: Could not find the output binary, the build might have failed.")
            print('Searched for build output at: {} and the current working directory is: '.format(self._buildConfig['output']), os.getcwd())
            raise Exception("Error building project check build.log")
        print('Build finished, output: {}'.format(self._buildConfig['output']))

        # We generate the factory image if applicable. This may depend on some build tool paths.
        self.generateFactoryImage()

        # Restore the system path
        os.environ['PATH'] = system_path

        return returnCodes

    def __incrementBootloaderSequenceNumber(self):
        self._bootloaderSequenceNumber += 1
        if OtaAfrProject.OTA_BOOTLOADER_CONFIG_PATH:
            self.__setIdentifierInFile(
                {
                    'SEQUENCE_NUMBER': '= ' + str(self._bootloaderSequenceNumber)
                },
                os.path.join(self._projectRootDir, OtaAfrProject.OTA_BOOTLOADER_CONFIG_PATH)
            )

    def copyCodesignerCertificateToBootloader(self, certificate):
        """Copies the input certificate to a file named: aws_ota_codesigner_certificate.pem under
        demos/ota/bootloader/utility/codesigner_cert_utility.
        """
        if OtaAfrProject.OTA_BOOTLOADER_CERTIFICATE_PATH:
            with open(os.path.join(self._projectRootDir, OtaAfrProject.OTA_BOOTLOADER_CERTIFICATE_PATH), 'w') as f:
                f.write(certificate)

    def __setDemoRunnerForOtaDemo(self):
        """
        Updates the aws_demo_runner.c to disable the default MQTT Echo demo and enable the OTA
        update demo.
        """
        demoRunnerFilePath = os.path.join(self._projectRootDir, OtaAfrProject.RUNNER_PATH)
        print("demoRunnerFilePath: " + demoRunnerFilePath)

        startMQTTdemo = "#define CONFIG_MQTT_DEMO_ENABLED"
        startotaUpdateDemo = "#define CONFIG_OTA_UPDATE_DEMO_ENABLED"
        for line in fileinput.input(files=demoRunnerFilePath, inplace=True):
            if (startMQTTdemo in line) and ("//" not in line) and ("/*" not in line):
                line = line.replace(startMQTTdemo, startotaUpdateDemo)
            sys.stdout.write(line)

    def __setTestRunnerForOtaDemo(self):
        """
        Updates the aws_test_runner_config.h file to enable the OTA demo.
        """
        self.__setIdentifierInFile(
            {
                '#define testrunnerFULL_TCP_ENABLED': '0',
                '#define testrunnerOTA_END_TO_END_ENABLED': '1'
            },
            os.path.join(self._projectRootDir, OtaAfrProject.RUNNER_PATH)
        )


    def setClientCredentialsForWifi(self, ssid, password, security):
        """
        Updates the aws_clientcredential.h file for the wifi configurations defined in
        the board.json
        """
        self.__setIdentifierInFile(
            {
                '#define clientcredentialWIFI_SSID': '\"' + ssid + '\"',
                '#define clientcredentialWIFI_PASSWORD': '\"' + password + '\"',
                '#define clientcredentialWIFI_SECURITY': security
            },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_PATH)
        )

    def setClientCredentialsForAwsIotEndpoint(self, awsIotEndpoint, awsIotEndpointPort = None):
        """
        Updates aws_clientcredential.g for the AWS IoT endpoint defined in board.json
        args:
            awsIotEndpoint(str): Sets clientcredentialMQTT_BROKER_ENDPOINT[]
            awsIotEndpointPort(str): Optionally sets clientcredentialMQTT_BROKER_PORT, if specified.
        """
        self.__setIdentifierInFile(
            {
                '#define clientcredentialMQTT_BROKER_ENDPOINT': '\"' + awsIotEndpoint + '\"',
                '#define clientcredentialMQTT_BROKER_PORT' : awsIotEndpointPort
            },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_PATH)
        )

    def __setIdentifierInFile(self, prefixToValue, filePath):
        """
        Set the indentied value, from prefix the input prefixToValue map, in the file path
        specified.
        If the value in the prefixToValue dictionary is None then the prefix is ignored if encountered.
        Args:
            prefixToValue (dict[str:str]): Identifier to value.
        """
        for line in fileinput.input(files=filePath, inplace=True):
            if any(line.startswith(prefix) for prefix in prefixToValue.keys()):
                prefix = next(prefix for prefix in prefixToValue.keys() if line.startswith(prefix))
                if prefixToValue[prefix] != None:
                    line = '{} {}\n'.format(prefix, prefixToValue[prefix])
            # print(line) will place an extra newline character in the file.
            sys.stdout.write(line)

    def setMqttLogsOn(self):
        """Set the MQTT debug logs to on in aws_mqtt_config.h
        """
        self.__setIdentifierInFile(
            { '#define mqttconfigENABLE_DEBUG_LOGS': '1' },
            os.path.join(self._projectRootDir, self._boardProjectPath, 'config_files', 'aws_mqtt_config.h')
        )

    def setFreeRtosConfigNetworkInterface(self, networkInterface):
        """Set the configNETWORK_INTERFACE_TO_USE in FreeRTOSConfig.h to networkInterface.
        Args:
            networkInterface (int): The number of the network interface
        """
        self.__setIdentifierInFile(
            {
                '#define configNETWORK_INTERFACE_TO_USE': str(networkInterface)
            },
            os.path.join(self._projectRootDir, self._boardProjectPath, 'config_files', 'FreeRTOSConfig.h')
        )

    def setClientCredentialForThingName(self, thingName):
        """Set aws_clientcredential.h with the input thingName.
        """
        self.__setIdentifierInFile(
            { '#define clientcredentialIOT_THING_NAME': '\"' + thingName + '\"' },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_PATH)
        )

    def setClientCredentialKeys(self, certificate, privateKey):
        """Set the certificate and private key in aws_clientcredential_keys.h
        """
        self.__setIdentifierInFile(
            {
                '#define keyCLIENT_CERTIFICATE_PEM': '\"' + certificate.replace('\n', '\\n') + '\"',
                '#define keyCLIENT_PRIVATE_KEY_PEM': '\"' + privateKey.replace('\n', '\\n') + '\"'
            },
            os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_KEYS_PATH)
        )

    def setApplicationVersion(self, major, minor, bugfix):
        """Set aws_application_version.h with the input version.
        """
        self.__setIdentifierInFile(
            {
                '#define APP_VERSION_MAJOR': major,
                '#define APP_VERSION_MINOR': minor,
                '#define APP_VERSION_BUILD': bugfix
            },
            os.path.join(self._projectRootDir, OtaAfrProject.APPLICATION_VERSION_PATH)
        )

    def setCodesignerCertificate(self, certificate):
        """Set aws_ota_codesigner_certificate.h with the certificate specified.
        """
        codeSignerCertificatePath = os.path.join(self._projectRootDir, OtaAfrProject.OTA_CODESIGNER_CERTIFICATE_PATH)
        signerCertificateTag = 'static const char signingcredentialSIGNING_CERTIFICATE_PEM[] = '
        for line in fileinput.input(files=codeSignerCertificatePath, inplace=True):
            if (signerCertificateTag in line):
                line = '{} {}\n'.format(signerCertificateTag, '\"' + certificate.replace('\n', '\\n') + '\";')
            sys.stdout.write(line)

    def setOtaUpdateDemoForRootCA(self):
        """Sets the secure connection certificate in the MQTT connection parameters
        in the OTA update demo.
        """
        self.__setIdentifierInFile(
            {
                '            xConnectParams.pcCertificate =': '( char* ) clientcredentialROOT_CA_PEM;',
                '            xConnectParams.ulCertificateSize =': 'sizeof(clientcredentialROOT_CA_PEM)-1;'
            },
            os.path.join(self._projectRootDir, OtaAfrProject.OTA_UPDATE_DEMO_PATH)
        )

    def setOtaUpdateDemoForNullCertificate(self):
        """Sets the secure connection certificate in the MQTT connection parameters
        in the OTA update demo.
        """
        self.__setIdentifierInFile(
            {
                '            xConnectParams.pcCertificate =': 'NULL;',
                '            xConnectParams.ulCertificateSize =': '0;'
            },
            os.path.join(self._projectRootDir, OtaAfrProject.OTA_UPDATE_DEMO_PATH)
        )

    def setOtaDemoRunnerForSNIDisabled(self):
        """Disabled SNI by setting mqttagentURL_IS_IP_ADDRESS in the connection parameters.
        """
        self.__setIdentifierInFile(
            {
                '            xConnectParams.xFlags =': 'mqttagentREQUIRE_TLS | mqttagentURL_IS_IP_ADDRESS;'
            },
            os.path.join(self._projectRootDir, OtaAfrProject.OTA_UPDATE_DEMO_PATH)
        )

    def addRootCAToClientCredentialKeys(self, certificate):
        """Adds the Beta endpoint's root Certificate Authority to aws_clientcredential_keys.h.
        The root CA was retrieved with openssl s_client -showcerts -connect iotmoonraker.us-east-1.beta.funkypineapple.io:8883
        """
        clientCertificateKeysPath = os.path.join(self._projectRootDir, OtaAfrProject.CLIENT_CREDENTIAL_KEYS_PATH)
        rootCAPemTag = 'static const char clientcredentialROOT_CA_PEM[] ='
        found = False
        for line in fileinput.input(files=clientCertificateKeysPath, inplace=True):
            if (rootCAPemTag in line):
                 line = '{} {}\n'.format(rootCAPemTag, ' \"' + certificate + '\";\n')
                 found = True
            sys.stdout.write(line)

        if not found:
            with open(clientCertificateKeysPath, 'a') as f:
                f.write('\nstatic const char clientcredentialROOT_CA_PEM[] = \"' + certificate + '\";\n')
