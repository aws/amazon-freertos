## Running the demos
More details available on our getting started page (on development): https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started.html

### Selecting your demos
Select your demo by changing the aws_demo_config.h file. 
For example for ESP32 it is located here ./vendors/espressif/boards/esp32/aws_demos/common/config_files  
By default '#define CONFIG_MQTT_DEMO_ENABLED ' MQTT demos is selected. 

### Cmake
Refer to CMake readme page 
./tools/cmake

### Buiding instruction per boards
#### ESP32
Uses CMake  
make -j4  
Erase flash: ./vendors/espressif/esp-idf/tools/idf.py erase_flash  
Flash: make flash or ./vendors/espressif/esp-idf/tools/idf.py flash  
Monitor: ./vendors/espressif/esp-idf/tools/idf.py monitor -p /dev/ttyUSB1  

You can also combine commands like:  
 ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -p /dev/ttyUSB1  
 
 #### Nordic
 Does not use CMAKE. Project can be compiled and flashed directly after installing Segger Embedded Studio:  
 https://www.nordicsemi.com/?sc_itemid=%7B48E11346-206B-45FD-860D-637E4588990B%7D  
 Do flash the bootloader prior flashing the demos app  


