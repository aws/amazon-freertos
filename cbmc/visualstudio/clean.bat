@echo off

if exist local.bat call local.bat

if not defined CBMCDIR set CBMCDIR=%UserProfile%\cbmc
if not defined RTOSDIR set RTOSDIR=%~dp0%\..\..
if not defined PROJECT set PROJECT=demos\pc\windows\visual_studio
if not defined PROJDIR set PROJDIR=%RTOSDIR%\%PROJECT%

if exist %CBMCDIR% (
  echo CBMC binaries in %CBMCDIR%
) else (
  echo Can't find CBMC binaries in %CBMCDIR%
  exit /b
)
if exist %RTOSDIR% (
  echo FreeRTOS binaries in %RTOSDIR%
) else (
  echo Can't find FreeRTOS binaries in %RTOSDIR%
  exit /b
)
if exist %PROJDIR% (
  echo FreeRTOS project in %PROJDIR%
) else (
  echo Can't find FreeRTOS project in %PROJDIR%
  exit /b
)

set CWD=%cd%

cd /d %PROJDIR%
msbuild /target:clean
rmdir /s /q %PROJDIR%\Debug
cd /d %CWD%
del aws_demos.goto

set CWD=
set CBMCDIR=
set RTOSDIR=
set PROJECT=
set PROJDIR=
