# JAGE (Just Another Game Engine)

JAGE is primarily a sandbox to experiment with C++ and other software, with a focus on foundational game engine building blocks. The core library is header-only and is exercised by a minimal GLFW demo app.

## Philosophy

JAGE is a **learning-focused, personal growth project**—not a race to ship features. The goal is to explore game engine architecture, modern C++, and systems programming fundamentals through deliberate practice.

**Core Values:**
- **Quality over speed** - No deadlines, no shortcuts
- **Test-driven development** - 100% test coverage, zero tolerance
- **Start simple, extend without rewriting** - Build small, composable foundations
- **Learning by doing** - Understand the "why," not just the "what"

## Design Principles

Every component in JAGE follows these principles:

**SOLID Architecture:**
- **Single Responsibility** - Each class/function does one thing well
- **Open-Closed** - Extend behavior without modifying existing code
- **Dependency Inversion** - Depend on abstractions, not concretions

**Swappable I/O:**
- Mock, simulated, and real hardware inputs must be indistinguishable
- Template-based composition (e.g., `jage::game<TWindow, TInputController>`)
- Enables testing, recording, replay without changing core logic

**Extensibility First:**
- Simple cores that scale up without rewrites
- Design for future features (e.g., recording, diagnostics) from day one
- Avoid premature optimization, but consider architectural implications

**Multi-Platform by Design:**
- Supports Linux, Windows, macOS (planned)
- Multiple compilers: GCC, Clang, MSVC
- No platform-specific hacks without proper abstraction

## Current Roadmap

JAGE is in active development. Current and planned subsystems:

**Implemented:**
- ✅ Game loop and window abstraction
- ✅ Input monitoring (keyboard, mouse, cursor)
- ✅ Time utilities (clock, snapshots, scaling)
- ✅ Concurrency primitives (double buffer, SPSC queue)
- ✅ Memory utilities (cacheline alignment)

**In Design:**
- 🔄 Input event bus (low-latency, async, producer→bus→queues→consumers)
- 🔄 Event normalization pipeline (raw → normalized → action mapping)

**Planned:**
- 📋 State registry for input queries
- 📋 Recording and replay infrastructure
- 📋 Joystick/gamepad support
- 📋 Entity-component system (ECS)
- 📋 Rendering abstraction (OpenGL, Vulkan)

## Why Header-Only?

JAGE's library is header-only to:
1. **Simplify integration** - Drop headers into any project, no linking
2. **Enable full optimization** - Compiler sees all code, can inline aggressively
3. **Reduce build complexity** - No static/dynamic library decisions
4. **Force good design** - Headers expose interfaces clearly

Trade-offs: Longer compile times, but for a learning project, the benefits outweigh the costs.

## Features
- Game loop and window abstraction (`jage::game`, `jage::window`) driven by a user-provided driver.
- Input system with keyboard, mouse, and cursor monitors; fixed-capacity callbacks; per-button state tracking.
- Time utilities: real-number durations, `hertz` literal, steady clock with time scaling, and snapshot reporting.
- Concurrency: cacheline-aligned double buffer for single-writer/single-reader data handoff.
- Containers: cacheline-aware SPSC queue that overwrites the oldest element when full.
- Memory helpers: cacheline size constant and `cacheline_slot` to pad and align values.

## Build (Dev Containers preferred)
Dev Containers are the happy path for local development. You get the exact toolchain and deps used in CI, with minimal fuss.

### Dev Containers (VS Code)
1. Open the repo in VS Code.
2. Run "Dev Containers: Reopen in Container".
3. Use the integrated terminal for build/test commands (you still run `conan install` inside the container).

### Dev Containers (CLI)
Prereqs: Docker, Node.js, npm.

```bash
sudo npm install -g @devcontainers/cli
devcontainer build --workspace-folder /path/to/jage
devcontainer up --workspace-folder /path/to/jage
devcontainer exec --workspace-folder /path/to/jage -- bash
```
Then run the usual `conan install` and CMake commands from inside the container shell.

### CLI Build (Conan + CMake)
Prereqs: CMake 3.28+, Conan v2, a C++23 compiler, and Python.

On Windows, install [Visual Studio 2026 Build Tools](https://visualstudio.microsoft.com/downloads/) (or full Visual Studio 2026) with the "Desktop development with C++" workload. CMake and Ninja are included with the build tools — launch a "Developer PowerShell for VS 2026" to get them on your PATH.

#### Linux (GCC)
```bash
python3 -m venv conan
source conan/bin/activate
pip install conan
conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing
cmake --preset conan-build-linux-gcc-debug
cmake --build build --target jage
```

Run the demo app:

```bash
./build/jage
```

#### Windows (MSVC)
```powershell
python -m venv .conan
.\.conan\Scripts\activate.bat
pip install conan
conan install . -pr:a profiles/windows -pr:a profiles/msvc -pr:a profiles/debug --build=missing
build-windows-msvc-debug\generators\conanbuild.bat
cmake --preset conan-build-windows-msvc-debug
cmake --build build --target jage
```

Run the demo app:

```powershell
.\build\jage.exe
```

## Tests
Preferred: run these inside the Dev Container so your environment matches CI.

```bash
cmake --build build --target run-all-jage-unit-tests
```

## Coverage
Coverage requires `lcov` and `gcov` and is only available on Linux. Preferred: run coverage inside the Dev Container to avoid toolchain mismatches.

```bash
conan install . -pr:a profiles/coverage -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing
cmake --preset conan-build-linux-gcc-debug
cmake --build build --target coverage
```

Coverage HTML is generated in `build/coverage-report`.

## Presets 101
Presets are generated by Conan and then included by your local
`CMakeUserPresets.json` (ignored by git). Run `conan install` first, then pick
a preset.

Command line:

```bash
# Linux
cmake --list-presets
cmake --preset conan-build-linux-gcc-debug
cmake --build build

# Windows
cmake --list-presets
cmake --preset conan-build-windows-msvc-debug
cmake --build build
```

VS Code (CMake Tools):
1. Run "CMake: Select Configure Preset".
2. Pick a `conan-build-*` preset.
3. Run "CMake: Configure" then "CMake: Build".

If no presets show up, it usually means `conan install` hasn’t been run for
that profile yet.

## Project layout
- `include/`: header-only library code
- `app/`: GLFW demo app entry point
- `test/`: unit tests and test helpers
- `cmake/`: compiler options and coverage helpers
- `profiles/`: Conan profiles for toolchains and sanitizers
- `docker/`: devcontainer image

## Dependencies
Core build uses:
- glad
- glfw3
- glm
- range-v3

Testing uses:
- GoogleTest/GoogleMock

Conan recipe lists:
- glad
- glfw
- glm
- gtest
- range-v3
- assimp
