# utilwin

A collection of POSIX/Unix utility functions for Windows, providing cross-platform compatibility for common operations.

## Features

- **dirent**: Directory entry functions (`opendir`, `readdir`, `closedir`, `scandir`, etc.)
- **process**: Process management utilities (`fork`, `socketpair`, UTF-8 support, etc.)
- **regex**: POSIX regular expression support (TRE regex library)
- **misc**: Various utilities (`getopt`, format functions, error lists)
- **win32u**: Windows Unicode and UTF-8 support utilities

## Installation

This project is designed to be installed via vcpkg:

```bat
vcpkg install utilwin:x64-windows-static --overlay-ports=<path-to-utilwin>\vcpkg.ports
```

For example:
```bat
vcpkg install utilwin:x64-windows-static --overlay-ports=C:\Projects\utilwin\vcpkg.ports
```

For more installation details, see [INSTALL.md](INSTALL.md).

## Building

The project uses CMake for building:

```bash
cmake -S src -B build
cmake --build build
```

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

### Third-Party Components

This project includes code from several third-party sources under different licenses:

- **getopt** (src/misc/getopt*.c): LGPL 2.1+ - GNU C Library
- **TRE regex** (src/regex/): BSD 2-Clause - Copyright (c) 2001-2009 Ville Laurikari

See [NOTICE](NOTICE) file for detailed copyright information.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## Author

lixungeng
