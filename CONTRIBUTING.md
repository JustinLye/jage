## Prerequisities

### Linux

#### Install system packages

##### Debian
```bash
scripts/bootstrap.sh
```
#### Install Conan

See [conan package manager installation documentation](https://docs.conan.io/2/installation.html) for most up-to-date information.

Below is an example of how to install conan
```bash
cd ~
python -m venv conan
source conan/bin/activate
pip install conan
```

## Building

### Linux

#### Activate Conan

```bash
source ~/conan/bin/activate
```

#### Install Build Requirements
```bash
conan install . -pr:a profiles/linux -pr:a profiles/gcc -pr:a profiles/debug --build=missing
```

#### Configure
```bash
cmake --preset conan-build-linux-gcc-debug
```

#### Build
```bash
cmake --build build --target run-all-jage-unit-tests
```


