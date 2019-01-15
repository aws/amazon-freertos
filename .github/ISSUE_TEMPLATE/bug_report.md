---
name: Bug report
about: Create a bug report for the project Amazon FreeRTOS
labels: bug
---

**Describe the bug**
A clear and concise description of what the bug is.

**System Information**
- Hardware board used (Please provide the name of the exact model you are using)
- IDE used
- Operating System [Windows|Linux|MacOS]
- Version of Amazon FreeRTOS (run ``git describe --tags`` to find it)
- Demo ran [Custom User Application|MQTT|Shadow|Greengrass|OTA|Device Defender|TCP Echo|Sub pub Demo|Simple TCP Server]
- If it is an Custom User Application, add the code of the application in the section `Code to reproduce the bug`. 

**Expected behavior**
A clear and concise description of what you expected to happen.

**Screenshots or console output**
If you are using an IDE, please provide the screenshot of the call stack
captured in the Debug View.
If appropriate, please cut-and-paste the console output to explain the issue.

**To reproduce**
Steps to reproduce the behavior

1. step1
2. step2 ...


**Code to reproduce the bug**
The code should be wrapped in the cpp tag for it to be displayed properly.
e.g. 
```cpp
printf("Hello World")
```

If the code is longer than 30 lines, [GIST](https://gist.github.com) is preferred.

**Additional Context**
Add any other context about the problem here.