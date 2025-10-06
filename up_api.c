/**
 * UP API implementation
 * Stub implementations for missing functions
 */

#include "up.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global error message
static char error_message[256] = "";

// Stub lexer function (required by yacc/bison)
int yylex(void) {
    return 0; // EOF
}

// Parse a string and return a document
up_document_t *up_parse_string(const char *input) {
    (void)input; // Unused for now

    // Return a stub document for now
    up_document_t *doc = malloc(sizeof(up_document_t));
    if (!doc) {
        snprintf(error_message, sizeof(error_message), "Memory allocation failed");
        return NULL;
    }

    doc->nodes = NULL;
    doc->count = 0;
    doc->capacity = 0;

    return doc;
}

// Get the last error message
const char *up_get_error(void) {
    return error_message[0] ? error_message : "Unknown error";
}

// Get a node from the document by key
up_node_t *up_document_get(up_document_t *doc, const char *key) {
    if (!doc || !key) {
        return NULL;
    }

    for (size_t i = 0; i < doc->count; i++) {
        if (doc->nodes[i] && doc->nodes[i]->key &&
            strcmp(doc->nodes[i]->key, key) == 0) {
            return doc->nodes[i];
        }
    }

    return NULL;
}

// Free a document
void up_document_free(up_document_t *doc) {
    if (!doc) {
        return;
    }

    for (size_t i = 0; i < doc->count; i++) {
        up_node_free(doc->nodes[i]);
    }

    free(doc->nodes);
    free(doc);
}

// Free a node
void up_node_free(up_node_t *node) {
    if (!node) {
        return;
    }

    free(node->key);
    free(node->type_annotation);
    up_value_free(node->value);
    free(node);
}

// Free a value
void up_value_free(up_value_t *value) {
    if (!value) {
        return;
    }

    switch (value->type) {
        case UP_TYPE_STRING:
            free(value->as.string.data);
            break;
        case UP_TYPE_BLOCK:
            for (size_t i = 0; i < value->as.block.count; i++) {
                free(value->as.block.keys[i]);
                up_value_free(value->as.block.values[i]);
            }
            free(value->as.block.keys);
            free(value->as.block.values);
            break;
        case UP_TYPE_LIST:
            for (size_t i = 0; i < value->as.list.count; i++) {
                up_value_free(value->as.list.items[i]);
            }
            free(value->as.list.items);
            break;
    }

    free(value);
}

