# export PATH=${HOME}/cbmc/src/cbmc:${HOME}/cbmc/src/goto-cc:${HOME}/cbmc/src/goto-instrument:${HOME}/cbmc/src/goto-analyzer:${HOME}/cbmc-viewer:${HOME}/cbmc-batch/bin:${PATH}

default: report

################################################################
# Define locations and binaries

FREERTOS ?= ${HOME}/freertos

GOTO_CC ?= goto-cc
GOTO_INSTRUMENT ?= goto-instrument
GOTO_ANALYZER ?= goto-analyzer
BATCH ?= cbmc-batch
VIEWER ?= cbmc-viewer

################################################################
# Define includes and options taken from aws_demos.vcxproj

INC += \
	-I$(FREERTOS)/demos/pc/windows/common/config_files \
	-I$(FREERTOS)/demos/pc/windows/common/application_code/include \
	-I$(FREERTOS)/demos/common/include \
	-I$(FREERTOS)/lib/include \
	-I$(FREERTOS)/lib/include/private \
	-I$(FREERTOS)/lib/FreeRTOS/include \
	-I$(FREERTOS)/lib/FreeRTOS/portable/MSVC-MingW \
	-I$(FREERTOS)/lib/FreeRTOS-Plus-TCP/include \
	-I$(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/portable/BufferManagement \
	-I$(FREERTOS)/lib/FreeRTOS-Plus-TCP/source/portable/Compiler/MSVC \
	-I$(FREERTOS)/lib/ota/portable/pc/windows \
	-I$(FREERTOS)/lib/third_party/mbedtls/include \
	-I$(FREERTOS)/lib/third_party/tracealyzer_recorder/Include \
	-I$(FREERTOS)/lib/third_party/jsmn \
	-I$(FREERTOS)/lib/third_party/pkcs11 \
	-I$(FREERTOS)/lib/third_party/tinycbor \
	-I$(FREERTOS)/lib/third_party/win_pcap \
	-I$(FREERTOS)/lib/cbor/src \
	-I$(FREERTOS)/lib/defender/src \
	-I$(FREERTOS)/lib/defender/src/metrics \
	-I$(FREERTOS)/lib/defender/src/report \
	-I./include \

DEF += \
	-D_DEBUG \
	-D__free_rtos__ \
	-D_CONSOLE \
	-D_WIN32_WINNT=0x0500 \
	-DWINVER=0x400 \
	-D_CRT_SECURE_NO_WARNINGS \
	-D__PRETTY_FUNCTION__=__FUNCTION__ \

DEF += -DCBMC=1

OPT += /wd4210 /wd4127 /wd4214 /wd4201 /wd4244 /wd4310

CFLAGS += $(CFLAGS2) $(DEF) $(OPT) -m32

################################################################

# ProcessDHCPReplies needs 7 bits, most need at most 4
# This value should be a command line macro passed down to include/cbmc.h
CBMCFLAGS += \
	--object-bits 7 \
	$(UNWINDING) \
	--32 \
	--bounds-check \
	--pointer-check \

UNWIND ?= 0
SIMPLIFY ?= 0

ABSTRACTIONS ?=

$(ENTRY)1.goto: $(OBJS)
	$(GOTO_CC) --function harness -o $@ $(OBJS)

$(ENTRY)3.goto: $(ENTRY)1.goto
	 $(GOTO_INSTRUMENT) $(ABSTRACTIONS) --add-library $< $@ \
		2>&1 | tee $(ENTRY)3.txt ; exit $${PIPESTATUS[0]}

# Simplify and constant propagation may benefit from unwinding first
$(ENTRY)4.goto: $(ENTRY)3.goto
ifeq ($(UNWIND_GOTO), 1)
	$(GOTO_INSTRUMENT) $(UNWINDING) $< $@ \
		2>&1 | tee $(ENTRY)4.txt ; exit $${PIPESTATUS[0]}
else
	cp $< $@
endif

# Skip simplify (and hence generate-function-body) until missing source locations debugged
$(ENTRY)5.goto: $(ENTRY)4.goto
ifeq ($(SIMPLIFY), 1)
	$(GOTO_INSTRUMENT) --generate-function-body '.*' $< $@ \
		2>&1 | tee $(ENTRY)5.txt ; exit $${PIPESTATUS[0]}
else
	cp $< $@
endif

# Skip simplify (and hence generate-function-body) until missing source locations debugged
$(ENTRY)6.goto: $(ENTRY)5.goto
ifeq ($(SIMPLIFY), 1)
	$(GOTO_ANALYZER) --simplify $@ $< \
		2>&1 | tee $(ENTRY)6.txt ; exit $${PIPESTATUS[0]}
else
	cp $< $@
endif

$(ENTRY)7.goto: $(ENTRY)6.goto
	$(GOTO_INSTRUMENT) --drop-unused-functions $< $@ \
		2>&1 | tee $(ENTRY)7.txt ; exit $${PIPESTATUS[0]}

$(ENTRY)8.goto: $(ENTRY)7.goto
	$(GOTO_INSTRUMENT) --slice-global-inits $< $@ \
		2>&1 | tee $(ENTRY)8.txt ; exit $${PIPESTATUS[0]}

$(ENTRY).goto: $(ENTRY)8.goto
	cp $< $@

%.goto : %.c
	$(GOTO_CC) -o $@ $(INC) $(CFLAGS) $<

cbmc.txt: $(ENTRY).goto
	cbmc $(CBMCFLAGS) --trace $< 2>&1 | tee $@

property.xml: $(ENTRY).goto
	cbmc $(CBMCFLAGS) --show-properties --xml-ui $< 2>&1 > $@

coverage.xml: $(ENTRY).goto
	cbmc $(filter-out --unwinding-assertions,$(CBMCFLAGS)) --cover location --xml-ui $< 2>&1 > $@

cbmc: cbmc.txt

property: property.xml

coverage: coverage.xml

report: cbmc.txt property.xml coverage.xml
	$(VIEWER) \
	--goto $(ENTRY).goto \
	--srcdir $(FREERTOS) \
	--blddir $(FREERTOS) \
	--htmldir html \
	--srcexclude "(./verification|./tests|./tools|./lib/third_party)" \
	--result cbmc.txt \
	--property property.xml \
	--block coverage.xml

clean:
	$(RM) $(CHECK_OPTIONS_OBJ) $(PROCESS_DHCP_REPLIES_OBJ) $(PARSE_DNS_REPLY_OBJ)
	$(RM) $(GOTOS)
	$(RM) $(OBJS) $(ENTRY).goto
	$(RM) $(ENTRY)[0-9].goto $(ENTRY)[0-9].txt
	$(RM) cbmc.txt property.xml coverage.xml TAGS
	$(RM) *~ \#*

veryclean: clean
	$(RM) -r html

.PHONY: cbmc property coverage report clean veryclean

################################################################
# Launching cbmc on cbmc-batch

BATCH ?= cbmc-batch
WS ?= ws

define encode_options
       '=$(shell echo $(1) | sed 's/ ,/ /g' | sed 's/ /;/g')='
endef

PROPMEM ?= 64000
COVMEM ?= 64000
CBMCPKG ?= cbmc-20180820
BATCHPKG ?= cbmc-batch-20180913
VIEWERPKG ?= cbmc-viewer-20180913

SRC_ROOT ?= $(FREERTOS)
SRC_TARFILE ?= s3://cbmc/freertos-20180821.tar.gz

BATCHFLAGS ?= \
	--srcdir $(FREERTOS) \
	--wsdir $(WS) \
	--jobprefix $(ENTRY) \
	--no-build \
	--goto $(ENTRY).goto \
	--cbmcflags $(call encode_options,$(CBMCFLAGS)) \
	--property-memory $(PROPMEM) \
	--coverage-memory $(COVMEM) \
	--cbmcpkg $(CBMCPKG) \
	--batchpkg $(BATCHPKG) \
	--viewerpkg $(VIEWERPKG) \
	--no-copysrc \
	--srctarfile $(SRC_TARFILE) \
	--blddir $(FREERTOS) \

define yaml_encode_options
       "$(shell echo $(1) | sed 's/ ,/ /g' | sed 's/ /;/g')"
endef

$(ENTRY).yaml: $(ENTRY).goto Makefile
	echo 'jobos: ubuntu14' > $@
	echo 'cbmcpkg: $(CBMCPKG)' >> $@
	echo 'batchpkg: $(BATCHPKG)' >> $@
	echo 'viewerpkg: $(VIEWERPKG)' >> $@
	echo 'goto: $(ENTRY).goto' >> $@
	echo 'build: false' >> $@
	echo 'cbmcflags: $(call yaml_encode_options,$(CBMCFLAGS))' >> $@
	echo 'property_memory: $(PROPMEM)' >> $@
	echo 'coverage_memory: $(COVMEM)' >> $@
	echo 'expected: "SUCCESSFUL"' >> $@

launch: $(ENTRY).goto Makefile
	mkdir -p $(WS)
	cp $(ENTRY).goto $(WS)
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
