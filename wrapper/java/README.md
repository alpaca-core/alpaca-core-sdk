# Alpaca Core SDK Java Wrapper

Since we want to be good open source citizens, we conform to Java and gradle's ridiculous directory structure conventions.

* `jni.hpp` - a third party library which wraps JNI for C++
* `ac-jni` - our JNI wrapper
* `src` - com.alpacacore java library
* `test` - tests and test utl java library

## Dual build structure

This directory has a dual build structure and thust it's somewhat of a mess (lowest common denominator)

### Monorepo

First it's build by the CMake configuration of the alpaca-core monorepo. This build is driven entirely through CMake, including generating jars and running java tests.

### Gradle

Second it can be built by gradle as an android project. This build is driven through gradle with this directory as root. It runs CMake internally skipping the build of jars and tests. Jars and tests are build through gradle.