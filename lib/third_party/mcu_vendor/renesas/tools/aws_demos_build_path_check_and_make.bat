@echo off
rem ###############################################################################################
rem # This batch file executes some preprocess for build and then executes the make if necessary
rem ###############################################################################################

setlocal

set verbose=0

if exist ".\aws_demos_build_path_check.bat" (
    set ide=csp
    set pj=.\
) else if exist "..\aws_demos_build_path_check_and_make.bat" (
    set ide=e2
    set pj=..\
) else (
    echo\
    echo Error: Unable to find the project folder.
    goto ERROR
)
if exist "%pj%src\smc_gen\r_config\" (
    set gen=smc
) else if exist "%pj%r_config\" (
    set gen=fit
) else (
    echo\
    echo Error: Unable to find the r_config folder.
    goto ERROR
)
if %verbose%==1 echo IDE is %ide%
if %verbose%==1 echo Project Folder is %pj%
if %verbose%==1 echo Project Type is %gen%

if %ide%==e2 if "%1"=="clean" goto CLEAN
if %ide%==e2 if "%2"=="clean" goto CLEAN
if %ide%==e2 if "%3"=="clean" goto CLEAN
if %ide%==e2 if "%4"=="clean" goto CLEAN
if %ide%==e2 if "%5"=="clean" goto CLEAN
if %ide%==e2 if "%6"=="clean" goto CLEAN
if %ide%==e2 if "%7"=="clean" goto CLEAN
if %ide%==e2 if "%8"=="clean" goto CLEAN
if %ide%==e2 if "%9"=="clean" goto CLEAN

if %ide%==csp (
    set qu=
) else if %ide%==e2 (
    set qu='
)
echo\
echo %qu%Checking source code tree...%qu%

if %ide%==csp (
    set FN=%CD%\..\..\..\..\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.c
) else if %ide%==e2 (
    set FN=%CD%\.\lib\aws\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj
)
if %verbose%==1 echo Maximum Path is "%FN%"
set FX=%FN:~259%
if %verbose%==1 echo Over of Path is "%FX%"

:DEPTH_CHECK_STEP
set ef=0
if not "%FX%"=="" (
    set ef=1
    echo Error: The following deep folder path will fail the build process.
    echo %CD%
)
if %ef%==1 goto ERROR

if %gen%==fit goto FIT

:SMC
if %verbose%==1 echo Do pre-build operation for Smart Configurator generated code

rem If you don't want to do pre-build operation, you can skip it by adding one of following files.
rem     src/NO_USE_FIT_modified_code
rem     src/NO_USE_FIT_pre_generated_code
rem if exist "%pj%src\NO_USE_FIT_modified_code" goto MAKE
rem if exist "%pj%src\NO_USE_FIT_pre_generated_code" goto MAKE

set MODIFIED_FIT_MODULES=r_bsp r_ether_rx r_flash_rx r_sci_rx r_byteq r_riic_rx r_sci_iic_rx r_cmt_rx r_s12ad_rx

rem goto MOVE_STEP_1 && rem # For debug
rem goto MOVE_STEP_2 && rem # For debug
rem goto MOVE_STEP_3 && rem # For debug
rem goto MOVE_STEP_4 && rem # For debug

set ef=0
if not exist "%pj%src\smc_gen\UNUSED_generated_code" (
    echo making folder: "src\smc_gen\UNUSED_generated_code"
    mkdir "%pj%src\smc_gen\UNUSED_generated_code" > nul
    if not exist "%pj%src\smc_gen\UNUSED_generated_code" (
        rem FAILED
        set ef=1
    )
)
if %ef%==1 goto ERROR

:MOVE_STEP_1
set ef=0
for %%S in (%MODIFIED_FIT_MODULES%) do (
    if exist "%pj%src\smc_gen\%%S\readme.txt" if exist "%pj%src\smc_gen\UNUSED_generated_code\%%S" (
        rem ERROR
        set ef=1
        echo Error: Unable to move "src\smc_gen\%%S" --^> "src\smc_gen\UNUSED_generated_code\%%S" because the folder already exists.
    )
)
if %ef%==1 goto ERROR

:MOVE_STEP_2
set ef=0
for %%S in (%MODIFIED_FIT_MODULES%) do (
    if exist "%pj%src\smc_gen\%%S\readme.txt" (
        rem MOVE_FOLDER
        echo moving folder: "src\smc_gen\%%S" --^> "src\smc_gen\UNUSED_generated_code\%%S"
        move "%pj%src\smc_gen\%%S" "%pj%src\smc_gen\UNUSED_generated_code" > nul
        if exist "%pj%src\smc_gen\%%S" (
            rem FAILED
            set ef=1
        )
    )
)
if %ef%==1 goto ERROR

:MOVE_STEP_3
set ef=0
for %%S in (%MODIFIED_FIT_MODULES%) do (
    if not exist "%pj%src\smc_gen\%%S" if exist "%pj%src\smc_gen\UNUSED_generated_code\%%S" if %ide%==e2 (
        rem MAKE_EMPTY_FOLDER
        if %%S==r_bsp (
            mkdir "%pj%src\smc_gen\%%S"                  > nul 2>&1
        ) else if %%S==r_ether_rx (
            mkdir "%pj%src\smc_gen\%%S"                  > nul 2>&1
        ) else if %%S==r_flash_rx (
            mkdir "%pj%src\smc_gen\%%S"                  > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src"              > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src\flash_type_1" > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src\flash_type_2" > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src\flash_type_3" > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src\flash_type_4" > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src\targets"      > nul 2>&1
        ) else (
            mkdir "%pj%src\smc_gen\%%S"                  > nul 2>&1
            mkdir "%pj%src\smc_gen\%%S\src"              > nul 2>&1
        )
    )
)

:MOVE_STEP_4
for %%S in (%MODIFIED_FIT_MODULES%) do (
    if exist "%pj%src\smc_gen\UNUSED_generated_code\%%S\readme.txt" (
        rem SUCCEEDED
        echo Use "src\FIT_modified_code\%%S" module instead of generated %%S
    )
)

:CONVERT_FOR_GNURX
if exist "%pj%src\smc_gen\general\r_cg_vector_table.c" (
    if not exist "%pj%src\smc_gen\general\r_cg_vector_pragma.h" (
        echo converting file: "src\smc_gen\general\r_cg_vector_table.c" --^> "src\smc_gen\general\r_cg_vector_pragma.h"
        cscript -nologo "%~dp0r_cg_vector_pragma.js" "%pj%src\smc_gen\general\r_cg_vector_table.c" "%pj%src\smc_gen\general\r_cg_vector_pragma.h"
    )
    if exist "%pj%src\smc_gen\general\r_cg_vector_pragma.h" (
        echo Use "src\smc_gen\general\r_cg_vector_pragma.h" instead of r_cg_vector_table.c
    )
)

goto MAKE

:FIT
if %verbose%==1 echo Do pre-build operation for FIT Configurator generated/based code

set MODIFIED_FIT_MODULES=r_bsp r_ether_rx r_flash_rx r_byteq r_sci_rx r_riic_rx r_sci_iic_rx r_cmt_rx r_s12ad_rx

:SHOW_MESSAGE
set ef=0
for %%S in (%MODIFIED_FIT_MODULES%) do (
    if exist "%pj%%%S" (
        if exist "%pj%%%S\*.h" (
            echo Use "src\FIT_modified_code\%%S" module instead of generated %%S
        )
    )
)

:MAKE
if %ide%==csp (
    endlocal
    exit 0
) else if %ide%==e2 (
    echo\
    echo %qu%Invoking make...%qu%
    echo %qu% %qu%
:CLEAN
    endlocal
    make -r %*
    exit %ERRORLEVEL%
)
exit 2

:ERROR
endlocal
exit 2
