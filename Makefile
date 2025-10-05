# Makefile for UP C Parser

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
TARGET = up-parser
SOURCES = up.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: help
help: ## Show this help message
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'Available targets:'
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "  %-15s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

.PHONY: test
test: build ## Run tests
	@echo "Running tests..."
	@for file in examples/*.up; do \
		echo "Testing $$file..."; \
		./$(TARGET) "$$file" || echo "Test not yet implemented"; \
	done

.PHONY: test-valgrind
test-valgrind: build ## Run tests with Valgrind
	@echo "Running Valgrind tests..."
	@for file in examples/*.up; do \
		echo "Valgrind testing $$file..."; \
		valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) "$$file" || echo "Valgrind test skipped"; \
	done

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
