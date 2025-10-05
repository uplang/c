# Quick Start Guide - C

Get started with the UP Parser for C in 5 minutes!

## Installation

```bash
git clone https://github.com/uplang/c
cd c
make
sudo make install
```

## Your First Program

Create `example.c`:

```c
#include <up.h>
#include <stdio.h>

int main() {
    up_parser_t *parser = up_parser_new();
    
    const char *input = 
        "name Alice\n"
        "age!int 30\n"
        "active!bool true\n";
    
    up_document_t *doc = up_parse_string(parser, input);
    if (!doc) {
        fprintf(stderr, "Parse error\n");
        up_parser_free(parser);
        return 1;
    }
    
    printf("Name: %s\n", up_doc_get_scalar(doc, "name"));
    printf("Age: %s\n", up_doc_get_scalar(doc, "age"));
    
    // Iterate nodes
    for (size_t i = 0; i < doc->node_count; i++) {
        up_node_t *node = &doc->nodes[i];
        printf("%s = %s\n", node->key, node->value.scalar);
    }
    
    up_document_free(doc);
    up_parser_free(parser);
    
    return 0;
}
```

Compile and run:

```bash
gcc example.c -lup -o example
./example
```

## Common Use Cases

### 1. Configuration Files

```c
#include <up.h>

FILE *f = fopen("config.up", "r");
if (!f) return 1;

up_parser_t *parser = up_parser_new();
up_document_t *doc = up_parse_file(parser, f);
fclose(f);

const char *host = up_doc_get_scalar(doc, "host");
printf("Host: %s\n", host);

up_document_free(doc);
up_parser_free(parser);
```

### 2. Error Handling

```c
up_document_t *doc = up_parse_string(parser, input);
if (!doc) {
    up_error_t *err = up_parser_get_error(parser);
    fprintf(stderr, "Error at line %d: %s\n", 
            err->line, err->message);
    return 1;
}
```

## Next Steps

- Read the [DESIGN.md](DESIGN.md) for implementation details
- Explore the [UP Specification](https://github.com/uplang/spec)
- Check out the header file `include/up.h`

## Need Help?

- 📚 [Full Documentation](README.md)
- 💬 [Discussions](https://github.com/uplang/spec/discussions)
- 🐛 [Report Issues](https://github.com/uplang/c/issues)
