# Contributing

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
