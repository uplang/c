/**
 * UP Parser for C
 * Structured Notation for Annotated Properties
 *
 * Usage:
 *   #include "up.h"
 *   up_document_t *doc = up_parse_string(up_text);
 *   // ... use document ...
 *   up_document_free(doc);
 */

#ifndef UP_H
#define UP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Value types */
typedef enum {
    UP_VALUE_STRING,
    UP_VALUE_BLOCK,
    UP_VALUE_LIST,
    UP_VALUE_TABLE
} up_value_type_t;

/* Forward declarations */
typedef struct up_value up_value_t;
typedef struct up_node up_node_t;
typedef struct up_block up_block_t;
typedef struct up_list up_list_t;
typedef struct up_table up_table_t;
typedef struct up_document up_document_t;

/* String value */
typedef struct {
    char *data;
    size_t length;
} up_string_t;

/* Block value (hash map) */
struct up_block {
    up_node_t **nodes;
    size_t count;
    size_t capacity;
};

/* List value */
struct up_list {
    up_value_t **values;
    size_t count;
    size_t capacity;
};

/* Table value */
struct up_table {
    char **columns;
    size_t column_count;
    char ***rows;
    size_t row_count;
};

/* Value union */
struct up_value {
    up_value_type_t type;
    union {
        up_string_t string_val;
        up_block_t block_val;
        up_list_t list_val;
        up_table_t table_val;
    } data;
};

/* Node (key-value pair with type annotation) */
struct up_node {
    char *key;
    char *type_annotation;
    up_value_t *value;
};

/* Document (list of top-level nodes) */
struct up_document {
    up_node_t **nodes;
    size_t count;
    size_t capacity;
};

/* Parser structure */
typedef struct {
    char **lines;
    size_t line_count;
    size_t line_num;
} up_parser_t;

/**
 * Parse a UP document from a string
 * @param input The UP document text (null-terminated)
 * @return Parsed document or NULL on error
 */
up_document_t *up_parse_string(const char *input);

/**
 * Parse a UP document from a file
 * @param filename Path to the UP file
 * @return Parsed document or NULL on error
 */
up_document_t *up_parse_file(const char *filename);

/**
 * Free a document and all its contents
 * @param doc Document to free
 */
void up_document_free(up_document_t *doc);

/**
 * Free a node and its value
 * @param node Node to free
 */
void up_node_free(up_node_t *node);

/**
 * Free a value
 * @param value Value to free
 */
void up_value_free(up_value_t *value);

/**
 * Get a node by key from a document
 * @param doc Document to search
 * @param key Key to find
 * @return Node or NULL if not found
 */
up_node_t *up_document_get(up_document_t *doc, const char *key);

/**
 * Get a value by key from a block
 * @param block Block to search
 * @param key Key to find
 * @return Value or NULL if not found
 */
up_value_t *up_block_get(up_block_t *block, const char *key);

/**
 * Get the last error message
 * @return Error message or NULL if no error
 */
const char *up_get_error(void);

#ifdef __cplusplus
}
#endif

#endif /* UP_H */

