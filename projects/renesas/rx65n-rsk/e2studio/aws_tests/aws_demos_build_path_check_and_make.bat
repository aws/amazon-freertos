@echo off
rem ################################################################################
rem # This batch file executes some preprocess for build and then executes the make
rem ################################################################################

if not exist "%~dp0..\..\..\..\..\vendors\renesas\tools\aws_demos_build_path_check_and_make.bat" (
    echo ERROR: Unable to find "%~dp0..\..\..\..\..\vendors\renesas\tools\aws_demos_build_path_check_and_make.bat"
    exit 2
)

"%~dp0..\..\..\..\..\vendors\renesas\tools\aws_demos_build_path_check_and_make.bat" %*
