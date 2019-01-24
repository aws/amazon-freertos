-include Makefile.local

SHELL=bash
show_progress = @echo ; echo "*** $1 ***"; echo

################################################################

CBMCFLAGS += \
	$(UNWINDING) \
	--verbosity 10 \
	--bounds-check \
	--pointer-check \
	--object-bits 16 \
	--timestamp wall \

# --unwindset
UNWINDING += \
	--unwind 3 \
	--unwinding-assertions \

ABSTRACTIONS += \

################################################################
# Preparing the goto program for cbmc

default: $(TARGET).goto

TARGET=$(ENTRY)0

$(TARGET)3.goto: $(ENTRY).goto
	cp $< $@

$(TARGET)4.goto: $(TARGET)3.goto
	$(GOTO_INSTRUMENT) $(UNWINDING) $< $@ \
		2>&1 | tee $(TARGET)4.txt ; exit $${PIPESTATUS[0]}

$(TARGET)5.goto: $(TARGET)4.goto
	$(GOTO_INSTRUMENT) --generate-function-body '.*' $< $@ \
		2>&1 | tee $(TARGET)5.txt ; exit $${PIPESTATUS[0]}

$(TARGET)6.goto: $(TARGET)5.goto
	$(GOTO_ANALYZER) --simplify $@ $< \
		2>&1 | tee $(TARGET)6.txt ; exit $${PIPESTATUS[0]}

$(TARGET)7.goto: $(TARGET)6.goto
	$(GOTO_INSTRUMENT) --drop-unused-functions $< $@ \
		2>&1 | tee $(TARGET)7.txt ; exit $${PIPESTATUS[0]}

$(TARGET)8.goto: $(TARGET)7.goto
	$(GOTO_INSTRUMENT) --slice-global-inits $< $@ \
		2>&1 | tee $(TARGET)8.txt ; exit $${PIPESTATUS[0]}

$(TARGET).goto: $(TARGET)8.goto
	cp $< $@

clean:
	$(RM) $(TARGET)*.goto $(TARGET)*.txt

veryclean: clean
	$(RM) -r $(ENTRY).goto

################################################################
# Launching cbmc on cbmc-batch

BATCH = cbmc-batch
WS=ws

define encode_options
       '=$(shell echo $(1) | sed 's/ ,/ /g' | sed 's/ /;/g')='
endef

PROPMEM = 64000
COVMEM = 64000
CBMCPKG = cbmc-20180820
#BATCHPKG = cbmc-batch-20180818
BATCHPKG = cbmc-batch-20180821
VIEWERPKG = cbmc-viewer-20180821

SRC_ROOT=$(FREERTOS)
SRC_TARFILE=s3://cbmc/freertos-20180821.tar.gz

BATCHFLAGS += \
	--srcdir $(SRC_ROOT) \
	--wsdir $(WS) \
	--jobprefix $(ENTRY) \
	--no-build \
	--goto $(TARGET).goto \
	--cbmcflags $(call encode_options,$(CBMCFLAGS)) \
	--property-memory $(PROPMEM) \
	--coverage-memory $(COVMEM) \
	--cbmcpkg $(CBMCPKG) \
	--batchpkg $(BATCHPKG) \
	--viewerpkg $(VIEWERPKG) \
	--no-copysrc \
	--srctarfile $(SRC_TARFILE) \
	--blddir 'd:\tuttle\freertos' \


launch: $(TARGET).goto Makefile
	mkdir -p $(WS)
	cp $(TARGET).goto $(WS)
	$(BATCH) $(BATCHFLAGS)

launch-clean:
	for d in $(ENTRY)*; do \
	  if [ -d $$d ]; then \
	    for f in $$d.json $$d.yaml Makefile-$$d; do \
	      if [ -f $$f ]; then mv $$f $$d; fi \
	    done\
	  fi \
	done
	$(RM) Makefile-$(ENTRY)-[0-9]*-[0-9]*
	$(RM) $(ENTRY)-[0-9]*-[0-9]*.json $(ENTRY)-[0-9]*-[0-9]*.yaml
	$(RM) -r $(WS)

launch-veryclean: launch-clean
	$(RM) -r $(ENTRY)-[0-9]*-[0-9]*

################################################################
# Packaging cbmc, cbmc-batch, and freertos and copying to S3

# CBMC

cbmc-clean:
	$(RM) /tmp/$(CBMC_PKG).tar.gz
	$(RM) -r /tmp/cbmc

cbmc-package:
	mkdir -p /tmp/cbmc
	cd $(CBMC_ROOT)/src; \
	cp \
		cbmc/cbmc \
		goto-cc/goto-cc \
		goto-instrument/goto-instrument \
		goto-analyzer/goto-analyzer \
		goto-diff/goto-diff \
			/tmp/cbmc
	tar fcz /tmp/$(CBMC_PKG).tar.gz -C /tmp cbmc


cbmc-install: cbmc-clean cbmc-package
	aws s3 cp /tmp/$(CBMC_PKG).tar.gz $(CBMC_S3)/

# CBMC Batch

batch-clean:
	$(RM) -r /tmp/cbmc-batch
	$(RM) /tmp/$(BATCH_PKG).tar.gz

batch-package:
	mkdir -p /tmp/cbmc-batch
	cp $(BATCH_ROOT)/bin/* /tmp/cbmc-batch
	tar fcz /tmp/$(BATCH_PKG).tar.gz -C /tmp cbmc-batch --exclude-vcs

batch-install: batch-clean batch-package
	aws s3 cp /tmp/$(BATCH_PKG).tar.gz $(BATCH_S3)/

# CBMC Viewer

viewer-clean:
	$(RM) -r /tmp/cbmc-viewer
	$(RM) /tmp/$(VIEWER_PKG).tar.gz

viewer-package:
	mkdir -p /tmp/cbmc-viewer
	cp $(VIEWER_ROOT)/* /tmp/cbmc-viewer
	tar fcz /tmp/$(VIEWER_PKG).tar.gz -C /tmp cbmc-viewer --exclude-vcs

viewer-install: viewer-clean viewer-package
	aws s3 cp /tmp/$(VIEWER_PKG).tar.gz $(VIEWER_S3)/

# FreeRTOS

NAME=$(notdir $(FREERTOS_ROOT))

freertos-clean:
	$(RM) /tmp/$(FREERTOS_PKG).tar.gz

freertos-package:
	tar fcz /tmp/$(FREERTOS_PKG).tar.gz -C $(FREERTOS_ROOT)/.. $(NAME) \
		--exclude $(NAME)/verification \
		--exclude-vcs

freertos-install: freertos-clean freertos-package
	aws s3 cp /tmp/$(FREERTOS_PKG).tar.gz $(FREERTOS_S3)/
