---
name: Bug Report
about: Create a report to help us improve amazon-freertos by identifying any confirmed
  bugs. To discuss a potential bug, or pose other inquiries, please open an issue
  using the template linked below.
title: "[BUG] <replace with your title>"
labels: 'Type: bug'
assignees: ''

---

**Describe the bug**
A clear and concise description explaining the bug.

**System information**
- Hardware board: [ ... ]
- IDE used: [ ... ]
- Operating System: [ Windows | Linux | MacOS ]
- Code version: (run ``git describe --tags`` to find it)
- Project/Demo: [ mqtt_demo_mutual_auth | shadow_demo | custom application | etc... ]
- If your project is a custom application, please add the relevant code
  snippet(s) in the section named `Code to reproduce the bug`.

**Expected behavior**
A clear description of the expected behavior.

**Screenshots or console output**
If appropriate, please paste the console output/error log explaining the issue.
If possible, include the call stack.

**Steps to reproduce**
Example:
1. "I am using project [ ... ], and configured with [ ... ]"
2. "When run on [ ... ], I observed [ ... ]"

...

**Code to reproduce the bug**
The code should be wrapped in the ``'cpp'`` tag in order to be displayed clearly. For example:

```cpp
printf("Hello World")
```

**Additional context**
Add any other context about the problem here.

<!-- For general inquiries, please open an issue with the "Type: Question" tag using [this template](https://forums.freertos.org). -->
