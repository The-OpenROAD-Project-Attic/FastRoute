BUILD_DIR = build

BIN_DIR = .

BIN_NAME = FRlefdef

CMAKE_OPT =

MAKE_OPT =

PARALLEL = 1

.PHONY: all dirs bin_cp

all: dirs compile bin_cp

compile:
	@( \
		mkdir build ;\
		cd build ;\
		cmake .. $(CMAKE_OPT) ;\
		make --no-print-directory -j$(PARALLEL) $(MAKE_OPT) ;\
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

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LOG_DIR)

clean_all:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
