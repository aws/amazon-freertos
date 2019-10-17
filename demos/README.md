Amazon FreeRTOS can be built utilizing a supported IDE toolchain or with cmake.  For additional instructions for each individual board, refer to the top level README for the Getting Started Guides: https://github.com/aws/amazon-freertos/blob/master/README.md


### Selecting a Demo

There are a variety of demos available for each user to explore the functionality available within Amazon FreeRTOS.  To select which demo to enable, please follow the steps below:

1. Navigate to  ```./vendors/<VENDOR_NAME>/boards/<BOARD>/aws_demos/common/config_files```  
2. Open ```aws_demo_config.h``` file. 
3. The defines are used to select which demo will be enabled from the list of available demos
4. By default, ```#define CONFIG_MQTT_DEMO_ENABLED``` MQTT demos is selected. 
5. Replace the ```#define``` with your demo of choice if necessary
6. Compile your project utilizing IDE or with cmake

Select your demo by changing the ```aws_demo_config.h``` file. 


### amazon-freeRTOS/projects

The ```./projects``` folder contains the IDE test and demo projects for each vendor and their boards.  The majority of boards can be built with both IDE and cmake (there are some exceptions!).  Please refer to the Getting Started Guides referred to in the README for board specific instructions.  
