/**
 * UP (Unified Properties) parser for C
 * Using C17 standard with modern idioms
 */

#ifndef UP_H
#define UP_H

#include <stdbool.h>
#include <stddef.h>

// Forward declarations
typedef struct up_value up_value_t;
typedef struct up_node up_node_t;
typedef struct up_document up_document_t;
typedef struct up_parser up_parser_t;

// Value types
typedef enum {
    UP_TYPE_STRING,
    UP_TYPE_BLOCK,
    UP_TYPE_LIST
} up_value_type_t;

// String value
typedef struct {
    char *data;
    size_t length;
} up_string_t;

// Block value (key-value map)
typedef struct {
    char **keys;
    up_value_t **values;
    size_t count;
    size_t capacity;
} up_block_t;

// List value
typedef struct {
    up_value_t **items;
    size_t count;
    size_t capacity;
} up_list_t;

// Value (tagged union)
struct up_value {
    up_value_type_t type;
    union {
        up_string_t string;
        up_block_t block;
        up_list_t list;
    } as;
};

// Node (key-value with optional type annotation)
struct up_node {
    char *key;
    char *type_annotation;  // NULL if no type
    up_value_t *value;
};

// Document (collection of nodes)
struct up_document {
    up_node_t **nodes;
    size_t count;
    size_t capacity;
};

// Parser state
struct up_parser {
    const char **lines;
    size_t line_count;
    size_t current_line;
};

// API functions
up_parser_t *up_parser_new(void);
void up_parser_free(up_parser_t *parser);

up_document_t *up_parse(const char *input);
up_document_t *up_parse_string(const char *input);
up_document_t *up_parser_parse_document(up_parser_t *parser, const char *input);

const char *up_get_error(void);
up_node_t *up_document_get(up_document_t *doc, const char *key);

void up_document_free(up_document_t *doc);
bool up_document_is_empty(const up_document_t *doc);
size_t up_document_size(const up_document_t *doc);

void up_node_free(up_node_t *node);
void up_value_free(up_value_t *value);

// Value constructors
up_value_t *up_value_new_string(const char *str);
up_value_t *up_value_new_block(void);
up_value_t *up_value_new_list(void);

// Block operations
void up_block_set(up_block_t *block, const char *key, up_value_t *value);
up_value_t *up_block_get(const up_block_t *block, const char *key);

// List operations
void up_list_append(up_list_t *list, up_value_t *value);

#endif // UP_H
