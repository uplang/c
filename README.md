# UP Parser for C

[![CI](https://github.com/uplang/c/workflows/CI/badge.svg)](https://github.com/uplang/c/actions)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://uplang.github.io/c/)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Official C implementation of the UP (Unified Properties) language parser.

📚 **[API Documentation](https://uplang.github.io/c/)** | 🧪 **[Test Status](https://github.com/uplang/c/actions)** | 📖 **[Specification](https://github.com/uplang/spec)**

> **Portable & Fast** - ANSI C compatible, minimal dependencies

## Features

- ✅ **Full UP Syntax Support** - Scalars, blocks, lists, tables, multiline strings
- ✅ **Type Annotations** - Parse and preserve type hints (`!int`, `!bool`, etc.)
- ✅ **ANSI C Compatible** - Works with C89 and later
- ✅ **Portable** - Cross-platform (Linux, macOS, Windows, BSD)
- ✅ **Well-Tested** - Comprehensive test suite
- ✅ **Minimal Dependencies** - Only standard library
- ✅ **CLI Tool** - Command-line utility included

## Requirements

- C compiler (GCC, Clang, MSVC)
- make (or cmake)

## Installation

### Build from Source

```bash
git clone https://github.com/uplang/c
cd c
make
sudo make install
```

### CMake

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

## Quick Start

```c
#include <up.h>
#include <stdio.h>

int main() {
    up_parser_t *parser = up_parser_new();

    const char *input =
        "name Alice\n"
        "age!int 30\n";

    up_document_t *doc = up_parse_string(parser, input);
    if (!doc) {
        fprintf(stderr, "Parse error\n");
        return 1;
    }

    // Access values
    const char *name = up_doc_get_scalar(doc, "name");
    printf("Name: %s\n", name);

    // Cleanup
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

**📖 For detailed examples and tutorials, see [QUICKSTART.md](QUICKSTART.md)**

## Documentation

- **[QUICKSTART.md](QUICKSTART.md)** - Getting started guide with examples
- **[DESIGN.md](DESIGN.md)** - Architecture and design decisions
- **[UP Specification](https://github.com/uplang/spec)** - Complete language specification

## API Overview

### Core Types

- **`up_parser_t`** - Opaque parser handle
- **`up_document_t`** - Parsed document handle
- **`up_node_t`** - Key-value pair structure
- **`up_value_t`** - Tagged union for value types

### Basic Usage

```c
#include <up.h>

// Create parser
up_parser_t *parser = up_parser_new();

// Parse from string
up_document_t *doc = up_parse_string(parser, content);

// Parse from file
FILE *f = fopen("config.up", "r");
up_document_t *doc = up_parse_file(parser, f);
fclose(f);

// Access values
const char *name = up_doc_get_scalar(doc, "name");

// Cleanup
up_document_free(doc);
up_parser_free(parser);
```

**See [DESIGN.md](DESIGN.md) for complete API documentation and implementation details.**

## CLI Tool

```bash
# Parse and display
up-parse config.up

# Validate syntax
up-validate config.up

# Convert to JSON
up-convert config.up --format json
```

## Testing

```bash
# Run all tests
make test

# Run with valgrind (memory leak detection)
make test-valgrind

# Run specific test
./tests/test_parser
```

## Project Structure

```
c/
├── include/
│   └── up.h             # Public API header
├── src/
│   ├── parser.c         # Parser implementation
│   ├── types.c          # Data structures
│   ├── memory.c         # Memory management
│   └── cli.c            # CLI tool
├── tests/
│   └── test_parser.c    # Tests
├── Makefile
├── CMakeLists.txt
├── README.md            # This file
├── QUICKSTART.md        # Getting started guide
├── DESIGN.md            # Architecture documentation
└── LICENSE              # GNU GPLv3
```

## Contributing

Contributions are welcome! Please see the main [CONTRIBUTING.md](https://github.com/uplang/spec/blob/main/CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Links

- **[UP Language Specification](https://github.com/uplang/spec)** - Official language spec
- **[Syntax Reference](https://github.com/uplang/spec/blob/main/SYNTAX-REFERENCE.md)** - Quick syntax guide
- **[UP Namespaces](https://github.com/uplang/ns)** - Official namespace plugins

### Other Implementations

- **[Go](https://github.com/uplang/go)** - Reference implementation
- **[Java](https://github.com/uplang/java)** - Modern Java 21+ with records and sealed types
- **[JavaScript/TypeScript](https://github.com/uplang/js)** - Browser and Node.js support
- **[Python](https://github.com/uplang/py)** - Pythonic implementation with dataclasses
- **[Rust](https://github.com/uplang/rust)** - Zero-cost abstractions and memory safety

## Support

- **Issues**: [github.com/uplang/c/issues](https://github.com/uplang/c/issues)
- **Discussions**: [github.com/uplang/spec/discussions](https://github.com/uplang/spec/discussions)
