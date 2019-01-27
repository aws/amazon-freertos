#if 0
Windows.h is included by much of the FreeRTOS code, so we need to have this file
around when we build harnesses. Having an empty file is fine, as the harnesses
don't use anything from Windows.h.
#endif
