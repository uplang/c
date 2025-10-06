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
        case UP_TYPE_STRING:
            printf("%s\n", value->as.string.data);
            break;
        case UP_TYPE_BLOCK:
            printf("{\n");
            for (size_t i = 0; i < value->as.block.count; i++) {
                print_indent(indent + 1);
                printf("%s: ", value->as.block.keys[i]);
                print_value(value->as.block.values[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        case UP_TYPE_LIST:
            printf("[\n");
            for (size_t i = 0; i < value->as.list.count; i++) {
                print_indent(indent + 1);
                print_value(value->as.list.items[i], indent + 1);
            }
            print_indent(indent);
            printf("]\n");
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
    if (name_node && name_node->value->type == UP_TYPE_STRING) {
        printf("Name lookup: %s\n", name_node->value->as.string.data);
    }

    up_document_free(doc);

    printf("\nSuccess!\n");
    return 0;
}

