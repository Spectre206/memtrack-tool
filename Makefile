# ============================================================
# Makefile — Memory Leak Detection Tool
# Team: Adeel · Faizullah · Qasim
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -g -fPIC -I./include
LDFLAGS = -shared -ldl -lpthread

# Directories
SRC_DIR     = src
INC_DIR     = include
BUILD_DIR   = build
SAMPLE_DIR  = samples
TEST_DIR    = tests
LOG_DIR     = logs

# Output library
LIB         = $(BUILD_DIR)/libmemtrack.so

# ── Default target ──────────────────────────────────────────
.PHONY: all
all: dirs $(LIB)
	@echo ""
	@echo "  Build complete: $(LIB)"
	@echo "  Run programs with:"
	@echo "    LD_LIBRARY_PATH=./$(BUILD_DIR) ./your_program"
	@echo ""

# ── Create directories ──────────────────────────────────────
.PHONY: dirs
dirs:
	@mkdir -p $(BUILD_DIR) $(LOG_DIR)

# ── Build the shared library ────────────────────────────────
$(LIB): $(SRC_DIR)/memtrack.c $(SRC_DIR)/reporter.c $(INC_DIR)/memtrack.h
	$(CC) $(CFLAGS) $(LDFLAGS) \
		$(SRC_DIR)/memtrack.c \
		$(SRC_DIR)/reporter.c \
		-o $(LIB)
	@echo "  Built: $(LIB)"

# ── Build sample programs (LINKED MODE) ─────────────────────
.PHONY: samples
samples: $(LIB)
	$(CC) $(CFLAGS) $(SAMPLE_DIR)/sample_a.c -o $(BUILD_DIR)/sample_a -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(SAMPLE_DIR)/sample_b.c -o $(BUILD_DIR)/sample_b -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(SAMPLE_DIR)/sample_c.c -o $(BUILD_DIR)/sample_c -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	@echo "  Samples built in $(BUILD_DIR)/"

# ── Build test programs (LINKED & PURE VALGRIND MODE) ───────
.PHONY: tests
tests: $(LIB)
	@echo "  Building linked tests (for memtrack)..."
	$(CC) $(CFLAGS) $(TEST_DIR)/test1_no_leak.c      -o $(BUILD_DIR)/test1 -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(TEST_DIR)/test2_single_leak.c  -o $(BUILD_DIR)/test2 -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(TEST_DIR)/test3_multi_leak.c   -o $(BUILD_DIR)/test3 -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(TEST_DIR)/test4_large_leak.c   -o $(BUILD_DIR)/test4 -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(TEST_DIR)/test5_invalid_free.c -o $(BUILD_DIR)/test5 -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	$(CC) $(CFLAGS) $(TEST_DIR)/test6_multithread.c  -o $(BUILD_DIR)/test6 -L./$(BUILD_DIR) -lmemtrack -ldl -lpthread
	@echo "  Building pure tests (for Valgrind)..."
	$(CC) $(CFLAGS) -DNO_MEMTRACK $(TEST_DIR)/test1_no_leak.c      -o $(BUILD_DIR)/test1_valgrind -lpthread
	$(CC) $(CFLAGS) -DNO_MEMTRACK $(TEST_DIR)/test2_single_leak.c  -o $(BUILD_DIR)/test2_valgrind -lpthread
	$(CC) $(CFLAGS) -DNO_MEMTRACK $(TEST_DIR)/test3_multi_leak.c   -o $(BUILD_DIR)/test3_valgrind -lpthread
	$(CC) $(CFLAGS) -DNO_MEMTRACK $(TEST_DIR)/test4_large_leak.c   -o $(BUILD_DIR)/test4_valgrind -lpthread
	$(CC) $(CFLAGS) -DNO_MEMTRACK $(TEST_DIR)/test5_invalid_free.c -o $(BUILD_DIR)/test5_valgrind -lpthread
	$(CC) $(CFLAGS) -DNO_MEMTRACK $(TEST_DIR)/test6_multithread.c  -o $(BUILD_DIR)/test6_valgrind -lpthread
	@echo "  All tests built in $(BUILD_DIR)/"

# ── Run all tests ───────────────────────────────────────────
.PHONY: run-tests
run-tests: dirs tests
	LD_LIBRARY_PATH=./$(BUILD_DIR) bash tests/run_tests.sh

# ── Run samples ─────────────────────────────────────────────
.PHONY: run-samples
run-samples: samples
	@echo ""; echo "── Sample A (expect: no leaks) ──────────────"
	LD_LIBRARY_PATH=./$(BUILD_DIR) ./$(BUILD_DIR)/sample_a
	@echo ""; echo "── Sample B (expect: 5 SMALL leaks) ─────────"
	LD_LIBRARY_PATH=./$(BUILD_DIR) ./$(BUILD_DIR)/sample_b
	@echo ""; echo "── Sample C (expect: 1 LARGE leak) ──────────"
	LD_LIBRARY_PATH=./$(BUILD_DIR) ./$(BUILD_DIR)/sample_c

# ── Clean ───────────────────────────────────────────────────
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(LOG_DIR)
	@echo "  Cleaned build/ and logs/ directories."

# ── Help ────────────────────────────────────────────────────
.PHONY: help
help:
	@echo ""
	@echo "  Memory Leak Detection Tool — Build Targets"
	@echo "  ─────────────────────────────────────────────"
	@echo "  make              Build the shared library only"
	@echo "  make samples      Build all sample programs (linked mode)"
	@echo "  make tests        Build all test programs (linked mode & valgrind mode)"
	@echo "  make run-samples  Build + run all sample programs"
	@echo "  make run-tests    Build + run all tests via script"
	@echo "  make clean        Remove build/ and logs/ directories"
	@echo ""
	@echo "  Linked mode (recommended):"
	@echo "    LD_LIBRARY_PATH=./build ./your_program"
	@echo ""
	@echo "  Preload mode (external programs):"
	@echo "    LD_PRELOAD=./build/libmemtrack.so ./your_program"
	@echo ""
	@echo "  With log file:"
	@echo "    MEMTRACK_LOG=logs/leak.txt LD_LIBRARY_PATH=./build ./your_program"
	@echo ""