#!/bin/bash

# This is used to generate .project file for IDE in AFR.
# This use meta-programming to modify the makefile and run the makefile to generate the .project file

############### WARNING: This file will overwrite any changes to amazon-freertos/projects/cypress/$board/mtb/$project_name/Makefile #############

board="$1"
project_name="${2:-aws_demos}"

pushd "$(dirname "${BASH_SOURCE[0]}")/../projects/cypress/$board/mtb/$project_name"

ide_link_locationuri="\$(patsubst ./%,PARENT-5-PROJECT_LOC/projects/cypress/${board}/mtb/${project_name}/%,\$(patsubst ../../../../../%,PARENT-5-PROJECT_LOC/%,\$(file)))"
ide_dir_link_str="\t\t<link>\n\t\t\t<name>\$(patsubst ../../../../../%,%,\$(file))</name>\n\t\t\t<type>2</type>\n\t\t\t<locationURI>$ide_link_locationuri</locationURI>\n\t\t</link>\n)"
ide_file_link_str="\t\t<link>\n\t\t\t<name>\$(patsubst ../../../../../%,%,\$(file))</name>\n\t\t\t<type>1</type>\n\t\t\t<locationURI>\$(patsubst ./%,PARENT-5-PROJECT_LOC/projects/cypress/${board}/mtb/${project_name}/%,\$(patsubst ../../../../../%,PARENT-5-PROJECT_LOC/%,\$(file)))</locationURI>\n\t\t</link>\n)"

echo ""
echo ".PHONY: generate_mtb_project" >>Makefile
echo "generate_mtb_project: secondstage" >>Makefile
echo "ifeq (\$(CY_SECONDSTAGE),true)" >>Makefile
echo "	rm .project" >>Makefile

echo "	@echo -e \"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\n<projectDescription>\n	<name>$project_name</name>\n	<comment></comment>\n	<projects>\n	</projects>\n	<buildSpec>\n		<buildCommand>\n			<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>\n			<triggers>clean,full,incremental,</triggers>\n			<arguments>\n			</arguments>\n		</buildCommand>\n		<buildCommand>\n			<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>\n			<triggers>full,incremental,</triggers>\n			<arguments>\n			</arguments>\n		</buildCommand>\n	</buildSpec>\n	<natures>\n		<nature>org.eclipse.cdt.core.cnature</nature>\n		<nature>org.eclipse.cdt.core.ccnature</nature>\n		<nature>com.cypress.studio.app.cymodusnature</nature>\n		<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>\n		<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>\n	</natures>\n	<linkedResources>\n\" >>.project" >>Makefile

#generate the include directory links
echo "	@echo -e \"\$(foreach file,\$(INCLUDES),$ide_dir_link_str\" >>.project" >>Makefile
echo "	@echo -e \"\$(foreach file,\$(filter-out \$(INCLUDES),\$(CY_SEARCH_AVAILABLE_H_INCLUDES)),$ide_dir_link_str\" >>.project" >>Makefile
echo "	@echo -e \"\$(foreach file,\$(filter-out \$(INCLUDES),\$(CY_SEARCH_AVAILABLE_HPP_INCLUDES)),$ide_dir_link_str\" >>.project" >>Makefile

#generate the source file links
echo "	@echo -e \"\$(foreach file,\$(SOURCES),$ide_file_link_str\" >>.project" >>Makefile
echo "	@echo -e \"\$(foreach file,\$(CY_RECIPE_SOURCE),$ide_file_link_str\" >>.project" >>Makefile
echo "	@echo -e \"\$(foreach file,\$(CY_RECIPE_GENERATED),$ide_file_link_str\" >>.project" >>Makefile
echo "	@echo -e \"\$(foreach file,\$(filter-out \$(SOURCES),\$(CY_SEARCH_APP_SOURCE)),$ide_file_link_str\" >>.project" >>Makefile

echo "	@echo -e \"\t</linkedResources>\n</projectDescription>\" >>.project" >>Makefile
echo "endif" >>Makefile

make generate_mtb_project

git checkout Makefile

popd
