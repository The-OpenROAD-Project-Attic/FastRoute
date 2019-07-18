PARALLEL = 1

.PHONY: all

all:
	@( \
		mkdir build ;\
		cd build ;\
		cmake .. ;\
		make -j$(PARALLEL) ;\
		)

dirs:
	@( \
		echo Create $(BUILD_DIR) ;\
		mkdir -p $(BUILD_DIR) ;\
		)

$(BIN_NAME): $(TYPE)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LOG_DIR)

clean_all:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
