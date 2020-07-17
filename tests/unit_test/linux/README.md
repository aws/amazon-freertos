# Unit Testing with CMOCK setting up guide

## Introduction to Unit Testing with AFR
Unit Testing with AFR uses the **cmake build** framework.<br>
Code coverage is achieved with **gcov/lcov** and results are displayed in: *build/coverage/*<br>
The executables are located in *build/bin* <br>

The build is tested on Linux and Windows with WSL. <br>


## Introduction to CMOCK
**CMOCK** is a mocking  framework, that automatically generates mocks.
It gives the ability to set expectations and implement stubs/callbacks.
This will allow you to call the functions to be tested and the **CMOCK** generated code will do all the rest for you.<br>

For example suppose your module accesses the internet using sockets, but you
want to test just your module (unit) without testing/calling any socket API
function as this could be unpredictable, slow, hard to simulate all possible
error scenarios and return codes.<br>
In order to achieve that you tell **CMOCK** to mock the header file where the
socket's APIs reside. For simplicity we will assume the only function used is
*func*, **CMOCK** will generate the following mocks for you:

```
func_Ignore[AndReturn]([return_val]);
func_ExpectAnyArgs[AndReturn]([return_val]);
func_Expect[AndReturn](param1, param2,..., paramN ,[return_val])
func_StubWithCallback(function_callback)   (*this is deprecated*)
func_AddCallback
func_Stub
func_ExpectWithArray[AndReturn](param1, [depth1], param2, [depth2], ..., paramN, [depthN], [return_val])
func_ReturnThruPtr_[parameter name](ret_pointer)
func_ReturnArrayThruPtr_parameeter(parameter, len)
func_ReturnMemThruPtr_[parameter name](size, ret_memory)
func_IgnoreArg_[parameter name]()
```
For a detailed explanation about these mocks check the official documentation:<br>
The **CMOCK** official website click [here] (http://www.throwtheswitch.org/cmock)<br>
The **CMOCK** git hub repository click [here] (https://github.com/ThrowTheSwitch/CMock)<br>

## How to build and execute
To build first go to the AFR root directory
```
$ cd afr_root
$ cmake -B build -DAFR_ENABLE_UNIT_TESTS=on -DBOARD=linux -DVENDOR=pc -DCOMPILER=linux-gcc -DCMAKE_BUILD_TYPE=Debug
$ cd build
$ make
$ make coverage
```
## Setting up a new Unit Testing module in AFR
To Setup a module for Unit Testing, as an example we will follow a walkthrough
approach for **lwip_secure_sockets** which are located in *libraries/abstractions/secure_sockets*. <br>

1. Open the file *tests/unit_test/linux/CMakeLists.txt* for editing.<br>
    Make sure at around line 15 the the Unit Test directory location exists in this case **../../../libraries** if not add it<br>

``` cmake
   add_subdirectory(../../../libraries libraries)
```
This will make the build system include your files in the build process

2.  Add an additional line in the CMakeLists.txt that is located in the directory mentioned above *libraries*
```cmake
if (AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(folder_name/project_name)
    return()
endif()
```
3. In the directory of the module to be tested create a new directory called *utest*

4. For simplicity, copy the CMakeLists.txt file from *libraries/abstractions/secure_sockets/utest/* into the new Unit Test Directory *utest*

5. Create a test soure file that ends with  *_utest.c* it is where your test code will live<br>
    you will end up with 2 files in the utest directory (project_name_utest.c and CMakeLists.txt)

6. Edit the copied file (CMakeLists.txt) in the following way
    1. Modify the *project_name* at the top to match the new one to be tested
    2. Replace all headers in *mock_list* with the ones you the source file
       under test uses.
       ``` cmake
        list(APPEND mock_list
                    "lwip/src/include/lwip/sockets.h"
                    "my_file_to_mock.h"
                )
       ```
    3. Replace all the include directories in *mock_include_list* with the
       directories your mocks are using
       ```cmake
        list(APPEND mock_include_list
                    /my/file/dir/needs
                )
        ```
    4. Replace the definitions in *mock_define_list* list with the definitions you see fit for your
       source files
       ``` cmake
       list(APPEND mock_define_list
                    -DMY_DEFINE=1
                )
       ```
       This will create header files in the following form:
       *mock_my_file.h* which you will have to include from
       your test code (project_name_utest.c)
       This compilation step would create a shared object with the name *project_name_mock.so*

    5. Replace all the files in *real_source_files* with the source files you will
       be testing. This will create a library with the name *project_file_real.a*
       ``` cmake
       list(APPEND real_source_files
                    code_to_test.c
                )
       ```
    6. Replace all the directories in *real_include_directories*  with the
       directories the source under test will include headers from
       ```cmake
       list(APPEND real_include_directories
                   /my/under/test/source/needs
               )
       ```
       This compilation step would create an object library called *project_name_real.a*
    7. Replace all directories in *test_include_directories* with what the test
       code needs (project_name_utest.c)
       ```cmake
           list(APPEND test_include_directories
                    /my/test/code/needs
                )
        ```

    8. And thats it for this file..

    9. At the end, this makefile will create the mocked header and source files,
       *project_name_mock.so*, *project_name_real.a*, and *project_name_utest*

7. Write your Unit Test code *project_name_utest.c* it is better if you copy a
   file from another place (for example, *secure_sockets*) to re-use its skeleton<br>
    A Few points to keep in mind:
    * There is no need to write a main function, it is automatically generated
    * Testing functions must start with *test_*
    * We use unity to test and assert results
8.  You should be ready to go by now<br>
    Always remember, if it is not tested it doesn't work :( ... Happy Testing!





