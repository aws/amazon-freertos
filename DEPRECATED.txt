# DEPRECATED

## Announcement:
As of November 18th, 2022, this repository is deprecated. The contents of this repository will remain available but we will no longer provide updates or accept new contributions and pull requests.
We recommend you start [here] (https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started-modular.html) for creating a new project.
If you have an existing FreeRTOS project based on this repository, see the [migration guide](https://docs.aws.amazon.com/freertos/latest/userguide/github-repo-migration.html). 

Over the years, AWS has improved the modularity of the FreeRTOS libraries and repository structure to make it easier for you to build and update FreeRTOS-based projects. This repository deprecation aligns with some of these significant initiatives:
*   We decomposed libraries to include them in their individual repositories and removed interdependencies between each library giving you the flexibility to choose the FreeRTOS libraries and project structure that suits your project and toolchain.
*   We split libraries that are AWS dependent and FreeRTOS dependent into separate repositories giving you the option to mix and match libraries that are specific to your board and use case.
*   We provided  feature stability, security patches, and critical bug fixes through the Long Term Support (LTS) libraries. 

Have more questions? Post them in the [FreeRTOS forum](https://forums.freertos.org/).