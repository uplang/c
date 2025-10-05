/**
 * Example usage of UP C parser
 */

#include "up.h"
#include <stdio.h>
#include <stdlib.h>

static void print_value(up_value_t *value, int indent);

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static void print_value(up_value_t *value, int indent) {
    switch (value->type) {
        case UP_VALUE_STRING:
            printf("%s\n", value->data.string_val.data);
            break;
        case UP_VALUE_BLOCK:
            printf("{\n");
            for (size_t i = 0; i < value->data.block_val.count; i++) {
                up_node_t *node = value->data.block_val.nodes[i];
                print_indent(indent + 1);
                printf("%s", node->key);
                if (node->type_annotation) {
                    printf("!%s", node->type_annotation);
                }
                printf(": ");
                print_value(node->value, indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        case UP_VALUE_LIST:
            printf("[\n");
            for (size_t i = 0; i < value->data.list_val.count; i++) {
                print_indent(indent + 1);
                print_value(value->data.list_val.values[i], indent + 1);
            }
            print_indent(indent);
            printf("]\n");
            break;
        case UP_VALUE_TABLE:
            printf("Table with %zu columns and %zu rows\n",
                   value->data.table_val.column_count,
                   value->data.table_val.row_count);
            break;
    }
}

int main(void) {
    const char *up_text =
        "name John Doe\n"
        "age!int 30\n"
        "active!bool true\n"
        "\n"
        "server {\n"
        "  host localhost\n"
        "  port!int 8080\n"
        "  debug!bool false\n"
        "}\n"
        "\n"
        "items [\n"
        "apple\n"
        "banana\n"
        "cherry\n"
        "]\n";

    printf("Parsing UP document...\n\n");

    up_document_t *doc = up_parse_string(up_text);
    if (!doc) {
        fprintf(stderr, "Parse error: %s\n", up_get_error());
        return 1;
    }

    printf("Parsed %zu top-level nodes:\n\n", doc->count);

    for (size_t i = 0; i < doc->count; i++) {
        up_node_t *node = doc->nodes[i];
        printf("%s", node->key);
        if (node->type_annotation) {
            printf("!%s", node->type_annotation);
        }
        printf(": ");
        print_value(node->value, 0);
        printf("\n");
    }

    /* Test lookup */
    up_node_t *name_node = up_document_get(doc, "name");
    if (name_node && name_node->value->type == UP_VALUE_STRING) {
        printf("Name lookup: %s\n", name_node->value->data.string_val.data);
    }

    up_document_free(doc);

    printf("\nSuccess!\n");
    return 0;
}

