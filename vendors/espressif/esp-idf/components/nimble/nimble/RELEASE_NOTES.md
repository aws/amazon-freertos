# RELEASE NOTES

04 June 2018 - Apache NimBLE v1.0.0

For full release notes, please visit the
[Apache Mynewt Wiki](https://cwiki.apache.org/confluence/display/MYNEWT/Release+Notes).

Apache NimBLE is an open-source Bluetooth 5.0 stack (both Host & Controller) that completely
replaces the proprietary SoftDevice on Nordic chipsets.

New features in this version of NimBLE include:

* Initial version after moving to separate repo - https://github.com/apache/mynewt-nimble
* removal of deprecated bletiny application (superseded by btshell)
* Added NimBLE Porting Layer (NPL) which abstracts OS specific details to improve portability
* Initial ports for FreeRTOS and RIOT OS
* Support for advertising up to 1650 bytes of data with Extended Advertising
* Support for host flow control
* Support for Direct Test Mode (DTM) via HCI interface
* Support for Device Information Service
* Bugfixes for issues found on UPF59 and during BT certification testing
* Lots of other bugfixes, minor enhancements and optimisations
* Mesh improvements

If working on next-generation RTOS and Bluetooth protocol stack
sounds exciting to you, get in touch, by sending a mail to the Apache Mynewt
Developer's list, dev@mynewt.apache.org.
