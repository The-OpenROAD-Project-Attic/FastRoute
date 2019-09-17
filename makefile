################################################################################
## Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora Oliveira
##          (Advisor: Ricardo Reis)
##
## BSD 3-Clause License
##
## Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice, this
##   list of conditions and the following disclaimer.
##
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and#or other materials provided with the distribution.
##
## * Neither the name of the copyright holder nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
################################################################################

BUILD_DIR = build

ROOT = ${PWD}

BIN_DIR = .
BIN_NAME = FRlefdef
OUTPUT_FILE = third_party/rsyn/bin/rsyn
# LIB_NAME =

SUPPORT_DIR = support
BENCHMARKS_DIR = $(SUPPORT_DIR)/ispd18

CMAKE = cmake
CMAKE_OPT =
MAKE = make
MAKE_OPT =

PARALLEL = 1

.PHONY: default
default: release ispd18_unit_test

.PHONY: all
all: clean default

.PHONY: release
release: setup
	@echo Change to $(BUILD_DIR)/$@
	@echo Call $(CMAKE)
	@cd $(BUILD_DIR)/$@ && $(CMAKE) $(CMAKE_OPT) -DCMAKE_BUILD_TYPE=$@ $(ROOT)
	@echo Call $(MAKE)
	@$(MAKE) -C $(BUILD_DIR)/$@ -j$(PARALLEL) --no-print-directory $(MK_OPT)
	@echo Remove old binary
	@rm -f $(BIN_NAME)
	@echo Copy binary
	@ln -f -s $(BUILD_DIR)/$@/$(OUTPUT_FILE) $(BIN_NAME)

.PHONY: debug
debug: setup
	@echo Change to $(BUILD_DIR)/$@
	@echo Call $(CMAKE)
	@cd $(BUILD_DIR)/$@ && $(CMAKE) $(CMAKE_OPT) -DCMAKE_BUILD_TYPE=$@ $(ROOT)
	@echo Call $(MAKE)
	@$(MAKE) -C $(BUILD_DIR)/$@ -j$(PARALLEL) --no-print-directory $(MK_OPT)
	@echo Remove old binary
	@rm -f $(BIN_NAME)
	@echo Copy binary
	@ln -f -s $(BUILD_DIR)/$@/$(OUTPUT_FILE) $(BIN_NAME)

.PHONY: setup
setup: check_submodules dirs
	@ln -f -s $(SUPPORT_DIR)/POST9.dat $(BIN_DIR)/
	@ln -f -s $(SUPPORT_DIR)/POWV9.dat $(BIN_DIR)/

.PHONY: check_submodules
check_submodules:
	@echo "Initialize submodules"
	@git submodule init
	@echo "Update submodules"
	@git submodule update

.PHONY: dirs
dirs:
	@echo Create $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/debug
	@mkdir -p $(BUILD_DIR)/release

.PHONY: ispd18_unit_test
ispd18_unit_test:
	@bash $(SUPPORT_DIR)/tests/ispd18_unit_test.sh

.PHONY: ispd18_download
ispd18_download:
	@bash $(SUPPORT_DIR)/ispd18_download.sh $(BENCHMARKS_DIR)

.PHONY: ispd18_clean
ispd18_clean:
	git clean -xdf $(SUPPORT_DIR)/ispd18

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean_all
clean_all: ispd18_clean
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
