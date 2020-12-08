/*******************************************************************************
* \file cy_em_eeprom.h
* \version 2.0
*
* \brief
* This file provides the function prototypes and constants for the
* Emulated EEPROM middleware library.
*
********************************************************************************
* \copyright
* Copyright 2017-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/**
********************************************************************************
* \mainpage Cypress Em_EEPROM Middleware Library
*
* The Emulated EEPROM (Em_EEPROM) middleware emulates an EEPROM storage in
* PSoC's internal flash memory. The Em_EEPROM middleware operates on the top
* of the flash driver included in the PSoC 6 Peripheral Driver Library
* (psoc6pdl).
*
* Use the Em_EEPROM to store non-volatile data on a target device when
* increasing flash memory endurance and restoring corrupted data from
* a redundant copy is required.
*
* <b>Features:</b>
* * EEPROM-Like Non-Volatile Storage
* * Easy to use Read and Write
* * Optional Wear Leveling
* * Optional Redundant Data Storage
*
********************************************************************************
* \section section_em_eeprom_general_description General Description
********************************************************************************
*
* Include cy_em_eeprom.h to get access to all functions and other declarations
* in this library. See the \ref section_em_eeprom_quick_start to start using
* the Em_EEPROM.
*
* Refer to the \ref section_em_eeprom_toolchain section for compatibility
* information.
*
* Refer to the \ref section_em_eeprom_changelog section for differences
* between Em_EEPROM versions. The \ref section_em_eeprom_changelog section
* also describes the impact of the changes to your code.
*
* Em_EEPROM operates on the top of the flash driver. The flash driver has
* some prerequisites for proper operation.
* Refer to the "Flash (Flash System Routine)" section of the PSoC 6
* Peripheral Driver Library (psoc6pdl) API Reference Manual.
* Also, refer to the \ref section_em_eeprom_miscellaneous section for
* the different Em_EEPROM middleware restrictions and limitations.
*
* The Em_EEPROM middleware can operate in various modes:
* * with or without wear leveling - depending on whether you want to increase
*   the endurance of the flash memory.
* * with or without a redundant copy - depending on how critical for you is
*   the ability to recover information.
* * to save flash and work via Em_EEPROM APIs similar to the flash driver APIs.
*   But, in this case, recovering your data and monitoring the flash endurance
*   is impossible.
*
* There are several use cases depending on where you store your Em_EEPROM data:
* * in the application flash
* * in the auxiliary flash
* * in the application flash at a fixed address.
*
* The \ref section_em_eeprom_configuration_considerations section provides
* the guidance for all these operation modes and use cases.
* You may also want to migrate from PSoC Creator to ModusToolbox
* or other environment to simply use the Em_EEPROM middleware APIs.
* Refer to the \ref section_em_eeprom_migration section.
*
* The \ref section_em_eeprom_quick_start section highlights the use case,
* when the Em_EEPROM data is located in the application flash, and the
* Em_EEPROM is configured to increase the flash endurance
* (the wearLevelingFactor parameter is turned on).
*
********************************************************************************
* \section section_em_eeprom_quick_start Quick Start Guide
********************************************************************************
*
* Cypress Em_EEPROM middleware can be used in various Development
* Environments such as ModusToolbox, MBED, etc. Refer to the
* \ref section_em_eeprom_toolchain section.
*
* The below steps describe the simplest way of enabling the Em_EEPROM
* middleware with placing EEPROM memory into the application flash.
*
* 1. Open/Create an application where to add the Em_EEPROM function.
*
* 2. Add the Em_EEPROM middleware to your project.
*    This quick start guide assumes that the environment is configured
*    to use the PSoC 6 Peripheral Driver Library (psoc6pdl) for development
*    and the PSoC 6 Peripheral Driver Library (psoc6pdl) is included in
*    the project. If you are using the ModusToolbox development environment
*    select the application in the Project Explorer window and navigate to
*    the Project/ModusToolbox Middleware Selector menu. A window appears,
*    check the Emulated EEPROM middleware and click the OK button.
*
* 3. Include Em_EEPROM in the main.c file:
*    \snippet em_eeprom/sut_001/main.c snippet_required_includes
*
* 4. Define the Em_EEPROM configuration as follow:
*    \snippet em_eeprom/sut_001/main.c snippet_configuration_data
*    \snippet em_eeprom/sut_001/main.c snippet_configuration_simple
*    \snippet em_eeprom/sut_001/main.c snippet_configuration_wear
*    \snippet em_eeprom/sut_001/main.c snippet_configuration_redundant
*    \snippet em_eeprom/sut_001/main.c snippet_configuration_blocking
*    Refer to the \ref cy_stc_eeprom_config_t structure for details of
*    other configuration options.
*
* 5. Declare the Em_EEPROM storage variable (further Em_EEPROM Storage)
*    in the application flash:
*    \snippet em_eeprom/sut_001/main.c snippet_application_storage_placing
*    The allocated memory must be initialized by zeros, and aligned to the
*    whole row size \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW, otherwise
*    the Em_EEPROM middleware behavior will be unexpected.<br>
*    Refer to the \ref section_em_eeprom_configuration_considerations section
*    for other options of Em_EEPROM flash allocation.
*
* 6. Allocate memory for the Em_EEPROM context structure:
*    \snippet em_eeprom/sut_001/main.c snippet_eeprom_context_declaration
*
* 7. Allocate memory for the Em_EEPROM configuration structure and
*    initialize it:
*    \snippet em_eeprom/sut_001/main.c snippet_eeprom_config_declaration
*
* 8. Initialize the Em_EEPROM middleware once at the start:
*    \snippet em_eeprom/sut_001/main.c snippet_eeprom_start
*
* 9. Now, the Em_EEPROM middleware is ready to use. Call the Write or Read
*    functions to write or read one byte:
*    \snippet em_eeprom/sut_001/main.c snippet_eeprom_read_write
*
********************************************************************************
* \section section_em_eeprom_configuration_considerations Configuration Considerations
********************************************************************************
*
* This section consists of different guides and instruction of how to enable,
* configure, and use the Emulated EEPROM Middleware in a design.
* As you can see from the \ref section_em_eeprom_quick_start section,
* the settings of the Em_EEPROM middleware are controlled with
* the \ref cy_stc_eeprom_config_t structure. Please see its description
* to learn about the parameters and values.
*
* Now we will describe the most common use cases along with the
* configuration structure examples and code snippets.
* The list of sections under Configuration Considerations:
*
* * \ref section_em_eeprom_operating_modes
*   * \ref section_em_eeprom_mode_wearleveling
*   * \ref section_em_eeprom_mode_redundantcopy
*   * \ref section_em_eeprom_mode_simple
* * \ref section_em_eeprom_location
*   * \ref section_em_eeprom_appsflash_location
*   * \ref section_em_eeprom_auxflash_location
*   * \ref section_em_eeprom_appsflash_fixed
* * \ref section_em_eeprom_migration
*
* Also refer to the \ref section_em_eeprom_miscellaneous for the
* existing restrictions.
*
********************************************************************************
* \subsection section_em_eeprom_operating_modes Operating Modes
********************************************************************************
*
* The settings of the Em_EEPROM middleware are controlled by
* the \ref cy_stc_eeprom_config_t structure. See its description
* to learn about the parameters and values.
*
********************************************************************************
* \subsubsection section_em_eeprom_mode_wearleveling Wear Leveling
********************************************************************************
*
* Depending on whether you want to increase the flash memory endurance
* or not, enable or disable the wear leveling.
* The higher the value is, the more flash is used, but a higher number of
* erase/write cycles can be done on Em_EEPROM.
* Multiply this number by the datasheet write endurance spec to determine
* the max of write cycles.<br>
* The amount of wear leveling from 1 to 10. 1 means no wear leveling is used.
*
* To configure the wear leveling just set the WEAR_LEVELING macro value from
* (1u) to (10u) in step #4 in the \ref section_em_eeprom_quick_start section:
* \snippet em_eeprom/sut_001/main.c snippet_configuration_wear
*
********************************************************************************
* \subsubsection section_em_eeprom_mode_redundantcopy Redundant Copy
********************************************************************************
*
* Depending on how critical for you is recovering information,
* configure the redundant copy feature.
* If enabled (1 - enabled, 0 - disabled), a checksum
* (stored in a row) is calculated on each row of data,
* while a redundant copy of Em_EEPROM is stored in another location.
* When data is read, first the checksum is checked. If that checksum is bad,
* and the redundant copy's checksum is good, the copy is restored.
*
* To configure the redundant copy just set the REDUNDANT_COPY macro value to
* (1u) or (0u) in step #4 in the \ref section_em_eeprom_quick_start section:
* \snippet em_eeprom/sut_001/main.c snippet_configuration_redundant
*
********************************************************************************
* \subsubsection section_em_eeprom_mode_simple Simple Mode
********************************************************************************
*
* Simple mode, when enabled (1 - enabled, 0 - disabled), means no
* additional service information is stored by the Em_EEPROM middleware
* like checksums, headers, a number of writes, etc.
* Data is stored directly by the specified address.
* The size of Em_EEPROM storage is equal to the number of
* byte specified in the eepromSize parameter rounded up to a full row
* size \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW. The wear leveling and
* redundant copy features are disabled, i.e. wearLevelingFactor and
* redundantCopy parameters are ignored.
*
* To configure Simple mode just set the SIMPLE_MODE macro value
* to (1u) or (0u) in step #4 in the \ref section_em_eeprom_quick_start section:
* \snippet em_eeprom/sut_001/main.c snippet_configuration_simple
*
********************************************************************************
* \subsection section_em_eeprom_location Em_EEPROM Location
********************************************************************************
*
* The user is responsible for allocating space in flash for Em_EEPROM
* (further the Em_EEPROM storage).
*
* The Em_EEPROM storage can be placed:
* * in the application flash
* * in the auxiliary flash.
*
* Additionally, the storage can be placed at a fixed address in the
* application flash.
*
* The storage location must be aligned to \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW.
*
* The storage size depends on other configuration parameters and is calculated
* using the following equation:
*
* 1. Simple mode is turned on. It means the direct mapping of the user data
*    in the Em_EEPROM storage:
*
*    <i>storageSize = eepromSize</i>
*
*    where:<br>
*    <i>eepromSize</i> the number of bytes to store in the Em_EEPROM storage
*    rounded up to a full row size \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW.
*    The row size is specific for a device family. Refer to the specific
*    PSoC device datasheet.
*
* 2. Simple mode is turned off. It means the Em_EEPROM middleware stores
*    service information about number of writes, checksums, etc.
*
*    <i>storageSize = eepromSize * 2 * wearLevelingFactor * (1 + redundantCopy)</i>
*
*    where:<br>
*    <i>eepromSize</i> the number of bytes to store in the Em_EEPROM storage
*    rounded up to the half of a row size (\ref CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u).
*    The row size is specific for a device family. Refer to the specific
*    PSoC device datasheet.
*
* Use the \ref CY_EM_EEPROM_GET_PHYSICAL_SIZE() macro to get the needed
* storage size depending on the configuration.
*
********************************************************************************
* \subsubsection section_em_eeprom_appsflash_location Em_EEPROM Location in the application flash
********************************************************************************
*
* The below example shows placing the Em_EEPROM storage in the application
* flash for GCC, ARMCC, and IAR compilers:
*
*    \code
*      CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
*      const uint8_t emEepromStorage[STORAGE_SIZE] = {0u};
*    \endcode
*
* where STORAGE_SIZE is the size of the Em_EEPROM storage. Refer to the
* \ref section_em_eeprom_location section for size calculation equations.
* For convenience, use the \ref CY_EM_EEPROM_GET_PHYSICAL_SIZE macro to
* get the needed Em_EEPROM storage size depending on the configuration.
*
********************************************************************************
* \subsubsection section_em_eeprom_auxflash_location Em_EEPROM Location in the auxiliary flash
********************************************************************************
*
* Writes to rows affect the endurance of other rows in the same sector.
* We recommend using the auxiliary flash for frequently-updated data.
* The below example shows placing the Em_EEPROM storage in the
* auxiliary flash (section .cy_em_eeprom) for GCC, ARMCC, and IAR compilers.
*
*    \code
*      CY_SECTION(".cy_em_eeprom")
*      CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
*      const uint8_t emEepromStorage[STORAGE_SIZE] = {0u};
*    \endcode
*
* where STORAGE_SIZE is the size of the storage. Refer to the
* \ref section_em_eeprom_location section for size calculation equations.
* For convenience, use the \ref CY_EM_EEPROM_GET_PHYSICAL_SIZE macro to
* get the needed Em_EEPROM storage size depending on the configuration.
*
********************************************************************************
* \subsection section_em_eeprom_appsflash_fixed Em_EEPROM Location in the application flash at a fixed address
********************************************************************************
*
* To allocate the Em_EEPROM storage at a fixed address in
* the application flash, modify the linker control file (linker script).
* This requires fundamental knowledge of the linker
* control file, because there is a risk of receiving a linker error while
* building the project if you make some improper modifications.
*
* This approach demonstrates adding the storage reservation in the application
* flash after the application. You must calculate the application end
* address and select the address of the Em_EEPROM storage so that the
* memory spaces of the storage and the application do not overlap.
* You might also add some offset between the application end address
* and the Em_EEPROM storage start address to ensure
* there is extra space in case the project code grows.
*
********************************************************************************
* \subsubsection section_em_eeprom_fixed_address_gcc Em_EEPROM Storage at a Fixed Address for GCC Compiler
********************************************************************************
*
* 1. Build the project to generate linker scripts.
*
* 2. Open the linker script "cy8c6xx7_cm4_dual.ld" for the CM4 core
*    and search the following declaration:
*    \code
*      etext =  . ;
*    \endcode
*    Paste the following code right after the declaration:
*    \code
*      EM_EEPROM_START_ADDRESS = <EEPROM Storage Address>;
*      .my_emulated_eeprom EM_EEPROM_START_ADDRESS :
*      {
*         KEEP(*(.my_emulated_eeprom))
*      } > flash
*    \endcode
*    where:
*   * EEPROM Storage Address is an absolute address in flash where
*     the Em_EEPROM operates. You must define the address value.
*     Ensure the address is aligned to the size of the device's flash row
*     and does not overlap with the memory space used by the application.
*
*   * my_emulated_eeprom is the name of the section
*     where the Em_EEPROM storage will be placed. The name can be changed to
*     any name you choose.
*
* 3. Save the changes and close the file.
*
* 4. Declare the Em_EEPROM storage in the newly created section. To do this,
*    declare an array in flash, aligned to the size of the flash row of
*    the device you are using. An example of such array declaration
*    is the following:
*    \code
*      CY_SECTION(".my_emulated_eeprom")
*      CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
*      const uint8 emEepromStorage[STORAGE_SIZE];
*    \endcode
*
* 5. After the Em_EEPROM storage is defined, pass the address to
*    the middleware:
*    \snippet em_eeprom/sut_001/main.c snippet_eeprom_pass_address
*
* 6. Build the project to verify the correctness of the linker control file
*    modifications.
*
********************************************************************************
* \subsection section_em_eeprom_miscellaneous Limitations and Restrictions
********************************************************************************
*
* * The Em_EEPROM storage location must be initialized with zeros and
*   aligned to the flash row size referred to in the specific PSoC device
*   datasheet otherwise the Em_EEPROM behavior may be unexpected.
*   For convenience, \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW is provided.
*
* * The Em_EEPROM storage size depends on the configuration. Refer to the
*   \ref section_em_eeprom_location section for size calculation equations.
*   For convenience, the \ref CY_EM_EEPROM_GET_PHYSICAL_SIZE macro is provided.
*
* * Do not modify the Em_EEPROM context structure \ref cy_stc_eeprom_context_t
*   since it may cause unexpected behavior of the Cy_Em_EEPROM functions
*   that rely on this context structure.
*
* * The Internal memory address map, flash organization, size of rows, etc.
*   is specific for each device family. Refer to the specific device datasheet
*   for the details.
*
* * The Read-While-Write (RWW) feature available in PSoC 6 MCU allows you
*   to write to flash while executing the code from flash. There are restrictions
*   on using this feature for EEPROM emulation. There are also multiple
*   constraints for blocking and nonblocking flash operations, relating to
*   interrupts, Power mode, IPC usage, etc.
*   Refer to the "Flash (Flash System Routine)" section of the PSoC 6
*   Peripheral Driver Library (psoc6pdl) API Reference Manual.<br>
*
* * Manage auxiliary flash space for both cores of PSoC 6. For PSoC 6,
*   by default, the complier always assigns both cores with full range
*   of auxiliary flash (0x14000000-0x14008000) for EM_EEPROM. Both
*   cores operate on the same flash object. A building error would occur
*   if there is an out-sync operation on the memory range from any single
*   core. If more than one driver and/or middleware occupying the auxiliary
*   flash is simultaneously under used, for example, Bluetooth Low Energy
*   (BLE) and Em_EEPROM, a building error will occur while generating the
*   elf file. The error occurs because there is an auxiliary flash region
*   allocated for BLE to store the Bonding list, and this region will only
*   be allocated to the core where the BLE host lies. This out-sync between
*   the two cores causes the building failure. For details of how to manage
*   the auxiliary flash for both cores properly refer to
*   the <a href="https://community.cypress.com/docs/DOC-15264">
*   <b>Manage Flash Space for Both Cores of PSoC 6 – KBA224173</b></a>
*
* * Writing of multiple rows by single the Cy_Em_EEPROM_Write() function
*   may lead to the following behavior:
*   The first row is written, then the device is reset due to power down or other
*   reasons, then the device is powered up again. This leads to data integrity
*   loss: i.e. the first row contains new data while the rest of the rows contain
*   old data and Em_EEPROM will not be able to detect the issue since
*   the row checksum is valid.
*
********************************************************************************
* \subsection section_em_eeprom_migration Migration from PSoC Creator
********************************************************************************
*
* This section helps migrate your project from PSoC Creator with the
* Em_EEPROM component to ModusToolbox or other software environment using
* the Em_EEPROM middleware.
*
* The migration consists of three steps:
* * \ref section_em_eeprom_migration_location
* * \ref section_em_eeprom_migration_configuration
* * \ref section_em_eeprom_migration_function
*
********************************************************************************
* \subsubsection section_em_eeprom_migration_location Migration of Em_EEPROM Location
********************************************************************************
*
* The PSoC Creator Em_EEPROM component has parameter "Use Emulated EEPROM". It
* defines where the Em_EEPROM storage is located.
* * "Use Emulated EEPROM" = No <br>
*   The Em_EEPROM storage is defined by the application program. Then
*   move the Em_EEPROM storage declaration from the PSoC Creator project
*   into the ModusToolbox project. Refer to \ref section_em_eeprom_location
*   to check for other possible options.
* * "Use Emulated EEPROM" = Yes <br>
*   The Em_EEPROM component provides Em_EEPROM storage located in the
*   auxiliary flash. The Em_EEPROM middleware requires the storage
*   to be provided by the application program.
*   Therefore, place the below code into your application program.
*     \code
*       CY_SECTION(".cy_em_eeprom")
*       CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
*       const uint8_t emEepromStorage[STORAGE_SIZE] = {0u};
*     \endcode
*   where STORAGE_SIZE is the size of the storage that can be seen in
*   the Em_EEPROM component customizer as "Actual EEPROM size (bytes)".<br>
*   For convenience, use the \ref CY_EM_EEPROM_GET_PHYSICAL_SIZE macro to
*   get the needed Em_EEPROM storage size depending on the configuration.
*
********************************************************************************
* \subsubsection section_em_eeprom_migration_configuration Migration of Configuration
********************************************************************************
*
* Allocate memory for Em_EEPROM context and configuration structures, and
* initialize the configuration structure per the Em_EEPROM component
* configuration:
*
*    \code
*      cy_stc_eeprom_context_t eepromContext;
*      cy_stc_eeprom_config_t eepromConfig =
*      {
*          .eepromSize = <EEPROM Size>,
*          .simpleMode = 0u,
*          .wearLevelingFactor = <Wear Level Factor>,
*          .redundantCopy = <Redundant Copy>,
*          .blockingWrite = <Use Blocking Write>,
*          .userFlashStartAddr = (uint32_t)&(emEepromStorage[0u]),
*      };
*    \endcode
* where the right side of initialization is the Em_EEPROM Component customizer
* parameters and "emEepromStorage" is the name of the storage.
*
********************************************************************************
* \subsubsection section_em_eeprom_migration_function Migration of Function
********************************************************************************
*
* Now, after the storage and configuration are defined, change the names of
* the functions used in the PSoC Creator project per the following table:
*
* <table class="doxtable">
*   <tr>
*     <th>PSoC Creator Em_EEPROM Component</th>
*     <th>ModusToolbox Em_EEPROM Middleware</th>
*   </tr>
*   <tr>
*     <td>EEPROM_Init(xxx)</td>
*     <td>Cy_Em_EEPROM_Init(&eepromConfig, &eepromContext)</td>
*   </tr>
*   <tr>
*     <td>EEPROM_Write(X1, X2, X3)</td>
*     <td>Cy_Em_EEPROM_Write(X1, X2, X3, &eepromContext)</td>
*   </tr>
*   <tr>
*     <td>EEPROM_Read(X1, X2, X3)</td>
*     <td>Cy_Em_EEPROM_Read(X1, X2, X3, &eepromContext)</td>
*   </tr>
*   <tr>
*     <td>EEPROM_Erase()</td>
*     <td>Cy_Em_EEPROM_Erase(&eepromContext)</td>
*   </tr>
*   <tr>
*     <td>EEPROM_NumWrites()</td>
*     <td>Cy_Em_EEPROM_NumWrites(&eepromContext)</td>
*   </tr>
* </table>
*
* <b> Note </b> The above table shows the function names with an assumption
* that the PSoC Creator component name is EEPROM.
*
********************************************************************************
* \section section_em_eeprom_toolchain Supported Software and Tools
********************************************************************************
*
* This version of the Em_EEPROM Middleware was validated for the compatibility
* with the following software and tools:
*
* <table class="doxtable">
*   <tr>
*     <th>Software and Tools</th>
*     <th>Version</th>
*   </tr>
*   <tr>
*     <td>ModusToolbox Software Environment</td>
*     <td>2.0</td>
*   </tr>
*   <tr>
*     <td>PSoC6 Peripheral Driver Library (PDL)</td>
*     <td>1.2.0</td>
*   </tr>
*   <tr>
*     <td>GCC Compiler</td>
*     <td>7.2.1</td>
*   </tr>
*   <tr>
*     <td>IAR Compiler</td>
*     <td>8.32</td>
*   </tr>
*   <tr>
*     <td>Arm Compiler 6</td>
*     <td>6.11</td>
*   </tr>
*   <tr>
*     <td>MBED OS</td>
*     <td>5.13.1</td>
*   </tr>
*   <tr>
*     <td>FreeRTOS</td>
*     <td>10.0.1</td>
*   </tr>
* </table>
*
********************************************************************************
* \section section_em_eeprom_MISRA MISRA-C Compliance
********************************************************************************
*
* The Cy_Em_EEPROM library's specific deviations:
*
* <table class="doxtable">
*   <tr>
*     <th>MISRA Rule</th>
*     <th>Rule Class (Required/Advisory)</th>
*     <th>Rule Description</th>
*     <th>Description of Deviation(s)</th>
*   </tr>
*   <tr>
*     <td>3.1</td>
*     <td>R</td>
*     <td rowspan="2">Cast between a pointer to object and an integral type.</td>
*     <td rowspan="2">The cast is used intentionally for the performance
*         reason.</td>
*   </tr>
*   <tr>
*     <td>11.3</td>
*     <td>A</td>
*   </tr>
*   <tr>
*     <td>17.4</td>
*     <td>R</td>
*     <td>Performing pointer arithmetic</td>
*     <td>The pointer arithmetic is used intentionally for the
*         performance reason.</td>
*   </tr>
* </table>
*
********************************************************************************
* \section section_em_eeprom_changelog Changelog
********************************************************************************
*
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td rowspan="10">2.0</td>
*     <td colspan="2">The Em_EEPROM 2.0 is not backward compatible with
*         the previous version. It was significantly rewritten with changing
*         the behavior of operation, adding many improvements and fixing
*         defects.<br>
*         However, the application programming interface (API) contains
*         only single change and you can seamlessly migrate to 2.0 version.
*         This change is consist in adding the \ref section_em_eeprom_mode_simple.
*         </td>
*   </tr>
*   <tr>
*     <td>Updated major and minor version defines</td>
*     <td>Follow naming convention</td>
*   </tr>
*   <tr>
*     <td>Updated documentation</td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td>Changed the \ref CY_EM_EEPROM_EEPROM_DATA_LEN macro by
*         adding the simpleMode parameter</td>
*     <td>Added new mode when wear leveling and redundant copy features
*         are disabled</td>
*   </tr>
*   <tr>
*     <td>Fixed MISRA violations</td>
*     <td>Improved the middleware robustness</td>
*   </tr>
*   <tr>
*     <td>Fixed the defect of the Cy_Em_EEPROM_Read() function when Emulated
*         EEPROM data corruption in some cases caused infinite loop</td>
*     <td>Fixed Defect</td>
*   </tr>
*   <tr>
*     <td>Fixed the defect of the Cy_Em_EEPROM_Read() function when the
*         function returns incorrect data after restoring data from the
*         redundant copy</td>
*     <td>Fixed Defect</td>
*   </tr>
*   <tr>
*     <td>Added the mechanism to restore the corrupted redundant copy from the main data copy</td>
*     <td>Improved the Em_EEPROM data reliability</td>
*   </tr>
*   <tr>
*     <td>Revised the operation of Cy_Em_EEPROM_Read() and Cy_Em_EEPROM_Init()
*         functions by removing the write operation.</td>
*     <td>Improved the Em_EEPROM functionality</td>
*   </tr>
*   <tr>
*     <td>Expanded the checksum verification to the entire row.</td>
*     <td>Improved the Em_EEPROM data reliability</td>
*   </tr>
*   <tr>
*     <td>1.10</td>
*     <td>Flattened the organization of the driver source code into a single
*         source directory and a single include directory </td>
*     <td>Simplified the Driver library directory-structure</td>
*   </tr>
*   <tr>
*     <td>1.0.1</td>
*     <td>Added the Em_EEPROM storage allocation note to the
*         \ref section_em_eeprom_configuration_considerations</td>
*     <td>Documentation update and clarification</td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial Version</td>
*     <td></td>
*   </tr>
* </table>
*
********************************************************************************
* \section section_em_eeprom_more_information More Information
********************************************************************************
*
* For more information, refer to the following documents:
*
* * <a href="https://www.cypress.com/products/modustoolbox-software-environment">
*      <b>ModusToolbox Software Environment, Quick Start Guide, Documentation,
*         and Videos</b>
*   </a>
*
* * <a href="https://www.cypress.com/an219434">
*      <b>AN219434 Importing PSoC Creator Code into an IDE for a PSoC 6
*         Project</b>
*   </a>
*
* * <a href="http://www.cypress.com/an210781">
*      <b>AN210781 Getting Started with PSoC 6 MCU with Bluetooth Low
*         Energy (BLE) Connectivity</b>
*   </a>
*
* * <a href="https://github.com/cypresssemiconductorco.github.io/psoc6pdl/pdl_api_reference_manual/html/index.html">
*   <b>PDL API Reference</b></a>
*
* * <a href="https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference">
*      <b>PSoC 6 Technical Reference Manual</b>
*   </a>
*
* * <a href="http://www.cypress.com/ds218787">
*      <b>PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet</b>
*   </a>
*
* * <a href="http://www.cypress.com">
*      <b>Cypress Semiconductor</b>
*   </a>
*
* \note
* The links to the other software component’s documentation (middleware and PDL)
* point to GitHub to the latest available version of the software.
* To get documentation of the specified version, download from GitHub and unzip
* the component archive. The documentation is available in
* the <i>docs</i> folder.
*
********************************************************************************
*
* \defgroup group_em_eeprom_macros Macros
* \brief
* This section describes the Emulated EEPROM Macros.
*
* \defgroup group_em_eeprom_functions Functions
* \brief
* This section describes the Emulated EEPROM Function Prototypes.
*
* \defgroup group_em_eeprom_data_structures Data Structures
* \brief
* This section describes the data structures defined by the Emulated EEPROM.
*
* \defgroup group_em_eeprom_enums Enumerated Types
* \brief
* This section describes the enumeration types defined by the Emulated EEPROM.
*
*/


#if !defined(CY_EM_EEPROM_H)
#define CY_EM_EEPROM_H

#include <stddef.h>
#include "cy_device_headers.h"
#include "cy_syslib.h"
#include "cy_flash.h"

/* The C binding of definitions if building with the C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* API Constants
*******************************************************************************/
/**
* \addtogroup group_em_eeprom_macros
* \{
*/
/** Library major version */
#define CY_EM_EEPROM_MW_VERSION_MAJOR       (2)

/** Library minor version */
#define CY_EM_EEPROM_MW_VERSION_MINOR       (0)

/** Em_EEPROM PDL ID */
#define CY_EM_EEPROM_ID                     (CY_PDL_DRV_ID(0x1BuL))

/** A prefix for Em_EEPROM function error return-values */
#define CY_EM_EEPROM_ID_ERROR               ((uint32_t)(CY_EM_EEPROM_ID | CY_PDL_STATUS_ERROR))

/** Defines the size of a flash row */
#define CY_EM_EEPROM_FLASH_SIZEOF_ROW       (CY_FLASH_SIZEOF_ROW)

/** Defines the maximum data length that can be stored in one flash row */
#define CY_EM_EEPROM_EEPROM_DATA_LEN(simpleMode) \
                (CY_EM_EEPROM_FLASH_SIZEOF_ROW / (2uL - (simpleMode)))

/** The number of flash rows required to create an Em_EEPROM of dataSize */
#define CY_EM_EEPROM_GET_NUM_ROWS_IN_EEPROM(dataSize, simpleMode) \
                ((((dataSize) - 1uL) / (CY_EM_EEPROM_EEPROM_DATA_LEN(simpleMode))) + 1uL)

/** Defines the maximum number of byte that can be stored in the Em_EEPROM storage */
#define CY_EM_EEPROM_GET_NUM_DATA(dataSize, simpleMode) \
                (CY_EM_EEPROM_GET_NUM_ROWS_IN_EEPROM(dataSize, simpleMode) * \
                CY_EM_EEPROM_FLASH_SIZEOF_ROW)

/** Returns the size of flash allocated for Em_EEPROM including wear leveling and a redundant copy overhead */
#define CY_EM_EEPROM_GET_PHYSICAL_SIZE(dataSize, simpleMode, wearLeveling, redundantCopy) \
                (CY_EM_EEPROM_GET_NUM_DATA(dataSize, simpleMode) * \
                ((((1uL - (simpleMode)) * (wearLeveling)) * ((redundantCopy) + 1uL)) + (simpleMode)))

/** \} group_em_eeprom_macros */


/*******************************************************************************
* Data Structure definitions
*******************************************************************************/
/**
* \addtogroup group_em_eeprom_data_structures
* \{
*/

/** Em_EEPROM configuration structure */
typedef struct
{
    /**
    * The size of data in bytes to store in the Em_EEPROM. The size is rounded
    * up to a full Em_EEPROM row size. The row size is specific for a device
    * family. Refer to the specific PSoC device datasheet.<br>
    * Note this size is often smaller than the total amount of flash used
    * for the Em_EEPROM storage. The Em_EEPROM storage size depends on
    * the Em_EEPROM configuration and can be bigger because increasing
    * flash endurance (wear-leveling) and restoring corrupted data from
    * a redundant copy. Refer to the \ref section_em_eeprom_location section
    * for size calculation equations. For convenience, use
    * the \ref CY_EM_EEPROM_GET_PHYSICAL_SIZE macro to get the needed
    * storage size depending on the configuration.
    */
    uint32_t eepromSize;

    /**
    * Simple mode, when enabled (1 - enabled, 0 - disabled), means no
    * additional service information is stored by the Em_EEPROM middleware
    * like checksums, headers, a number of writes, etc.
    * Data is stored directly by the specified address.
    * The size of Em_EEPROM storage is equal to the number of
    * byte specified in the eepromSize parameter rounded up to a full row
    * size \ref CY_EM_EEPROM_FLASH_SIZEOF_ROW. The wear leveling and
    * redundant copy features are disabled, i.e. wearLevelingFactor and
    * redundantCopy parameters are ignored.
    */
    uint32_t simpleMode;

    /**
    * The higher the factor is, the more flash is used,
    * but a higher number of erase/write cycles can be done on Em_EEPROM.
    * Multiply this number by the datasheet write endurance spec to determine
    * the max of write cycles.<br>
    * The amount of wear leveling from 1 to 10. 1 means no wear leveling is used.
    */
    uint32_t wearLevelingFactor;

    /**
    * If enabled (1 - enabled, 0 - disabled), a checksum
    * (stored in a row) is calculated on each row of data,
    * while a redundant copy of Em_EEPROM is stored in another location.
    * When data is read, first the checksum is checked. If that checksum is bad,
    * and the redundant copy's checksum is good, the copy is restored.
    */
    uint8_t redundantCopy;

    /**
    * If enabled (1 - enabled, 0 - disabled), the blocking writes
    * to flash are used in the design. Otherwise, non-blocking flash writes
    * are used. From the user's perspective, the behavior of blocking and
    * non-blocking writes are the same - the difference is that the
    * non-blocking writes do not block the interrupts.
    */
    uint8_t blockingWrite;

    /**
    * The address of the flash storage.
    * The storage start address is filled to the Emulated EEPROM
    * configuration structure and then the structure is passed
    * to the Cy_Em_EEPROM_Init() function.
    */
    uint32_t userFlashStartAddr;
} cy_stc_eeprom_config_t;


/**
* The Em_EEPROM context data structure. It is used to store specific
* Em_EEPROM context data.
* Do not modify the context structure since it may cause unexpected behavior
* of the Cy_Em_EEPROM functions that rely on this context structure.
*/
typedef struct
{
    /** The number of flash rows allocated for the Em_EEPROM excluding the number
    * of rows allocated for wear-leveling and a redundant copy overhead.
    */
    uint32_t numberOfRows;

    /** The number of bytes to store in Em_EEPROM. */
    uint32_t eepromSize;

    /** The amount of wear leveling from 1 to 10. 1 means no wear leveling is used. */
    uint32_t wearLevelingFactor;

    /** The start address for the Em_EEPROM memory in flash. */
    uint32_t userFlashStartAddr;

    /** The number of user's data bytes in one row. */
    uint32_t byteInRow;

    /** No headers configured */
    uint32_t simpleMode;

    /** The pointer to the last written row. */
    uint32_t * ptrLastWrittenRow;

    /** If not zero, a redundant copy of the Em_EEPROM is included. */
    uint8_t redundantCopy;

    /** If not zero, a blocking write to flash is used. Otherwise,
    * a non-blocking write is used. This parameter is used only for PSoC 6.
    */
    uint8_t blockingWrite;

} cy_stc_eeprom_context_t;

/** \} group_em_eeprom_data_structures */

/*******************************************************************************
* Enumerated Types and Parameters
*******************************************************************************/
/**
* \addtogroup group_em_eeprom_enums
* \{
* Specifies the return values meaning.
*/

/** The Em_EEPROM return enumeration type. */
typedef enum
{
    /** The operation executed successfully */
    CY_EM_EEPROM_SUCCESS      = 0x00uL,
    /** The function input parameter is invalid */
    CY_EM_EEPROM_BAD_PARAM    = (CY_EM_EEPROM_ID_ERROR + 1uL),
    /** The data in Em_EEPROM is corrupted */
    CY_EM_EEPROM_BAD_CHECKSUM = (CY_EM_EEPROM_ID_ERROR + 2uL),
    /** The provided configuration is invalid or placing Em_EEPROM in flash failed */
    CY_EM_EEPROM_BAD_DATA     = (CY_EM_EEPROM_ID_ERROR + 3uL),
    /** Write to Em_EEPROM failed */
    CY_EM_EEPROM_WRITE_FAIL   = (CY_EM_EEPROM_ID_ERROR + 4uL),
    /** When redundant copy is enabled and one of the copies has bad CRC and other one valid CRC */
    CY_EM_EEPROM_REDUNDANT_COPY_USED    = (CY_EM_EEPROM_ID_ERROR + 5uL),
} cy_en_em_eeprom_status_t;

/** \} group_em_eeprom_enums */


/*******************************************************************************
*        Function Prototypes
*******************************************************************************/
/**
* \addtogroup group_em_eeprom_functions
* \{
*/
cy_en_em_eeprom_status_t Cy_Em_EEPROM_Init(
                const cy_stc_eeprom_config_t * config,
                cy_stc_eeprom_context_t * context);

cy_en_em_eeprom_status_t Cy_Em_EEPROM_Read(
                uint32_t addr,
                void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context);

cy_en_em_eeprom_status_t Cy_Em_EEPROM_Write(
                uint32_t addr,
                const void * eepromData,
                uint32_t size,
                cy_stc_eeprom_context_t * context);

cy_en_em_eeprom_status_t Cy_Em_EEPROM_Erase(
                cy_stc_eeprom_context_t * context);

uint32_t Cy_Em_EEPROM_NumWrites(
                cy_stc_eeprom_context_t * context);

/** \} group_em_eeprom_functions */


/*******************************************************************************
* Internal Macro Definitions
*******************************************************************************/

/* Device-specific flash constants */
#define CY_EM_EEPROM_FLASH_BASE_ADDR                (CY_FLASH_BASE)
#define CY_EM_EEPROM_FLASH_SIZE                     (CY_FLASH_SIZE)
#define CY_EM_EEPROM_EM_EEPROM_BASE_ADDR            (CY_EM_EEPROM_BASE)
#define CY_EM_EEPROM_EM_EEPROM_SIZE                 (CY_EM_EEPROM_SIZE)
#define CY_EM_EEPROM_EM_EEPROM_END_ADDR             (CY_EM_EEPROM_EM_EEPROM_BASE_ADDR + CY_EM_EEPROM_EM_EEPROM_SIZE)

#define CY_EM_EEPROM_FLASH_END_ADDR                 (CY_EM_EEPROM_FLASH_BASE_ADDR + CY_EM_EEPROM_FLASH_SIZE)

/* Checks whether Em_EEPROM is in the flash range. */
#define CY_EM_EEPROM_IS_IN_FLASH_RANGE(startAddr, endAddr) \
                    ((((startAddr) > CY_EM_EEPROM_FLASH_BASE_ADDR) && ((endAddr) <= CY_EM_EEPROM_FLASH_END_ADDR)) || \
                     (((startAddr) >= CY_EM_EEPROM_EM_EEPROM_BASE_ADDR) && ((endAddr) <= CY_EM_EEPROM_EM_EEPROM_END_ADDR)))

/* Defines the length of data that can be stored in the Em_EEPROM header. */
#define CY_EM_EEPROM_HEADER_DATA_LEN                ((CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u) - 16u)

/*
* Checks whether the Em_EEPROM address locations from startAddr1 to endAddr1
* are crossed with the Em_EEPROM address locations from startAddr2 to endAddr2.
*/
#define CY_EM_EEPROM_IS_ADDRESES_CROSSING(startAddr1, endAddr1 , startAddr2, endAddr2) \
                                    (((startAddr1) > (startAddr2)) ? (((startAddr1) >= (endAddr2)) ? (0u) : (1u) ) : \
                                    (((startAddr2) >= (endAddr1)) ? (0u) : (1u)))
#define CY_EM_EEPROM_IS_ADDRESES_CROSSING(startAddr1, endAddr1 , startAddr2, endAddr2) \
                                    (((startAddr1) > (startAddr2)) ? (((startAddr1) >= (endAddr2)) ? (0u) : (1u) ) : \
                                    (((startAddr2) >= (endAddr1)) ? (0u) : (1u)))


/* Checks whether the given address belongs to the Em_EEPROM address of the row
* specified by rowNum.
*/
#define CY_EM_EEPROM_IS_ADDR_IN_ROW_RANGE(addr, rowNum) \
                                    (((addr) < ((rowNum) * (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u))) ? (0u) : \
                                    (((addr) > ((((rowNum) + 1u) * (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u)) - 1u)) ? \
                                    (0u) : (1u)))

/* CRC-8 constants */
#define CY_EM_EEPROM_CRC8_POLYNOM                   ((uint8_t)(0x31u))
#define CY_EM_EEPROM_CRC8_POLYNOM_LEN               (8u)
#define CY_EM_EEPROM_CRC8_SEED                      (0xFFu)
#define CY_EM_EEPROM_CRC8_XOR_VAL                   ((uint8_t) (0x80u))

#define CY_EM_EEPROM_CALCULATE_CRC8(crc)            \
                  ((CY_EM_EEPROM_CRC8_XOR_VAL == ((crc) & CY_EM_EEPROM_CRC8_XOR_VAL)) ? \
                  ((uint8_t)(((uint8_t)((uint8_t)((crc) << 1u))) ^ CY_EM_EEPROM_CRC8_POLYNOM)) : ((uint8_t)((crc) << 1u)))

/* Offsets for the 32-bit RAM buffer addressing */
#define CY_EM_EEPROM_HISTORIC_DATA_OFFSET_U32       ((CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u) / 4u)
#define CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET_U32     (0u)
#define CY_EM_EEPROM_HEADER_SEQ_NUM_OFFSET_U32      (1u)
#define CY_EM_EEPROM_HEADER_ADDR_OFFSET_U32         (2u)
#define CY_EM_EEPROM_HEADER_LEN_OFFSET_U32          (3u)
#define CY_EM_EEPROM_HEADER_DATA_OFFSET_U32         (4u)
#define CY_EM_EEPROM_FLASH_SIZEOF_ROW_U32           (CY_FLASH_SIZEOF_ROW / 4u)

/* The same offsets as above used for direct memory addressing. */
#define CY_EM_EEPROM_HISTORIC_DATA_OFFSET           (CY_EM_EEPROM_FLASH_SIZEOF_ROW / 2u)
#define CY_EM_EEPROM_HEADER_CHECKSUM_OFFSET         (0u)
#define CY_EM_EEPROM_HEADER_ADDR_OFFSET             (8u)
#define CY_EM_EEPROM_HEADER_LEN_OFFSET              (12u)
#define CY_EM_EEPROM_HEADER_DATA_OFFSET             (16u)

#define CY_EM_EEPROM_U32                            (4u)

/* The maximum wear-leveling value. */
#define CY_EM_EEPROM_MAX_WEAR_LEVELING_FACTOR       (10u)

/* The maximum allowed flash row Write/Erase operation duration. */
#define CY_EM_EEPROM_MAX_WRITE_DURATION_MS          (50u)

/*
* These defines are obsolete and kept for the backward compatibility only.
* They will be removed in future versions.
*/
#define CY_EM_EEPROM_VERSION_MAJOR                  (CY_EM_EEPROM_MW_VERSION_MAJOR)
#define CY_EM_EEPROM_VERSION_MINOR                  (CY_EM_EEPROM_MW_VERSION_MINOR)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CY_EM_EEPROM_H */


/* [] END OF FILE */
