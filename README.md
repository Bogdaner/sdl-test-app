# Starter project for cross-platform SDL app C++

## Build

Use the following command to build for Windows, Linux, MacOS:

```bash
cmake -S . -B build
cmake --build build
```

Use the following command to build for iOS:

```bash
cmake -S . -B build -G Xcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED
cmake --build build
```
