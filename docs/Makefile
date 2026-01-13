# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
#SPHINXBUILD   ?= sphinx-build
SPHINXBUILD   ?= python3 -m sphinx.cmd.build
CMAKE_RST_GEN ?= python3 ../scripts/generate_cmake_rst.py --skip_private --skip_undocumented --headline="SDK CMake API Reference" ../cmake
DOXYGEN 	  ?= doxygen

all: html

cmake_intro:
	@$(CMAKE_RST_GEN) -o cmake_intro.rst

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
en: Makefile cmake_intro
	@$(SPHINXBUILD) -M html "en" "output/en" $(SPHINXOPTS) $(O)

zh: Makefile cmake_intro
	@$(SPHINXBUILD) -M html "zh" "output/zh" $(SPHINXOPTS) $(O)

api_doc:
	@rm -rf output/api_doc && mkdir -p output/api_doc
	@export HPM_SDK_BASE=$(shell pwd)/../; cd output/api_doc && $(DOXYGEN) ../../doxygen/Doxyfile

html: en zh api_doc
.PHONY: all

clean:
	@rm -rf output
	@rm -rf en/boards
	@rm -rf en/samples
	@rm -rf zh/boards
	@rm -rf zh/samples

