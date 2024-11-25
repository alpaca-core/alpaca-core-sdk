# Build

> If you're not only interested in building the project but also setting up a development environment, you might want also to check out the [development environment documentation](dev-env.md).

## Prerequisites

The minimum requirements are:

* C++ compiler with C++20 support such as:
    * MSVC 19.30 (Visual Studio 2022 v17.00) or later
    * GCC 11 or later
    * Clang 14 or later
* [CMake](https://cmake.org/) 3.24 or later
* For some build flavors and optional components:
    * GUI Demos: [SDL2](https://wiki.libsdl.org/SDL2/FrontPage)
    * Different computational backends require their respective SDKs and dependencies

If you want practical steps on for specific environments, check the [developer environments documentation](dev-env.md).

## Configure

### CMake Options

* Advanced and `OFF` by default if the project is a subdir:
    * `AC_LOCAL_BUILD_TOOLS` - build tools for all components. No finer-grain options are available yet
    * `AC_LOCAL_BUILD_TESTS` - build tests for all active components. No finer-grain options are available yet. Git LFS is required for some tests
    * `AC_LOCAL_BUILD_EXAMPLES` - build examples for all active components. No finer-grain options are available yet. Git LFS is required for some examples
* From ac-build and only available when ac-local is the root project and `OFF` by default:
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
