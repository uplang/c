# Design Documentation - C Implementation

This document describes the architecture and design decisions of the C UP parser implementation.

## Overview

The C implementation prioritizes:

- **Portability** - ANSI C compatible
- **Performance** - Minimal overhead
- **Memory Safety** - Explicit resource management
- **Simplicity** - Clear, readable code
- **Minimal Dependencies** - Only standard library

## Architecture

### Data Structures

```c
typedef struct up_document {
    up_node_t *nodes;
    size_t node_count;
    size_t node_capacity;
} up_document_t;

typedef struct up_node {
    char *key;
    char *type;  // NULL if not specified
    up_value_t value;
} up_node_t;

typedef enum {
    UP_VALUE_SCALAR,
    UP_VALUE_BLOCK,
    UP_VALUE_LIST,
    UP_VALUE_TABLE,
    UP_VALUE_MULTILINE
} up_value_type_t;

typedef struct up_value {
    up_value_type_t type;
    union {
        char *scalar;
        up_block_t *block;
        up_list_t *list;
        up_table_t *table;
    };
} up_value_t;
```

### Memory Management

```c
// Allocate
up_parser_t *parser = up_parser_new();
up_document_t *doc = up_parse_string(parser, input);

// Use
const char *name = up_doc_get_scalar(doc, "name");

// Free
up_document_free(doc);
up_parser_free(parser);
```

**Design Rationale:**
- Explicit allocation/deallocation
- No hidden memory allocation
- Clear ownership
- Valgrind clean

## Parser Implementation

### Single-Pass Parsing

```c
up_document_t* up_parse_string(up_parser_t *parser, const char *input) {
    up_document_t *doc = up_document_new();
    
    // Line-by-line parsing
    char *line;
    while ((line = read_line(input)) != NULL) {
        if (is_empty(line) || is_comment(line)) {
            continue;
        }
        
        up_node_t node = parse_line(line);
        up_document_add_node(doc, &node);
    }
    
    return doc;
}
```

## Error Handling

### Error Structure

```c
typedef struct up_error {
    int line;
    char *message;
} up_error_t;

// Check for errors
up_document_t *doc = up_parse_string(parser, input);
if (!doc) {
    up_error_t *err = up_parser_get_error(parser);
    fprintf(stderr, "Line %d: %s\n", err->line, err->message);
}
```

## Performance

- **Zero allocations** in hot path where possible
- **Stack buffers** for small strings
- **Reuse buffers** in parser state
- **Single pass** parsing

## Design Decisions

### Why Opaque Pointers?

**Pros:**
- ABI stability
- Encapsulation
- Forward compatibility

**Decision:** Better API design

### Why Tagged Union?

**Pros:**
- Memory efficient
- Type-safe access
- C idiom

**Decision:** Standard C pattern

## Contributing

When contributing:

1. **ANSI C** - Compatible with C89
2. **No leaks** - Valgrind clean
3. **Tests** - Unit tests for all features
4. **Documentation** - Comment all public APIs

## References

- [UP Specification](https://github.com/uplang/spec)
- [C Coding Standards](https://www.kernel.org/doc/html/latest/process/coding-style.html)
