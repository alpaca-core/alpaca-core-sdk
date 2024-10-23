# Alpaca Core Local SDK

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) [![Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20) [![Build](https://github.com/alpaca-core/alpaca-core/actions/workflows/build.yml/badge.svg)](https://github.com/alpaca-core/alpaca-core/actions/workflows/build.yml)

The Alpaca Core Local SDK, or AC Local for short, is a multi-platform SDK for local AI Inference.

"Local" here means running on the device which executes the binaries. This could be a server, a desktop, or a mobile device.

It provides a unified API for multiple inference models. The API itself can be split into two layers:

* Programming-language specific: The API which one calls writing code in a specific programming language. It's just a means to call the:
* Inference API: A JSON/CBOR/POJO-like API which is used to communicate with the underlying inference engines following their specific API schema.

## Supported models

This is the list of currently supported models. It will be updated as new models are added.

* Multiple LLM-s through [llama.cpp](https://github.com/ggerganov/llama.cpp)
* Whisper through [whisper.cpp](https://github.com/ggerganov/whisper.cpp)

## Minimal Example

```cpp
ac::local::ModelFactory factory;
ac::local::addLlamaInference(factory);

auto model = factory.createModel(
    {
        .inferenceType = "llama.cpp",
        .assets = {
            {.path = "/path/to/model.gguf"}
        }
    }, {}, {}
);

auto instance = model->createInstance("general", {});

auto result = instance->runOp("run",
    {{"prompt", "If you could travel faster than light,"}}, {});


std::cout << result << "\n";
```

## Bindings, Wrappers, and Integrations

* [C wrapper](wrapper/c)
* [Java wrapper](wrapper/java)
* [Swift wrapper](wrapper/swift)
* [Cocoa DictConverter](wrapper/cocoa) - Convert `NSDictionary` to `ac::Dict` and back in Objective-C++ 

## Demos

## Documentation

## Build

*The repo has submodules. Don't forget to fetch them.*

Use CMake. Works as a root or as a subdirectory. Some useful presets are provided in the repo.

Detailed build instructions can be found in the [documentation](doc/dev/build.md).

## License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This software is distributed under the MIT Software License. See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2024 [Alpaca Core, Inc](https://github.com/alpaca-core)

## Third Party Libraries

[A list of the third party libraries used here](third-party.md). Please consider supporting them.
