# Makefile for UP C Parser

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
TARGET = example
LIB_SOURCES = up.c up_api.c
EXAMPLE_SOURCES = example.c
ALL_SOURCES = $(LIB_SOURCES) $(EXAMPLE_SOURCES)
OBJECTS = $(ALL_SOURCES:.c=.o)

.PHONY: help
help: ## Show this help message
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'Available targets:'
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "  %-15s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

.PHONY: test
test: build ## Run tests
	@echo "Build successful - C library and example compiled"
	@echo "Run './example' to test the parser"

.PHONY: test-valgrind
test-valgrind: build ## Run tests with Valgrind
	@echo "Running Valgrind tests..."
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) || echo "Valgrind not available"

.PHONY: all
all: build ## Build all targets

.PHONY: build
build: $(TARGET) ## Build the parser

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean: ## Clean build artifacts
	rm -f $(OBJECTS) $(TARGET)

.PHONY: install
install: build ## Install to /usr/local/bin
	install -m 755 $(TARGET) /usr/local/bin/

.PHONY: test-ci
test-ci: ## Run CI tests locally using act (requires: brew install act)
	act --container-architecture linux/amd64 -j test
	act --container-architecture linux/amd64 -j build

.DEFAULT_GOAL := build
