---
name: Bug Report
about: Create a report to help us improve amazon-freertos by identifying any confirmed
  bugs. To discuss a potential bug, or pose other inquiries, please open an issue
  using the template linked below.
title: "[BUG] <replace with your title>"
labels: 'Type: Bug'
assignees: ''

---

**Describe the bug**
Please provide a clear and concise description explaining the bug.

**System information**
- Hardware board: [ ... ]
- IDE used: [ ... ]
- Operating System: [ Windows | Linux | MacOS ]
- Code version: (run ``git describe --tags`` to find it)
- Project/Demo: [ mqtt_demo_mutual_auth | shadow_demo | custom application | etc... ]
- If your project is a custom application, please add the relevant code snippet(s) in the section titled `"Code to reproduce bug"`.

**Expected behavior**
A clear description of the expected behavior.

**Screenshots or console output**
If appropriate, please paste the console output/error log explaining the issue. If possible, include the call stack.

**Steps to reproduce bug**
    Example:
      1. "I am using project [ ... ], and have configured with [ ... ]"
      2. "When run on [ ... ], I observed that [ ... ]"

**Code to reproduce bug**
The code should be wrapped in the ``cpp`` tag in order to be displayed clearly. For example:

```cpp
printf("Hello World")
```

**Additional context**
Include any additional context relevant to the issue here.

<!-- For general inquiries, please open an issue with the "Type: Question" tag using [this template](.github/ISSUE_TEMPLATE/general_inquiry.md). -->
