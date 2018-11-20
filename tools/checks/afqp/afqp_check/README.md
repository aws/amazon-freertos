### Amazon FreeRTOS Qualifcation Program (AFQP) Check

#### Update to Directory Structure
Update src/ rules.cfg with the names of files and directories of importance.
Update AFR_ROOT in src/afqp_check.py for updated relative path from tools/checks/afqp/afqp_check.
Update PORTABLE_LAYER_PATHS in src/afqp_check.py, if applicable.

#### Dependencies
* Python 2.7+ or Python 3+

#### The Checks
* Check for required AFQP files:
    * see **tools/checks/afqp/fs_checkrules.json**
* Check for **configPLATFORM_NAME** in **FreeRTOSConfig.h**
    * Return *error* if missing.
* Check for build artifiacts.
    * Return *error* if present.
* Check for vendor license in AFQP files.
    * Return *warning* if the Amazon FreeRTOS license is in ported code.
    * Return *error* for each missing Amazon FreeRTOS copyright.
    * Return *error* for incorrect copyright year.
    * Return *error* for incorrect versions on ported files.
* Check for a compliant Eclipse project.
    * Return *error* for naming demo project anything other than "**aws_demos**" in the **.project** file.
    * Return *error* for multiple project roots defined in the ide's **.project** file.
    * Return *error" for each missing **\<locationURI\>** tag in the linked resource in the **.project** file.
    * Return *warning* for each include path in the **.cproject** not extending from the single root variable defined in the **.project**.
    
#### Testing
Run **pytest** from this directory (tools/checks/afqp/afqp_check)

##### Dependencies
* ```pytest```
* ```pytest-cov```
