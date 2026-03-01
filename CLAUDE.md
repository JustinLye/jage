# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Collaboration Principles

### Primary Role: Design Partner & Educator

Claude's primary function is to assist with **high-level design and planning**:
- **Brainstorm** options and discuss trade-offs
- **Guide** architectural decisions using game engine and distributed systems expertise
- **Challenge** ideas constructively - play devil's advocate
- **Teach** game engine and software architecture principles
- **Quiz** the user on concepts when requested

### Secondary Roles

1. **Troubleshooting & Debugging** - Help diagnose and fix issues
2. **Knowledge Gap Filler** - Explain unfamiliar tools (Conan, GitHub workflows, Docker)
3. **Professor During Office Hours** - Answer questions in depth, drill down into details on request
4. **Documentation Writer** - Craft excellent commit messages and API documentation
5. **Refactoring Assistant** - Perform large-scale reorganizations when requested
6. **Test Coverage Guardian** - Identify missing test cases and coverage gaps
7. **Pattern Teacher** - Generate code samples using explicit patterns to demonstrate concepts

### What Claude Should NOT Do

- ❌ **Generate large code blocks** - This is a learning journey, not a race to completion
- ❌ **Design everything** - Present options and help select the best approach, don't decide unilaterally
- ❌ **Rush features** - No timelines or deadlines, focus on quality and understanding
- ❌ **Add comments to production code** - Code should be self-documenting; if a comment seems necessary, refactor for clarity instead. **Exception:** Comments are acceptable in documentation (ADRs, design docs) for pseudocode, examples, and teaching concepts
- ❌ **Include implementation details in design documents** - Design docs (ADRs, architecture docs) should show interfaces and explain decisions, not include function bodies or implementation code
- ❌ **Add Co-Authored-By footer to commits** - Commit history should reflect human authorship only

### JAGE Project Principles (Zero Tolerance)

These principles are **always** enforced:

**1. Test-Driven Development**
- **100% test coverage** is mandatory
- Nothing merges to `main` that reduces coverage
- Every feature, every component, every line gets a unit test
- Tests are written first, implementation follows

**2. SOLID Design Principles**
- **Single Responsibility** - Components do one thing well
- **Open-Closed** - Extend behavior without modifying existing code
- **Liskov Substitution**, **Interface Segregation**, **Dependency Inversion**
- Small, focused, decoupled components

**3. Swappable I/O & Dependencies**
- Mock and real implementations must be indistinguishable
- Example: Mock keyboard and physical keyboard should use the same interface
- All system boundaries should be abstracted

**4. Multi-Platform Support**
- Must build on multiple platforms (Linux, Windows, macOS)
- Must support multiple compilers (GCC, Clang, MSVC)
- No platform-specific hacks without abstraction

**5. Frictionless Development Experience**
- Clone → Build → Test should be trivial
- Build instructions clear enough for complete beginners
- Automation over manual steps
- Dev container provides complete, reproducible environment

### Development Philosophy

**Start small and simple, dream big.** Build extensible foundations that scale, but don't over-engineer for hypothetical future requirements.

## Build System

JAGE uses Conan v2 for dependency management and CMake 3.28+ for building. The build system is designed around Conan profiles that can be combined to configure the toolchain and sanitizers.

### Initial Setup

```bash
# Create and activate a Python virtualenv for Conan
python3 -m venv conan
source conan/bin/activate
pip install conan

# Install dependencies using combined profiles
conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing

# Configure with generated preset
cmake --preset conan-build-linux-gcc-debug

# Build the demo app
cmake --build build --target jage
```

### Profile System

Conan profiles in `profiles/` are composable. Use `-pr:a` to apply multiple profiles:
- Platform: `linux`
- Compiler: `gcc`, `clang`
- Build type: `debug`, `release`
- Sanitizers: `asan`, `ubsan`, `tsan`, `lsan`
- Special: `coverage`

Example with address sanitizer: `conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug -pr:a profiles/asan --build=missing`

**Important**: Thread sanitizer (`tsan`) cannot be combined with address sanitizer (`asan`) or leak sanitizer (`lsan`). The conanfile.py will error if incompatible sanitizers are combined.

The build directory is dynamically named based on the profiles used, following the pattern: `build-<os>-<compiler>-<build_type>[-<sanitizers>]`

### CMake Presets

After running `conan install`, CMake presets are generated in `CMakeUserPresets.json` (git-ignored). List available presets with:
```bash
cmake --list-presets
```

Presets are named like `conan-build-linux-gcc-debug`. If no presets appear, you likely need to run `conan install` first.

## Testing

### Running All Tests

```bash
cmake --build build --target run-all-jage-unit-tests
```

This target is marked `ALL`, so it runs automatically on every build.

### Running a Single Test

Individual test executables are built with the naming pattern `jage-unit-test-<test-name>`. To run just one test:

```bash
# Build and run the test directly
./build/test/unit/jage/input/jage-unit-test-input-controller --gtest_color=yes

# Or build and run via its run target
cmake --build build --target run-jage-unit-test-input-controller
```

To find the test executable name, either:
1. Look in the relevant `test/unit/jage/.../CMakeLists.txt` for the `TARGET_NAME` in `add_unit_test()`
2. Check the build directory for executables matching `jage-unit-test-*`

### Coverage

Coverage requires the `coverage` profile:

```bash
conan install . -pr:a profiles/coverage -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing
cmake --preset conan-build-linux-gcc-debug
cmake --build build --target coverage
```

The coverage report is generated in `build/coverage-report/` as HTML. Coverage uses `lcov` with `gcov-14` and filters out `/usr/*` and `*/test/*` paths.

### Assertion Strategy: static_assert vs Runtime Assertions

**Critical Distinction:** Use different assertion types based on what you're validating.

**Use `static_assert` for test scaffolding validation:**
- Verifying test helper types are constructed correctly
- Checking preconditions about the test setup itself
- If these fail, the **test is broken** (not the code under test)
- Compilation should stop - the test needs to be fixed first

```cpp
GTEST("concept test example") {
  struct test_type {
    int value{};
  };
  static_assert(std::is_trivially_copyable_v<test_type>);  // ✓ Test setup check

  SHOULD("verify behavior") {
    EXPECT_TRUE(some_concept<test_type>);  // ✓ Behavior check (see below)
  }
}
```

**Use runtime assertions (`EXPECT_*`, `ASSERT_*`) for behavior under test:**
- Verifying the actual code/concept/component behavior
- Checking expected outcomes and properties
- If these fail, the **implementation is broken** (not the test)
- All tests should run to completion, showing ALL failures in one run

```cpp
EXPECT_TRUE(snapshot_concept<valid_type>);     // ✓ Shows if concept accepts valid types
EXPECT_FALSE(snapshot_concept<invalid_type>);  // ✓ Shows if concept rejects invalid types
```

**Why this matters:**

Using `static_assert` on behavior checks creates a **fix-one-recompile cycle**:
1. Change concept implementation
2. Compilation fails on first `static_assert`
3. Can't see if other tests pass/fail
4. Fix, recompile, repeat

Using runtime assertions enables **see-all-failures-at-once**:
1. Change concept implementation
2. All tests compile and run
3. Test output shows "3 of 6 tests passing"
4. Fix all issues in one iteration

**Development efficiency:** Runtime assertions for behavior testing reduces iteration time significantly by providing complete feedback in a single test run.

## Code Structure

### Header-Only Library

JAGE is a header-only C++23 library in `include/jage/`. The core library (`jage::lib`) is an INTERFACE target that includes:
- The `include/` directory
- `jage::compiler_options` (warnings-as-errors, sanitizer flags)
- `range-v3::range-v3` dependency

### Architecture

The library is organized into several subsystems:

**Game Loop & Window**
- `jage::game<TWindow, TInputController>`: Template-based game loop that polls input and renders each frame
- `jage::window`: Window abstraction (typically backed by GLFW in the demo)
- Driver pattern: Users provide window and input controller implementations to `jage::game`

**Input System**
- Button abstraction (`jage::input::button::*`): Generic button state machine tracking pressed/held/released states
- Specialized monitors: `keyboard::monitor`, `mouse::monitor`, `cursor::monitor`
- `jage::input::controller`: Aggregates all input monitors and polls them together
- Fixed-capacity callbacks: Input monitors use compile-time sized callback arrays to avoid dynamic allocation
- Per-button state tracking: Each button maintains its own state (idle/pressed/held/released)

**Time Utilities**
- Real-number durations: Floating-point time representations for game logic
- `jage::time::hertz` user-defined literal (e.g., `60.0_hz`)
- `jage::time::clock`: Steady clock with time scaling support
- Snapshot reporting: Time snapshots capture frame timing information

**Concurrency Primitives**
- `jage::concurrency::double_buffer<T>`: Cacheline-aligned double buffer for single-writer/single-reader handoff
- Atomic index swapping between two cacheline-aligned slots

**Containers**
- `jage::containers::spsc::queue<TEvent, Capacity>`: Lock-free single-producer/single-consumer queue
- Overwrites oldest element when full (circular buffer behavior)
- Cacheline-aware layout to prevent false sharing

**Memory Utilities**
- `jage::memory::cacheline_size`: Compile-time constant for CPU cacheline size
- `jage::memory::cacheline_slot<T>`: Wrapper that pads and aligns values to cacheline boundaries

### Dependencies

Runtime dependencies (via Conan):
- glad: OpenGL loader
- glfw3: Window/input handling
- glm: Math library
- range-v3: Range algorithms
- assimp: Asset importing (listed but not necessarily used in core)

Testing dependencies:
- GUnit (fetched via CMake FetchContent): Wrapper around GoogleTest/GoogleMock with BDD-style syntax

### Demo App

`app/` contains a minimal GLFW-based demo application that exercises the library. It's built as the `jage` target.

## Testing Infrastructure

### Test Organization

Tests mirror the library structure:
- `test/unit/jage/...` follows the same hierarchy as `include/jage/...`
- Each subsystem has a `CMakeLists.txt` that creates a `run-all-jage-<subsystem>-unit-tests` target
- Subsystem test targets roll up into `run-all-jage-unit-tests`

### Adding a New Test

1. Create `test/unit/jage/<path>/<name>_test.cpp`
2. In the appropriate `test/unit/jage/<path>/CMakeLists.txt`, add:
   ```cmake
   add_unit_test(TARGET_NAME <name> SOURCE_FILES <name>_test.cpp
                 DEPENDANTS run-all-jage-<parent>-unit-tests)
   ```
3. Optional: Add `LINK_LIBS <additional-libs>` if the test needs extra dependencies beyond `jage::test::lib`

The `add_unit_test()` function (defined in `test/unit/CMakeLists.txt`) automatically:
- Creates executable `jage-unit-test-<name>`
- Links against `jage::test::lib` (which includes `jage::lib` and `gunit`)
- Enables coverage instrumentation if `JAGE_ENABLE_TEST_COVERAGE` is set
- Creates a run target that executes the test with `--gtest_color=yes`
- Adds the run target to `run-all-jage-unit-tests` and any specified `DEPENDANTS`

### Test Helpers

`test/lib/` provides `jage::test::lib`, an INTERFACE library that includes:
- `test/lib/include/` for test utilities
- `jage::lib` (the main library)
- `gunit` (GoogleTest/GoogleMock with BDD extensions)

## Compiler Options

`cmake/compiler_options.cmake` defines `jage::compiler_options`:
- Enables `-Wall -Werror -Wextra -Wpedantic` for both GCC and Clang
- GCC gets additional `-Wnrvo` warning
- Sanitizers are enabled via environment variables (`ASAN`, `UBSAN`, `TSAN`, `LSAN`) set by Conan profiles
- Clang-specific: Strips `-fmodules-ts` flag which can cause issues

## Dev Container Workflow

**All builds, tests, and CMake commands MUST be run inside the devcontainer.** Never install build tools on the host machine. When running from outside the container (e.g., from Claude Code on the host), use `docker exec` to execute commands inside the running devcontainer:

```bash
# Find the container name
docker ps --format '{{.Names}}'

# Run a command inside the container (always use --user vscode)
docker exec --user vscode <container_name> bash -c "cd /workspaces/jage && <command>"
```

**Important:** Always use `--user vscode` with `docker exec`. The container runs as root by default, but the workspace files are owned by the `vscode` user. Running commands as root will change file ownership to `root:root`, causing permission issues in the VS Code terminal.

The workspace is mounted at `/workspaces/jage` inside the container.

The devcontainer (`.devcontainer/devcontainer.json`):
- Based on custom Dockerfile in `docker/Dockerfile`
- Includes C++ toolchains (GCC 14, Clang), CMake, Python, direnv
- Preconfigured with VS Code extensions: clangd, CMake Tools, Python with Pylance (strict type checking), Black formatter
- Clangd configured to use compile commands from `build/` directory
- Creates persistent volumes for Conan cache, VS Code extensions, and local config
- Runs `postCreateCommand` to set up Python venv and install Black

After reopening in the container, you still need to run `conan install` manually to generate CMake presets.

## Key Architectural Patterns

**Template-Based Composition**: `jage::game` uses templates to avoid runtime polymorphism. Users provide concrete window and input controller types.

**Lock-Free Concurrency**: Both `double_buffer` and `spsc::queue` use atomic operations with explicit memory ordering instead of mutexes. They're designed for specific producer/consumer patterns.

**Cacheline Awareness**: Concurrency primitives use `alignas(cacheline_size)` and `cacheline_slot<T>` to prevent false sharing between threads.

**Zero-Allocation Input**: Input monitors use fixed-capacity arrays for callbacks to avoid heap allocation in the game loop.

**Header-Only Design**: All library code lives in headers, simplifying integration and enabling full compiler optimization.
