## Build Enterprise Security application

This application shows a simple WiFi Enterprise Security connection.

### Make root directory

```
mkdir <dev>
cd <dev>
```

### Clone AFR

```
cd <dev>
git clone --recurse-submodules git@git-ore.aus.cypress.com:repo/amazon-freertos.git
```

### Build and flash application image
#### Build makefiles using CMake, then build and flash application

NOTE: CMake only needs to be run once per application. After that, just use "make" for rebuilding.

1. Generate makefiles using CMake
```
cd <dev>/amazon-freertos
cmake -DBLE_SUPPORTED=1 -DVENDOR=cypress -DCOMPILER=arm-gcc -DBOARD=CY8CPROTO_062_4343W -DAPP=vendors/cypress/apps/internal/test/enterprise-security -S . -B build/ -G Ninja
```

2. Build the application
```
cmake --build build
```

3. The build produces these files in the output directory

```
<dev>/amazon-freertos/build/enterprise-security
<appname>.elf			- ELF file
<appname>.hex			- Intel hex file
```

4. Program Device

Use CyProgrammer to program the ELF file.

#### Build and flash application via Modus Toolbox IDE

1. copy enterprise-security folder from vendors/cypress to projects/cypress/<TARGET_BOARD>/mtb/enterprise-security/
```
cd <dev>
cp -r vendors/cypress/apps/enterprise-securit projects/cypress/<TARGET_BOARD>/mtb/enterprise-security/
```

2. Open ModusToolbox IDE 

3. Import enterprise-security prject into MTB IDE
   File->Import->Existing Projects into Workspace->select root directory->select enterprise-security project->finish

4. Trigger image build and flash the image by selecting "aws_demos Program" in Launches
