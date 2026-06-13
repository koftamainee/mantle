# Contributing to Mantle

Thanks for your interest! Here's how to get started.

## Code style

- C++23, 4-space indent, 100-column limit
- Naming conventions: 
  - `PascalCase` for classes 
  - `snake_case` for functions, methods and variables
  - `m_snake_case` for class private member variables
  - `s_snake_case` for static variables
  - `g_snake_case` for global variables
  - `kPascalCase` for constants
  - `MANTLE_` prefix for macros
- All of project's classes and functions should be in `mantle` namespace
- Exceptions and RTTI are banned in this project. Your code will not compile if any of them present
- Project features custom virtual memory system with allocators and `pmr` wrapper for STL containers. Prefer to use it over global heap
- `clang-format` and custom formatting are enforced via CI. Also you can apply it locally, for this run `cmake --build build --target mantle-format` before pushing

## PR process

1. Fork the repo and create a branch from `master`
2. Make your changes
3. Run tests: `ctest --test-dir build --output-on-failure`
4. Run formatting: `cmake --build build --target mantle-format`
5. Open a PR against `master` with a clear title and description

## Commit messages

- Use imperative mood ("Add feature", not "Added feature")
- Prefix with the module name if applicable: `physics: fix broadphase crash`
- Include `[skip-ci-format]` if you already ran formatting

## Questions

Open an issue or start a discussion. Happy hacking!
