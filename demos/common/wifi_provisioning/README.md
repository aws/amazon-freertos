## WiFi provisioning
WiFi provisioning is a BLE service that allows to safely send credential from phone to device over Bluetooth low energy. 
The source code for the service can be found in the Amazon FreeRTOS repo under folder ```lib/bluetooth_low_energy/services/wifi_provisioning```. 

This directory is a placeholder, WiFi provisioning really take place in the main loop:

```
void vApplicationDaemonTaskStartupHook( void )
{

	if( prvBLEInit() != eBTStatusSuccess )
	{
		configPRINTF((" Failed to Initialize BLE\n" ));
		while( 1 )
		{
		}
	}

	if( SYSTEM_Init() == pdPASS )
	{

		/* Connect to the BT before running the demos */
		prvBLEConnect();

		WIFI_PROVISION_Start();

		/* Connect to the wifi before running the demos */
		prvWifiConnect();

		/* Run all demos. */
		DEMO_RUNNER_RunDemos();
	}
}
```

WiFi provisioning is executed in the main loop to support executing tests on WiFi. A customer application could start WiFi provisioning at a different place or time.

## Provisioning sequence

When WiFi provisioning service starts for the first time, it waits for user to provision a new WIFI network.

Through the custom mobile application developed using the [IOS sdk] (https://github.com/aws/amazon-freertos-ble-ios-sdk), user can choose to perform following operations:
- List all saved WiFi networks in the flash and scan new WiFi networks
- Securely Provsion a new WiFi network using the credentials input from user
- Reorder the priority of saved networks in any order
- Delete saved networks from flash
- Switch connections between the saved WiFi networks

Once the device restarts it automatically connects to one of the provisioned WiFi networks in the order it was saved. Maximum number of saved networks at any time in flash 
is currently set to 8

MQTT demos like the hello world demo are designed to use the WIFI network as the preferred transport by default. If during WiFi provisioning we delete the WiFi network or
the network connectivity is lost, it falls back to BLE as the transport mechanism with the help of MQTT proxy on the companion IOS SDK.



