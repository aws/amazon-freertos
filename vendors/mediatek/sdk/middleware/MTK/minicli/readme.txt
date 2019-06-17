MINICLI module usage guide

Brief:          This module is a Command Line Interface engine implementation.
Usage:          GCC:
                    . Include this module by adding the following line to your GCC project Makefile:
                      include $(SOURCE_DIR)/middleware/MTK/minicli/module.mk
                    . Set MTK_MINICLI_ENABLE to "y" in <project filder>/GCC/feature.mk.
                
                KEIL: 
                    . Drag the middleware/MTK/minicli folder to your project and define the MTK_MINICLI_ENABLE macro. 
                    . Add middleware/MTK/minicli/inc to INCLUDE_PATH.
                    . Add io_def.c to your project
                    . Add kernel\service\libkservice_CM4_MT7687_Keil.lib to your project
                    . Add driver\board\mt76x7_hdk\util\inc to INCLUDE_PATH
                    . Add driver\board\mt76x7_hdk\wifi\inc to INCLUDE_PATH
                IAR:
                    . Drag the middleware/MTK/minicli folder to your project and define the MTK_MINICLI_ENABLE macro. 
                    . Add middleware/MTK/minicli/inc to INCLUDE_PATH.
                    . Add io_def.c to your project
                    . Add kernel\service\libkservice_CM4_MT7687_IAR.lib to your project
                    . Add driver\board\mt76x7_hdk\util\inc to Project -> Options -> C/C++ Compiler -> Preprocessor -> Additional include directories
                    . Add driver\board\mt76x7_hdk\wifi\inc to Project -> Options -> C/C++ Compiler -> Preprocessor -> Additional include directories
Dependency:     HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
Notice:         N/A
Relative doc:   N/A
Example project:Please find the project under project folder with iot_sdk prefix. 

