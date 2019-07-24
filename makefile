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

BIN_DIR = .
BIN_NAME = FRlefdef

BENCHMARKS_DIR = benchmarks
SUPPORT_DIR = support

CMAKE = cmake
CMAKE_OPT =

MAKE_OPT =

PARALLEL = 1

.PHONY: all dirs bin_cp

all: dirs compile bin_cp

compile:
	@( \
		mkdir -p $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		$(CMAKE) .. $(CMAKE_OPT) ;\
		$(MAKE) --no-print-directory -j$(PARALLEL) $(MAKE_OPT) ;\
		)

dirs:
	@( \
		echo Create $(BUILD_DIR) ;\
		mkdir -p $(BUILD_DIR) ;\
		)

bin_cp:
	@( \
		cp build/rsyn/bin/rsyn $(BIN_DIR)/$(BIN_NAME) ;\
		cp fastroute-lib/POST9.dat $(BIN_DIR)/ ;\
		cp fastroute-lib/POWV9.dat $(BIN_DIR)/ ;\
		)

unit_test: download
	@bash ./support/unit_test.sh

download:
	@( \
		if [ ! -d $(BENCHMARKS_DIR) ] ; then \
			echo Could not find $(BENCHMARKS_DIR) folder ; \
			mkdir -p $(BENCHMARKS_DIR) ; \
			echo Downloading $(BENCHMARKS_DIR) ; \
			bash ./support/download_ispd18.sh $(BENCHMARKS_DIR) ; \
		fi ; \
	)
	@cp -r $(SUPPORT_DIR)/ispd18/* $(BENCHMARKS_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LOG_DIR)

clean_all:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
