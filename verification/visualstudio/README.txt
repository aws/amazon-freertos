This directory contains two scripts to build and clean FreeRTOS for CBMC.

The scripts depend on variables that can be set or set in local.bat:

CBMCDIR directory containing the goto-cl and goto-link binaries
RTOSDIR directory containing the root of the FreeRTOS repository
PROJECT path from RTOSDIR to directory containing the *.vcxproj project files
