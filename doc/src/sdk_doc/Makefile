# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

ifndef HPM_SDK_BASE
  $(error HPM_SDK_BASE is not defined)
endif

ifeq (,$(wildcard $(HPM_SDK_BASE)/env.sh))
  $(error can not locate HPM_SDK_BASE:$(HPM_SDK_BASE))
endif

LANGUAGE   ?= en
SPHINXBUILD ?= sphinx-build
SOURCE_DIR   = $(HPM_SDK_BASE)
VERSION_FILE = $(HPM_SDK_BASE)/VERSION
DOC_OUTPUT_DIR ?= $(HPM_SDK_BASE)/doc/output/
SDK_DOC_OUTPUT_DIR ?= $(DOC_OUTPUT_DIR)/sdk_doc

DOXYGENBUILD ?= doxygen
API_DOC_DOXYFILE_PATH = $(HPM_SDK_BASE)/doc/src/api_doc/Doxyfile

export HPM_DOC_APIDOC_OUTPUT_DIR=$(DOC_OUTPUT_DIR)/api_doc

$(shell if [ ! -e $(DOC_OUTPUT_DIR) ];then mkdir -p $(DOC_OUTPUT_DIR); fi)

# fetch latest version number from SDK verion
VERSION     ?= $(shell awk '/^VERSION_MAJOR.*/{sub(/^0*/, "", $$3); v=$$3; if (!v) v=0; ver=v "."}; /^VERSION_MINOR.*/{sub(/^0*/, "", $$3); v=$$3; if (!v) v=0; ver= ver v "."}; /^PATCHLEVEL.*/{sub(/^0*/, "", $$3); v=$$3; if (!v) v=0; ver= ver v}; /^EXTRAVERSION.*/{extra = $$3; if (extra) ver = ver "-" extra}; END { print ver}' $(VERSION_FILE))

l := $(shell echo $(LANGUAGE) | tr '[A-Z]' '[a-z]')
ifneq "$(l)" "en"
ifneq "$(l)" "zh"
  $(error x can only be en or zh)
endif
endif

SPHINX_OUTPUT_DIR	=$(SDK_DOC_OUTPUT_DIR)/$(l)
$(shell if [ ! -e $(SPHINX_OUTPUT_DIR) ];then mkdir -p $(SPHINX_OUTPUT_DIR); fi)
SPHINX_OPTS	=-c $(HPM_SDK_BASE)/doc/src/sdk_doc -t $(l)

# Put it first so that "make" without argument is like "make help".
help:
	@echo "please use `make target` where target is one of"
	@echo "  latest  to make html documentation into latest output path"
	@echo "  release to make html documentation into specific release output path"
	@echo ""
	@echo "LANGUAGE can be used to select lanuage, only en/zh are supported, e.x."
	@echo "  make latest LANGUAGE=en"

.PHONY: help clean_latest clean_release latest release

# Generate documentation and update output for the latest
latest:
	@echo $(HPM_DOC_APIDOC_OUTPUT_DIR)
	@$(DOXYGENBUILD) $(API_DOC_DOXYFILE_PATH)
	@$(SPHINXBUILD) -M html "$(SOURCE_DIR)" "$(SPHINX_OUTPUT_DIR)/$@/" $(SPHINX_OPTS) -t publish

# Generate documentation and update output for specific verion
release:
	@$(DOXYGENBUILD) $(API_DOC_DOXYFILE_PATH)
	$(SPHINXBUILD) -M html "$(SOURCE_DIR)" "$(SPHINX_OUTPUT_DIR)/$(VERSION)/" $(SPHINX_OPTS) -t publish -t release

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINX_OPTS).
clean_latest:
	@$(SPHINXBUILD) -M clean "$(SOURCE_DIR)" "$(SPHINX_OUTPUT_DIR)/latest" $(SPHINX_OPTS) $(O)

clean_release:
	@$(SPHINXBUILD) -M clean "$(SOURCE_DIR)" "$(SPHINX_OUTPUT_DIR)/$(VERSION)" $(SPHINX_OPTS) $(O)

clean:
	@$(SPHINXBUILD) -M $@ "$(SOURCE_DIR)" "$(SPHINX_OUTPUT_DIR)" $(SPHINX_OPTS) $(O)
