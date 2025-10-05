/**
 * UP Parser for C - Implementation
 * Structured Notation for Annotated Properties
 */

#include "up.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 8
#define MAX_ERROR_LEN 256

static char error_message[MAX_ERROR_LEN] = {0};

/* Helper functions */
static void set_error(const char *msg) {
    strncpy(error_message, msg, MAX_ERROR_LEN - 1);
    error_message[MAX_ERROR_LEN - 1] = '\0';
}

const char *up_get_error(void) {
    return error_message[0] ? error_message : NULL;
}

static char *string_duplicate(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char *dup = malloc(len + 1);
    if (dup) {
        memcpy(dup, str, len + 1);
    }
    return dup;
}

static char *string_trim(const char *str) {
    while (isspace(*str)) str++;
    const char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    size_t len = end - str + 1;
    char *trimmed = malloc(len + 1);
    if (trimmed) {
        memcpy(trimmed, str, len);
        trimmed[len] = '\0';
    }
    return trimmed;
}

static char **split_lines(const char *input, size_t *count) {
    size_t capacity = INITIAL_CAPACITY;
    char **lines = malloc(sizeof(char*) * capacity);
    *count = 0;

    const char *start = input;
    const char *ptr = input;

    while (*ptr) {
        if (*ptr == '\n' || *ptr == '\r') {
            size_t len = ptr - start;
            char *line = malloc(len + 1);
            memcpy(line, start, len);
            line[len] = '\0';

            if (*count >= capacity) {
                capacity *= 2;
                lines = realloc(lines, sizeof(char*) * capacity);
            }
            lines[(*count)++] = line;

            if (*ptr == '\r' && *(ptr + 1) == '\n') ptr++;
            start = ptr + 1;
        }
        ptr++;
    }

    /* Last line */
    if (start < ptr) {
        size_t len = ptr - start;
        char *line = malloc(len + 1);
        memcpy(line, start, len);
        line[len] = '\0';

        if (*count >= capacity) {
            capacity *= 2;
            lines = realloc(lines, sizeof(char*) * capacity);
        }
        lines[(*count)++] = line;
    }

    return lines;
}

static void free_lines(char **lines, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}

/* Value constructors */
static up_value_t *up_value_new_string(const char *str) {
    up_value_t *val = calloc(1, sizeof(up_value_t));
    val->type = UP_VALUE_STRING;
    val->data.string_val.data = string_duplicate(str);
    val->data.string_val.length = strlen(str);
    return val;
}

static up_value_t *up_value_new_block(void) {
    up_value_t *val = calloc(1, sizeof(up_value_t));
    val->type = UP_VALUE_BLOCK;
    val->data.block_val.capacity = INITIAL_CAPACITY;
    val->data.block_val.nodes = calloc(INITIAL_CAPACITY, sizeof(up_node_t*));
    return val;
}

static up_value_t *up_value_new_list(void) {
    up_value_t *val = calloc(1, sizeof(up_value_t));
    val->type = UP_VALUE_LIST;
    val->data.list_val.capacity = INITIAL_CAPACITY;
    val->data.list_val.values = calloc(INITIAL_CAPACITY, sizeof(up_value_t*));
    return val;
}

/* Forward declarations */
static up_node_t *parse_line(up_parser_t *parser);
static up_value_t *parse_value(up_parser_t *parser, const char *value_part, const char *type_anno);
static up_value_t *parse_multiline(up_parser_t *parser, const char *first_line, const char *type_anno);
static up_value_t *parse_block(up_parser_t *parser);
static up_value_t *parse_list(up_parser_t *parser);

/* Node functions */
static up_node_t *up_node_new(const char *key, const char *type_anno, up_value_t *value) {
    up_node_t *node = calloc(1, sizeof(up_node_t));
    node->key = string_duplicate(key);
    node->type_annotation = type_anno ? string_duplicate(type_anno) : NULL;
    node->value = value;
    return node;
}

void up_node_free(up_node_t *node) {
    if (!node) return;
    free(node->key);
    free(node->type_annotation);
    up_value_free(node->value);
    free(node);
}

void up_value_free(up_value_t *value) {
    if (!value) return;

    switch (value->type) {
        case UP_VALUE_STRING:
            free(value->data.string_val.data);
            break;
        case UP_VALUE_BLOCK:
            for (size_t i = 0; i < value->data.block_val.count; i++) {
                up_node_free(value->data.block_val.nodes[i]);
            }
            free(value->data.block_val.nodes);
            break;
        case UP_VALUE_LIST:
            for (size_t i = 0; i < value->data.list_val.count; i++) {
                up_value_free(value->data.list_val.values[i]);
            }
            free(value->data.list_val.values);
            break;
        case UP_VALUE_TABLE:
            for (size_t i = 0; i < value->data.table_val.column_count; i++) {
                free(value->data.table_val.columns[i]);
            }
            free(value->data.table_val.columns);
            for (size_t i = 0; i < value->data.table_val.row_count; i++) {
                for (size_t j = 0; j < value->data.table_val.column_count; j++) {
                    free(value->data.table_val.rows[i][j]);
                }
                free(value->data.table_val.rows[i]);
            }
            free(value->data.table_val.rows);
            break;
    }
    free(value);
}

/* Document functions */
up_document_t *up_document_new(void) {
    up_document_t *doc = calloc(1, sizeof(up_document_t));
    doc->capacity = INITIAL_CAPACITY;
    doc->nodes = calloc(INITIAL_CAPACITY, sizeof(up_node_t*));
    return doc;
}

void up_document_free(up_document_t *doc) {
    if (!doc) return;
    for (size_t i = 0; i < doc->count; i++) {
        up_node_free(doc->nodes[i]);
    }
    free(doc->nodes);
    free(doc);
}

static void document_add_node(up_document_t *doc, up_node_t *node) {
    if (doc->count >= doc->capacity) {
        doc->capacity *= 2;
        doc->nodes = realloc(doc->nodes, sizeof(up_node_t*) * doc->capacity);
    }
    doc->nodes[doc->count++] = node;
}

/* Parser */
static up_node_t *parse_line(up_parser_t *parser) {
    if (parser->line_num >= parser->line_count) return NULL;

    char *line = string_trim(parser->lines[parser->line_num]);
    if (!line || line[0] == '\0' || line[0] == '#') {
        parser->line_num++;
        free(line);
        return NULL;
    }

    /* Split key and value */
    char *space = strchr(line, ' ');
    char *tab = strchr(line, '\t');
    char *sep = space;
    if (tab && (!sep || tab < sep)) sep = tab;

    char *key_part = line;
    char *value_part = "";

    if (sep) {
        *sep = '\0';
        value_part = string_trim(sep + 1);
    }

    /* Parse type annotation */
    char *bang = strchr(key_part, '!');
    char *key = key_part;
    char *type_anno = NULL;

    if (bang) {
        *bang = '\0';
        type_anno = bang + 1;
    }

    parser->line_num++;
    up_value_t *value = parse_value(parser, value_part, type_anno);

    up_node_t *node = up_node_new(key, type_anno, value);
    free(line);
    if (sep && value_part != sep + 1) free(value_part);

    return node;
}

static up_value_t *parse_value(up_parser_t *parser, const char *value_part, const char *type_anno) {
    if (strncmp(value_part, "```", 3) == 0) {
        return parse_multiline(parser, value_part, type_anno);
    } else if (strcmp(value_part, "{") == 0) {
        return parse_block(parser);
    } else if (strcmp(value_part, "[") == 0) {
        return parse_list(parser);
    } else {
        return up_value_new_string(value_part);
    }
}

static up_value_t *parse_multiline(up_parser_t *parser, const char *first_line, const char *type_anno) {
    /* Collect lines until ``` */
    char *content = NULL;
    size_t content_size = 0;
    size_t content_capacity = 1024;
    content = malloc(content_capacity);
    content[0] = '\0';

    while (parser->line_num < parser->line_count) {
        char *line = parser->lines[parser->line_num];
        char *trimmed = string_trim(line);

        if (strcmp(trimmed, "```") == 0) {
            free(trimmed);
            parser->line_num++;
            break;
        }
        free(trimmed);

        size_t line_len = strlen(line);
        if (content_size + line_len + 2 >= content_capacity) {
            content_capacity *= 2;
            content = realloc(content, content_capacity);
        }

        if (content_size > 0) {
            strcat(content, "\n");
            content_size++;
        }
        strcat(content, line);
        content_size += line_len;
        parser->line_num++;
    }

    up_value_t *val = up_value_new_string(content);
    free(content);
    return val;
}

static up_value_t *parse_block(up_parser_t *parser) {
    up_value_t *block_val = up_value_new_block();
    up_block_t *block = &block_val->data.block_val;

    while (parser->line_num < parser->line_count) {
        char *line = string_trim(parser->lines[parser->line_num]);

        if (strcmp(line, "}") == 0) {
            free(line);
            parser->line_num++;
            break;
        }

        if (line[0] == '\0' || line[0] == '#') {
            free(line);
            parser->line_num++;
            continue;
        }

        free(line);
        up_node_t *node = parse_line(parser);
        if (node) {
            if (block->count >= block->capacity) {
                block->capacity *= 2;
                block->nodes = realloc(block->nodes, sizeof(up_node_t*) * block->capacity);
            }
            block->nodes[block->count++] = node;
        }
    }

    return block_val;
}

static up_value_t *parse_list(up_parser_t *parser) {
    up_value_t *list_val = up_value_new_list();
    up_list_t *list = &list_val->data.list_val;

    while (parser->line_num < parser->line_count) {
        char *line = string_trim(parser->lines[parser->line_num]);

        if (strcmp(line, "]") == 0) {
            free(line);
            parser->line_num++;
            break;
        }

        if (line[0] == '\0' || line[0] == '#') {
            free(line);
            parser->line_num++;
            continue;
        }

        up_value_t *item = up_value_new_string(line);
        free(line);

        if (list->count >= list->capacity) {
            list->capacity *= 2;
            list->values = realloc(list->values, sizeof(up_value_t*) * list->capacity);
        }
        list->values[list->count++] = item;
    }

    return list_val;
}

up_document_t *up_parse_string(const char *input) {
    up_parser_t parser;
    parser.lines = split_lines(input, &parser.line_count);
    parser.line_num = 0;

    up_document_t *doc = up_document_new();

    while (parser.line_num < parser.line_count) {
        up_node_t *node = parse_line(&parser);
        if (node) {
            document_add_node(doc, node);
        }
    }

    free_lines(parser.lines, parser.line_count);
    return doc;
}

up_document_t *up_parse_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        set_error("Failed to open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);

    up_document_t *doc = up_parse_string(content);
    free(content);
    return doc;
}

up_node_t *up_document_get(up_document_t *doc, const char *key) {
    for (size_t i = 0; i < doc->count; i++) {
        if (strcmp(doc->nodes[i]->key, key) == 0) {
            return doc->nodes[i];
        }
    }
    return NULL;
}

up_value_t *up_block_get(up_block_t *block, const char *key) {
    for (size_t i = 0; i < block->count; i++) {
        if (strcmp(block->nodes[i]->key, key) == 0) {
            return block->nodes[i]->value;
        }
    }
    return NULL;
}

