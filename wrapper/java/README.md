# Alpaca Core SDK Java Wrapper

Since we want to be good open source citizens, we conform to Java and gradle's ridiculous directory structure conventions.

* `jni.hpp` - our fork of [mapbox/jni.hpp](https://github.com/mapbox/jni.hpp): a third party library which wraps JNI for C++
* `ac-jni` - JNI code which produces the native shared lib for the wrapper
* `src` - com.alpacacore Java library and tests (in many many subdirs)
* `test` - internal tests: testing the JNI code itself

## Dual build structure

This directory has a dual build structure and thus it's somewhat of a mess (lowest common denominator)

### Monorepo

First, it can be configured and built by the CMake configuration of the alpaca-core monorepo as a subdir of the root `CMakeLists.txt`. This build, including generating jars and running java tests, is driven entirely through CMake. Tests can be executed through CTest.

### Gradle

Second, it can be built by gradle as an Android project. This build is driven through gradle with this directory as root. It runs CMake internally skipping the build of jars and tests. 

The internal CMake configuration does not start with the root `CMakeLists.txt`, but instead with `src/native/CMakeLists.txt`. Thus it skips a lot of subdirs and targets: examples, tools, tests, etc.

Here jars and tests (JUnit- and Android-only) are built through gradle. C++-only tests and the internal Java Wrapper tests are not built and run.
