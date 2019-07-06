INPUT_FILE = input
OUTPUT_FILE = out

BIN_NAME = FastRoute
BIN_ARGS = $(INPUT_FILE) -o $(OUTPUT_FILE)

TEST_BIN = run_tests.sh

LIB_NAME = libFastRoute.a

PRE_CMD =
POS_CMD = 2>&1 | tee $(INPUT_FILE).log

BUILD_DIR = build
SUPPORT_DIR = support
LOG_DIR = log

PARALLEL = 1

TYPE = normal

default: $(TYPE)

all: clean release

run:
	$(PRE_CMD) ./$(BIN_NAME) $(BIN_ARGS) $(POS_CMD)

debug: dirs cmake_debug call_make
normal: dirs cmake_normal call_make
release: dirs cmake_release call_make
lib: dirs cmake_lib call_make_lib

call_make:
	@echo Call make
	@make -C $(BUILD_DIR) $(MK_OPT) -j$(PARALLEL) --no-print-directory
	@echo Remove old binary
	@rm -f $(BIN_NAME)
	@echo Copy binary
	@cp $(BUILD_DIR)/$(BIN_NAME) .

call_make_lib:
	@echo Call make
	@make -C $(BUILD_DIR) $(MK_OPT) -j$(PARALLEL) --no-print-directory
	@echo Remove old binary
	@rm -f lib/$(LIB_NAME)
	@echo Copy binary
	@cp $(BUILD_DIR)/$(LIB_NAME) lib/

cmake_normal:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Debug .. ;\
		)


cmake_lib:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		export TYPE_CALL=1 ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Debug .. ;\
		)


cmake_release:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Release .. ;\
		)

cmake_debug:
	@( \
		echo Change to $(BUILD_DIR) ;\
		cd $(BUILD_DIR) ;\
		echo Call cmake ;\
		cmake $(CM_OPT) -DCMAKE_BUILD_TYPE=Debug .. ;\
		)

dirs:
	@( \
		echo Create $(BUILD_DIR) ;\
		mkdir -p $(BUILD_DIR) ;\
		)

test: $(BIN_NAME)
	@( \
		echo Removing old log folder ;\
		rm -rf $(LOG_DIR) ;\
		echo Changing to support folder ;\
		cd $(SUPPORT_DIR) ;\
		echo "Running tests ($(TEST_BIN))" ;\
		bash $(TEST_BIN) ;\
		)

$(BIN_NAME): $(TYPE)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LOG_DIR)

clean_all:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_NAME)
