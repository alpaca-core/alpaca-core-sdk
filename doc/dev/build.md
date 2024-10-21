# Build

These are the build instruction and build documentation for the Alpaca Core Local SDK.

If you're not only interested in building the project but also setting up a development environment, you might want also to check out the [development environment documentation](dev-env.md).

> [!IMPORTANT]
> When cloning this repo, don't forget to fetch the submodules.
> * Either: `$ git clone https://github.com/alpaca-core/ac-local.git --recurse-submodules`
> * Or:
>    * `$ git clone https://github.com/alpaca-core/ac-local.git`
>    * `$ cd ac-local`
>    * `$ git submodule update --init --recursive`

## Prerequisites

* C++ compiler with C++20 support such as:
    * MSVC 19.30 (Visual Studio 2022 v17.00) or later 
    * GCC 11 or later
    * Clang 14 or later
* [CMake](https://cmake.org/) 3.22 or later
* For some build flavors and optional components:
    * Android: CMake 3.24 is needed to properly find JNI
    * GUI Demos: [SDL2](https://wiki.libsdl.org/SDL2/FrontPage)
    * Different computational backends require their respective SDKs and dependencies

## Configure

Some useful presets are provided in `CMakePresets.json`. As a convention each presets writes the configured files in `<repo-root>/out/build/<preset-name>`. Note that most of the included CMake presets all also require [ninja](https://ninja-build.org/).

Example: 

* `$ cmake --preset=release`
* Configuration files are written in `<repo-root>/out/build/release`

For finer grain control or if you're using the repo as a subdirectory:

### CMake Options

* `AC_BUILD_LOCAL` - build the local inference api and integrations of inference libraries
* `AC_INFERENCE_LIBS` - `all` or `none` or semicolon separated list. Which inference libraries to build. The supported ones are:
    * `dummy`: a dummy inference library for testing and experimenting
    * `llama.cpp`: wrapping our fork of [ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp)
    * `whisper`: wrapping our fork of [ggerganov/whisper.cpp](https://github.com/ggerganov/whisper.cpp)
* `AC_BUILD_ACORD`: build the Alpaca Core daemon. This is `OFF` by default if the project is a subdir. Only available if `AC_BUILD_LOCAL` is `ON`.
* Advanced and `OFF` by default if the project is a subdir:
    * `AC_BUILD_TOOLS` - build tools. This means all tools for all active components. No finer-grain options are available yet
    * `AC_BUILD_TESTS` - build tests. This means all tests for all active components. No finer-grain options are available yet
    * `AC_BUILD_EXAMPLES` - build examples. This means all examples for all active components. No finer-grain options are available yet
    * `AC_BUILD_POC` - build proof of concept, sandbox, and experimental projects
* Wrapper config:
    * `AC_C_WRAPPER`: build C wrapper. `ON` by default
    * `AC_JAVA_WRAPPER`: build Java wrapper. `Auto` by default, which means only build if JNI is available

### Troubleshooting

#### SDL2 is installed, but CMake still can't find it

You likely have to add the path to the SDL2 installation to the `CMAKE_PREFIX_PATH` environment variable. Or configure with `-DCMAKE_PREFIX_PATH=<path-to-sdl2>`

## Build

`$ cmake --build <path-to-build-dir>` (for example `$ cmake --build out/build/release`)

After this the produced binaries will be in `<path-to-build-dir>/bin`.

You can run the tests with `ctest` from the build directory.

## Install

*TBD*
