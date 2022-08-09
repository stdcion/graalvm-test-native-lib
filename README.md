## About

This is a project for testing and debugging GraalVM native-image on different platforms. The output artifacts are CMakeLists, dynamic library and header files with exported C functions that can be used with C-ABI.

## Requirement

1) GraalVM Community Edition 22.1.0 (Java 11)
2) native-image (for install call `gu install native-image` from $GRAAL_HOME/bin path)
3) Maven
4) C compiler

## Build

```text
mvn clean install
```

The target platform will be determined automatically.

## Maven tasks

1) `graal-win` - for build a native image on windows.
2) `graal-mac` - for build a native image on mac.
3) `graal-linux` - for build a native image on linux.
4) `gen-cap-cache` - to create a new cap-cache, must be called with one of the above tasks, puts a new cap-cache in the `newCapCacheDir`.

## Structure

1) `capcache-linux, capcache-mac, capcache-mac-arm64, capcache-win` - pre-generated CAP (C Annotation Processor) files for different platforms.
2) `src/main` - Java source code.
3) `src/CMakeLists.txt` - CMake file for the generated native library.
4) `target/` - output directory
5) `target/native-image` - generated native library, headers files ad CMake files.
