# Contributing to JAGE

Welcome! JAGE is a learning-focused project with high standards for code quality, testing, and design. This guide will help you understand the expectations and workflow.

## Before You Start

**Read the README** - Understand the project philosophy, design principles, and roadmap.

**Questions?** Open a GitHub issue or discussion. We prefer design discussions *before* large PRs.

## Development Principles

### Test-Driven Development (Mandatory)

JAGE follows **zero-tolerance TDD**:
- **100% test coverage** - Every line of code must be tested
- **Tests first** - Write tests before implementation
- **No exceptions** - PRs that reduce coverage will not be merged

**Workflow:**
1. Write a failing test for the new behavior
2. Implement the minimum code to make it pass
3. Refactor while keeping tests green
4. Verify coverage: `cmake --build build --target coverage`

### SOLID Principles (Mandatory)

Every contribution must follow SOLID principles:

**Single Responsibility:**
- Classes/functions do one thing well
- If you struggle to name something concisely, it's probably doing too much

**Open-Closed:**
- Extend behavior through composition/inheritance, not modification
- Use templates, interfaces, and policies for extensibility

**Liskov Substitution:**
- Subtypes must be substitutable for their base types
- Mock implementations must behave identically to real ones

**Interface Segregation:**
- Small, focused interfaces
- Consumers shouldn't depend on methods they don't use

**Dependency Inversion:**
- Depend on abstractions (concepts, interfaces), not concrete types
- Use templates to avoid runtime polymorphism overhead

### Swappable I/O (Mandatory)

All I/O boundaries must be swappable:
- **Mock and real implementations must be indistinguishable**
- Use template-based composition: `template<typename TKeyboardAdapter>`
- Example: Gameplay systems shouldn't know if input comes from a real keyboard, mock, or replay file

**Why?** Enables testing, recording, replay, and simulation without changing core logic.

### Architectural Review

For **non-trivial changes**, open a GitHub issue first:
- New subsystems or features
- Changes affecting multiple files
- Public API modifications
- Performance-critical paths

Discuss the design before writing code. This avoids wasted effort and ensures alignment with project goals.

## Code Quality Standards

### Naming Conventions
- `snake_case` for functions, variables, namespaces
- `PascalCase` for types/classes
- `SCREAMING_SNAKE_CASE` for macros (avoid macros when possible)

### Comments
- Explain **why**, not **what** (code should be self-documenting)
- Use comments for non-obvious design decisions or trade-offs
- No commented-out code (use git history)

### Error Handling
- Validate at system boundaries (user input, file I/O, network)
- Trust internal code and framework guarantees
- Avoid defensive programming within the library

### Performance
- Don't optimize prematurely
- Profile before optimizing
- Document performance-critical sections

## Adding a New Feature

**Step-by-step process:**

1. **Design Phase:**
   - Open a GitHub issue describing the feature
   - Discuss architectural approach (if non-trivial)
   - Get feedback before coding

2. **Test Phase:**
   - Create test file: `test/unit/jage/<subsystem>/<feature>_test.cpp`
   - Write tests for expected behavior (they should fail)
   - Add test to `CMakeLists.txt` using `add_unit_test()`

3. **Implementation Phase:**
   - Create header: `include/jage/<subsystem>/<feature>.hpp`
   - Implement minimum code to pass tests
   - Follow SOLID principles, ensure swappable I/O

4. **Verification Phase:**
   - Run tests: `cmake --build build --target run-all-jage-unit-tests`
   - Check coverage: `cmake --build build --target coverage`
   - Verify 100% coverage for new code

5. **Documentation Phase:**
   - Add inline documentation (if public API)
   - Update README if feature is user-facing
   - Update CLAUDE.md if there are special build/test considerations

6. **Pull Request:**
   - Clear description of what and why
   - Reference related issues
   - Ensure CI passes (tests, coverage, multi-compiler builds)

## Pull Request Expectations

**Good PRs:**
- ✅ Single, focused change
- ✅ 100% test coverage maintained or improved
- ✅ All tests pass on all platforms/compilers
- ✅ Follows SOLID principles
- ✅ Clear commit messages (see Commit Message Guidelines)
- ✅ No unrelated formatting changes

**PRs that will be rejected:**
- ❌ Reduces test coverage
- ❌ Breaks existing tests
- ❌ Large, unfocused changes
- ❌ Missing tests
- ❌ Violates SOLID principles
- ❌ Platform-specific hacks without abstraction

## Commit Message Guidelines

We follow [Conventional Commits](https://www.conventionalcommits.org/) with gitmoji:

**Format:**
```
<emoji> [<scope>] <description>

<optional body>

<optional footer>
```

**Examples:**
```
✨ [input] Add keyboard event normalization layer

Introduces raw → normalized → action mapping pattern.
Normalized events are platform-agnostic (Key::Space vs VK_SPACE).

Closes #42
```
```
🐛 [memory] Fix cacheline alignment on ARM64

Cacheline size detection was incorrect on ARM platforms.
Now uses std::hardware_destructive_interference_size.
```
```
♻️ [tests] Refactor SPSC queue tests for clarity

Split monolithic test into focused test cases.
No behavior changes.
```

**Common emojis:**
- ✨ New feature
- 🐛 Bug fix
- ♻️ Refactor
- 📝 Documentation
- ✅ Tests
- ⚡ Performance
- 🔧 Configuration
- 🚀 Deployment/CI

## Testing Guidelines

### What to Test
- **Public APIs** - All public functions and classes
- **Edge cases** - Empty inputs, null pointers, boundary conditions
- **Error paths** - Invalid arguments, resource exhaustion
- **Integration** - How components work together

### What NOT to Test
- Private implementation details (test behavior, not internals)
- Third-party library internals (trust your dependencies)

### Test Organization
- Mirror library structure: `test/unit/jage/<subsystem>/`
- One test file per header (usually)
- Use GUnit's BDD syntax for readability:
  ```cpp
  "should return true when button is pressed"_test = [] {
      // Given
      button_state state;
      // When
      state.press();
      // Then
      expect(state.is_pressed());
  };
  ```

### Running Tests
```bash
# All tests
cmake --build build --target run-all-jage-unit-tests

# Specific subsystem
cmake --build build --target run-all-jage-input-unit-tests

# Single test
./build/test/unit/jage/input/jage-unit-test-button
```

## Coverage Requirements

**Zero tolerance: 100% coverage required.**

Check coverage:
```bash
conan install . -pr:a profiles/coverage -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing
cmake --preset conan-build-linux-gcc-debug
cmake --build build --target coverage
```

Open `build/coverage-report/index.html` to see detailed coverage report.

**If coverage drops below 100%:**
1. Identify uncovered lines in the report
2. Add tests for those paths
3. Re-run coverage to verify

## Multi-Platform Testing

JAGE must build on multiple platforms and compilers:
- Linux (GCC, Clang)
- Windows (MSVC, Clang) - planned
- macOS (Clang) - planned

**Before submitting a PR:**
1. Test with both GCC and Clang:
   ```bash
   # GCC
   conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing
   cmake --preset conan-build-linux-gcc-debug && cmake --build build

   # Clang
   conan install . -pr:a profiles/linux -pr:a profiles/clang -pr:a profiles/debug --build=missing
   cmake --preset conan-build-linux-clang-debug && cmake --build build
   ```

2. Test with sanitizers:
   ```bash
   # Address sanitizer
   conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug -pr:a profiles/asan --build=missing
   cmake --preset conan-build-linux-gcc-debug-asan && cmake --build build

   # UndefinedBehavior sanitizer
   conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug -pr:a profiles/ubsan --build=missing
   cmake --preset conan-build-linux-gcc-debug-ubsan && cmake --build build
   ```

## Questions or Stuck?

- **Open an issue** - For bugs, feature requests, or design questions
- **Start a discussion** - For open-ended questions or ideas
- **Read CLAUDE.md** - For build system and architecture details

We're here to help and learn together. Don't hesitate to ask questions!

---

## Setup Instructions

This document describes how to set up and build the project using either the native toolchain (CMake + Conan) or the provided devcontainer.

## Command line (CMake + Conan)

Prerequisites:
- CMake
- Conan (v2)
- A C/C++ compiler toolchain
- Python (for Conan install if needed)

Example setup and build on Linux:

```bash
# Install Conan in a virtualenv
python3 -m venv conan
source conan/bin/activate
pip install conan

# Install dependencies
conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing

# Configure
cmake --preset conan-build-linux-gcc-debug

# Build
cmake --build build --target run-all-jage-unit-tests
```

## Dev Containers (VS Code)

Prerequisites:
- Docker (or Docker Desktop)
- VS Code
- VS Code "Dev Containers" extension

Steps:
1. Open the repo folder in VS Code.
2. Run "Dev Containers: Reopen in Container" from the Command Palette.
3. Use the VS Code terminal to run the same Conan/CMake commands as above.

The container image and VS Code extensions are defined in `.devcontainer/devcontainer.json` and `docker/Dockerfile`.

Once inside the VS Code IDE, run the conan install command with the desired profiles, then select the generated CMake configuration preset in VS Code to configure the project.

## Dev Containers (Command line)

Prerequisites:
- Docker (or Docker Desktop)
- Node.js and npm

Steps:
```bash
# On ubuntu
sudo apt install nodejs npm
sudo npm install -g @devcontainers/cli

devcontainer build --workspace-folder /path/to/jage
devcontainer up --workspace-folder /path/to/jage

devcontainer exec --workspace-folder /path/to/jage -- bash
```

Once inside the container shell, run the same Conan/CMake commands from the command line section.

## Scripts

### prune_gone_branches.py

Deletes local git branches whose upstream has been removed from the remote. It will prompt before deleting the currently checked out branch unless `--respond-yes` is provided.

Example:

```bash
scripts/prune_gone_branches.py
```
