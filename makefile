BUILD_DIR = build

BIN_DIR = .

PARALLEL = 1

.PHONY: all dirs bin_cp

all: dirs compile bin_cp

compile: $(BIN_NAME)
	@( \
		mkdir build ;\
		cd build ;\
		cmake .. ;\
		make --no-print-directory -j$(PARALLEL) ;\
		)

dirs:
	@( \
		echo Create $(BUILD_DIR) ;\
		mkdir -p $(BUILD_DIR) ;\
		)

bin_cp:
	@( \
		cp build/rsyn/bin/rsyn $(BIN_DIR)/FRlefdef ;\
		cp fastroute-lib/POST9.dat $(BIN_DIR)/ ;\
		cp fastroute-lib/POWV9.dat $(BIN_DIR)/ ;\
		)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LOG_DIR)

clean_all:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
