# rm-toto — POSIX-oriented Makefile (gcc or clang on Unix, MSYS2 UCRT64, etc.)
#
# Compiler flag rationale (release CFLAGS):
#   -std=c11          ISO C11 baseline requested by the project spec.
#   -O2               Strong optimisation without -O3’s aggressive trade-offs.
#   -Wall -Wextra     Enable most warnings.
#   -Wpedantic        Reject common extensions and dubious constructs.
#   -Werror           Treat warnings as build-breaking (zero-warning policy).
#   -D_POSIX_C_SOURCE=200809L  Expose POSIX.1-2008 (lstat, unlink, rmdir, …).

CC := $(shell command -v gcc >/dev/null 2>&1 && echo gcc || echo clang)

CFLAGS_COMMON := -std=c11 -Wall -Wextra -Wpedantic -Werror \
	-D_POSIX_C_SOURCE=200809L -Iinclude

CFLAGS := $(CFLAGS_COMMON) -O2

# Sanitizers need matching compile+link flags.
CFLAGS_DEBUG := $(CFLAGS_COMMON) -g -O1 -fsanitize=address,undefined \
	-fno-omit-frame-pointer

BIN_DIR := build
TEST_DIR := build/tests

TARGET := $(BIN_DIR)/rm-toto
TARGET_DEBUG := $(BIN_DIR)/rm-toto-debug
TEST_BIN := $(TEST_DIR)/test_core

MAIN_SRCS := src/main.c \
	src/rm_toto_emit.c \
	src/rm_toto_walk.c \
	src/rm_toto_prompt.c \
	src/rm_toto_cli.c

HDRS := $(wildcard include/*.h)

TEST_SRCS := tests/test_runner.c \
	tests/test_join_path_basic.c \
	tests/test_join_path_truncation.c \
	tests/test_join_path_trailing_slash.c \
	tests/test_join_path_empty_name.c \
	tests/test_is_root_path.c

.PHONY: all debug test clean install

all: $(TARGET)

$(BIN_DIR) $(TEST_DIR):
	mkdir -p $@

$(TARGET): $(MAIN_SRCS) $(HDRS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(MAIN_SRCS)

debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(MAIN_SRCS) $(HDRS) | $(BIN_DIR)
	$(CC) $(CFLAGS_DEBUG) -o $@ $(MAIN_SRCS)

$(TEST_BIN): $(TEST_SRCS) $(HDRS) $(wildcard tests/*.h) | $(TEST_DIR)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRCS)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(TARGET) $(TARGET_DEBUG) $(TEST_BIN)
	rm -f $(TARGET).exe $(TARGET_DEBUG).exe $(TEST_BIN).exe
	rm -f $(BIN_DIR)/*.o

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/rm-toto
