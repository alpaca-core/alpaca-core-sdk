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

The minimum requirements are:

* C++ compiler with C++20 support such as:
    * MSVC 19.30 (Visual Studio 2022 v17.00) or later
    * GCC 11 or later
    * Clang 14 or later
* [CMake](https://cmake.org/) 3.22 or later
* For some build flavors and optional components:
    * Android: CMake 3.24 is needed to properly find JNI
    * GUI Demos: [SDL2](https://wiki.libsdl.org/SDL2/FrontPage)
    * Different computational backends require their respective SDKs and dependencies

If you want practical steps on for specific environments, check the [developer environments documentation](dev-env.md).

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
    * `AC_BUILD_TOOLS` - build all tools for all active components. No finer-grain options are available yet
    * `AC_BUILD_TESTS` - build all tests for all active components. No finer-grain options are available yet. Git LFS is required for some tests
    * `AC_BUILD_EXAMPLES` - build all examples for all active components. No finer-grain options are available yet. Git LFS is required for some examples
    * `AC_BUILD_POC` - build proof of concept, sandbox, and experimental projects
* Wrapper config. 
    * Wrapper options are have three possible values: `ON`, `OFF`, and `AUTO`. `AUTO` will try to detect if the wrapper can be built.
    * `AC_WRAPPER_DEFAULT_VALUE`: this is not a CMake option, but you can define it for the initial configuration. It determines what the dafault value for wrapper options will be. If it's not defined, it defaults to `OFF` when the project is a subdir, and `AUTO` if it's root.
    * `AC_C_WRAPPER`: build C wrapper. `AUTO` equals `ON`. C should always be available if you have C++.
    * `AC_JAVA_WRAPPER`: build Java wrapper. `AUTO` checks if JNI is available
    * `AC_COCOA_WRAPPER`: build Cocoa wrapper. `AUTO` checks for `APPLE`
    * `AC_SWIFT_WRAPPER`: build Swift wrapper. `AUTO` checks if `swiftc` is available
* Only available when ac-local is the root project and `OFF` by default:
    * `SAN_ADDR`: enable address sanitizer.
    * `SAN_UB`: enable undefined behavior sanitizer. Not supported on Windows.
    * `SAN_LEAK`: enable leak sanitizer. Only supported on Linux.
    * `SAN_THREAD`: enable thread sanitizer. Not supported on Windows.

### Troubleshooting

#### SDL2 is installed, but CMake still can't find it

You likely have to add the path to the SDL2 installation to the `CMAKE_PREFIX_PATH` environment variable. Or configure with `-DCMAKE_PREFIX_PATH=<path-to-sdl2>`

## Build

`$ cmake --build <path-to-build-dir>` (for example `$ cmake --build out/build/release`)

After this the produced binaries will be in `<path-to-build-dir>/bin`.

You can run the tests with `ctest` from the build directory.

## Install

*TBD*
