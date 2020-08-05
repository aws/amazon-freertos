## BSP for CY8CKIT-062S2-43012 

This BSP has the following limitations/workarounds (common issues for all BSP), also see https://confluence.cypress.com/x/sZN5B):

- BSP initialize function uses both configurator generated code (PDL based init) as well as HAL based init. [DRIVERS-137](https://jira.cypress.com/browse/DRIVERS-137) 
- Retargetting to the default UART doesn't use the Retarget I/O middleware utility, instead implements it within the BSP. This will be updated once the following JIRA is fixed: [BSP-651](https://jira.cypress.com/browse/BSP-651)
- CapSense: Not enabled due to [BSP-600](https://jira.cypress.com/browse/BSP-600)

For instructions on manual bench testing this BSP, see BSP manual testing in [Board Support Packages (BSP)](https://confluence.cypress.com/pages/viewpage.action?pageId=75076561)

