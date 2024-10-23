# Third Party Libraries

> If I have seen further it is by standing on the shoulders of Giants. - Isaac Newton

Our software is powered by a number of third-party libraries. We are grateful to the authors and contributors of these libraries for their hard work and dedication. We have made an effort to list all the libraries we use, but if we have missed any, please let us know.

If you like our software, consider giving these libraries a star on GitHub, a donation, or contributing to them in some other way.

Here's the list in no particular order:

## In Alpaca Core Local itself

### Inference

| Library | Usage |
| --- | --- |
| [ggml](https://github.com/ggerganov/ggml) | inference runner |
| [llama.cpp](https://github.com/ggerganov/llama.cpp) | llama inference |
| [whisper.cpp](https://github.com/ggerganov/wisper.cpp) | whisper inference |

### Other

| Library | Usage |
| --- | --- |
| [JSON for Modern C++](https://github.com/nlohmann/json) | `ac::Dict` |
| [splat](https://github.com/iboB/splat) | C and C++ macros |
| [itlib](https://github.com/iboB/itlib) | C++ STL extensions library |
| [jalog](https://github.com/iboB/jalog) | logging |
| [Magic Enum](https://github.com/Neargye/magic_enum) | enum reflection |

## In Acord

| Library | Usage |
| --- | --- |
| [Boost.Asio](https://github.com/boostorg/asio) | networking |
| [Boost.Beast](https://github.com/boostorg/beast) | HTTP support |
| [OpenSSL](https://github.com/openssl/openssl) | HTTPS support |
| [xec](https://github.com/iboB/jalog) | task execution |
| [furi](https://github.com/iboB/furi) | URI parsing |
| [xxhash](https://github.com/Cyan4973/xxHash) | hash functions |

## In other demos and examples

| Library | Usage |
| --- | --- |
| [SDL2](https://github.com/libsdl-org/SDL) | graphics backend for GUI demos |
| [Dear ImGui](https://github.com/ocornut/imgui) | GUI library for some demos |
| [dr_libs](https://github.com/mackron/dr_libs) | audio decoding |

## In the build system and tests

| Library | Usage |
| --- | --- |
| [CPM.cmake](https://github.com/TheLartians/CPM.cmake) | package management |
| [iboB's CMake Modules](https://github.com/iboB/splat) | CMake modules |
| [doctest](https://github.com/onqtam/doctest) | testing |

## In the wrappers

| Library | Usage |
| --- | --- |
| [Unity Test](https://github.com/ThrowTheSwitch/Unity) | C testing |
| [jni.hpp](https://github.com/mapbox/jni.hpp) | JNI wrapper |
