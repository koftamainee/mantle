# 🔥 Mantle — Vulkan game engine

[![CI](https://github.com/koftamainee/mantle/actions/workflows/build.yml/badge.svg)](https://github.com/koftamainee/mantle/actions/workflows/build.yml)
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C.svg)](https://en.cppreference.com/w/cpp/23)
[![Vulkan](https://img.shields.io/badge/Vulkan-1.3-red.svg)](https://www.vulkan.org)

**Mantle** is a modular game engine built on Vulkan, ECS, and Jolt Physics.
Written in C++23 with a focus on clean architecture and runtime performance.

---

## ✨ Features

- **Vulkan renderer** — frame graph with automatic resource management, transient resources, GPU allocator, blackboard system
- **ECS** — flecs-driven entity component system
- **Physics** — Jolt Physics integration with character controller, broad phase layers
- **Memory** — custom virtual memory system, arena allocators, TLSF, PMR adaptors
- **Build** — CMake + C++23, clang-format and custom formatting scripts via CI, modular library layout
- **CI/CD** — automated build & test (Linux + Windows), daily releases with changelogs

---

## 📦 Modules

Module | Description
---|---
`core` | Memory allocators, logging, concurrency, asserts, macros
`build_info` | Compile-time metadata (version, hash, date, compiler)
`system_info` | Runtime hardware/OS detection
`window` | SDL-based windowing with custom allocator
`renderer` | Vulkan renderer with frame graph
`engine` | Engine lifecycle, world creation, system wiring
`math` | AABB, frustum, plane
`noise` | Procedural noise samplers (was used in previous builds, now unused)
`ecs` | ECS component types (flecs)
`physics` | Jolt Physics integration
`keybinds` | Configurable key/button bindings
`input` | Action/axis input system

---

## 🚀 Quick start

### Requirements

- C++23 compiler (targeting GCC 14+ on Linux and, MSVC 2022 17.12+ on Windows)
- CMake 3.26+
- Ninja or Make on Linux, MSVC build system on windows
- Vulkan SDK 1.3+
- Linux: Wayland/X11 dev packages, Mesa

### Build

```bash
git clone --recursive https://github.com/koftamainee/mantle
cd mantle
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
```

### Run

```bash
cd build/target
./game
```

---

## 🧱 Project structure

```
mantle/
├── mantle/
│   ├── src/              # engine modules
│   ├── tests/            # unit tests (gtest)
│   └── third_party/      # dependencies (git submodules)
├── game/                 # game layer
├── cmake/                # build system scripts
├── scripts/              # tooling (formatting, headers, error codes)
├── .github/              # CI workflows & templates
├── CMakeLists.txt        # root build file
├── VERSION               # version string
└── LICENSE               # Apache 2.0
```

### Module layout

Each engine module lives in `mantle/src/<name>/` and follows a fixed layout:

```
mantle/src/<name>/
├── CMakeLists.txt
├── public/
│   └── mantle/
│       └── <name>/
│           └── *.h       # public API headers
└── private/
    ├── *.cpp              # implementation
    └── *.h                # internal headers (optional)
```

A module's `CMakeLists.txt` is a single call:

```cmake
mantle_module(NAME <name> DEPS <other modules>)
```

- **Public headers** go under `public/mantle/<name>/` — included as `#include "mantle/<name>/foo.h"`
- **Private code** goes in `private/` — not exposed to consumers
- **Dependencies** are linked publicly (transitive to anything that depends on the module)
- Header-only modules (no `private/*.cpp`) automatically become `INTERFACE` libraries

Example — the `input` module:

```cmake
mantle_module(NAME input DEPS ecs window keybinds)
```

```
mantle/src/input/
├── CMakeLists.txt
├── public/mantle/input/
│   └── input_system.h
└── private/
    └── input_system.cpp
```

To add a new module, create the directory with the layout above, call `mantle_module()` in its `CMakeLists.txt`, and add `add_subdirectory(<name>)` to `mantle/src/CMakeLists.txt`.

---

## 🧩 Third-party

| Library | License | Purpose |
|---|---|---|
| [SDL](https://github.com/libsdl-org/SDL) | Zlib | Windowing, input |
| [Vulkan SDK](https://www.vulkan.org) | Apache 2.0 | Graphics API |
| [flecs](https://github.com/SanderMertens/flecs) | MIT | ECS framework |
| [Jolt Physics](https://github.com/jrouwe/JoltPhysics) | MIT | Physics simulation |
| [LuaJIT](https://github.com/LuaJIT/LuaJIT) | MIT | Scripting VM |
| [sol2](https://github.com/ThePhD/sol2) | MIT | C++/Lua bindings |
| [spdlog](https://github.com/gabime/spdlog) | MIT | Logging |
| [imgui](https://github.com/ocornut/imgui) | MIT | Debug UI |
| [glm](https://github.com/g-truc/glm) | MIT | Math library |
| [googletest](https://github.com/google/googletest) | BSD-3 | Testing |
| [tlsf](https://github.com/mattconte/tlsf) | BSD-3 | Real-time allocator |

---

## 🛠️ Tooling

- **mantle-format** — `cmake --build build --target mantle-format` runs clang-format, copyright headers, and include normalization. CI auto-formats on push when `[skip-ci-format]` flag is not present in commit message.
- **[mint](https://github.com/koftamainee/mint)** — asset pipeline that converts glTF/GLB → `.mmesh`/`.mmat`/`.mprop`/`.mskel`/`.manim`/`.ktx2`. Install via `cargo install --git https://github.com/koftamainee/mint`. See its README for format specs and CLI reference.

---

## 🤝 Contributing

Pull requests are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md).

## 📄 License

Apache 2.0 — see [LICENSE](LICENSE).
