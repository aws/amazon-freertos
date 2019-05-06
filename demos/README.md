## Running the demos
More details available on our getting started page (on development): https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started.html

### Selecting your demos
Select your demo by changing the ```aws_demo_config.h``` file. 

For example for ESP32 it is located here ```./vendors/espressif/boards/esp32/aws_demos/common/config_files```  

By default ```#define CONFIG_MQTT_DEMO_ENABLED``` MQTT demos is selected. 

### Cmake
Refer to CMake readme page in folder

```./tools/cmake```
Note: The cmake commands provided in example are made to be run from the root directory or with the following command:  
``` -C <your-build-directory>``` 

### Buiding instruction per boards
#### ESP32 

First use _cmake_ to generate the project files for Espressif, which uses _make_ to build: 

CMake command for building Espressif demos:
(from the root directory ofyour clone)

```cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S. -B<you build directory>```

if you want to build tests instead:

```cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S. -B<you build directory> -DAFR_ENABLE_TESTS=1```

if you want to build the debug flavor of the image:

 ```cmake -DVENDOR=espressif -DBOARD=esp32_wrover_kit -DCOMPILER=xtensa-esp32 -S. -B<you build directory> -DCMAKE_BUILD_TYPE=Debug```

 
then use make to build, e.g.: 

```make -j4```

You can flash your board and see the logs with the following commands:

**Erase flash**: ```./vendors/espressif/esp-idf/tools/idf.py erase_flash -b <your-build-directory> ```

**Flash binary to the board**: ```make flash or ./vendors/espressif/esp-idf/tools/idf.py flash -b <your-build-directory> ```

**Monitor**: ```./vendors/espressif/esp-idf/tools/idf.py monitor -p /dev/ttyUSB1 -b <your-build-directory> ```

You can also combine commands. e.g.:  

```./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -p /dev/ttyUSB1 -b <your-build-directory> ```
 
 #### Nordic
 Does not use CMake. Project can be compiled and flashed directly after installing Segger Embedded Studio:  
 https://www.nordicsemi.com/?sc_itemid=%7B48E11346-206B-45FD-860D-637E4588990B%7D.
 Do flash the bootloader prior flashing the demos app.


