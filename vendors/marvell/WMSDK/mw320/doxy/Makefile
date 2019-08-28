# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

PLANTUML_JAR ?= $(wildcard ~/build-deps/plantuml.jar)

.PHONY: all clean

t_cp    := $(shell which cp | tail -1)


include files-sdk.mk

define create_refman
$(1).doc:
	mkdir -p gen/
	$(t_cp) doxy-$(1).conf gen/doxy-$(1).conf
	chmod +w gen/doxy-$(1).conf
	echo 'INPUT=$($(1)-files)' >> gen/doxy-$(1).conf
ifneq ($(PLANTUML_JAR),)
# Enable uml_diag only if the images can be generated
	echo 'ENABLED_SECTIONS=uml_diag' >> gen/doxy-$(1).conf
	echo 'IMAGE_PATH=doxy/gen/images' >> gen/doxy-$(1).conf
	mkdir -p gen/images
	cd .. ; java -jar $(PLANTUML_JAR) -v -o `pwd`/doxy/gen/images $($(1)-files)
endif
	cd .. ; doxygen doxy/gen/doxy-$(1).conf
	$(t_cp) -f utils/logo.gif refman/html/
	$(t_cp) -f utils/doxygen.sty utils/Marvell_logo_spot_PMS_cropped.png utils/Marvell_logotype_c_small.png refman/latex/
	$(MAKE) -C refman/latex/ refman.pdf
	mkdir -p refman/pdf/
	mv refman/latex/refman.pdf refman/pdf/$(1)-reference-manual.pdf
endef

list_of_refman := sdk bin_sdk


$(foreach elem, $(list_of_refman),$(eval $(call create_refman,$(elem))))

ifneq ($(BIN_SDK_NAME),)

PUB_HDR_LIST := ../bin_sdks/$(BIN_SDK_NAME)/Internal/public_headers
bin_sdk-files := $(foreach l,$(shell cat $(PUB_HDR_LIST)), $(patsubst sdk/%,%,$(l)))
bin_sdk-files += ./bin_sdks/$(BIN_SDK_NAME)/Internal/README.refman

all: bin_sdk.doc
else
doxy.all: sdk.doc 

endif

clean:
	$(AT)rm -fr ./refman/ ./gen

