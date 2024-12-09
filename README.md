# Alpaca Core Local SDK

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) [![Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20) [![Build](https://github.com/alpaca-core/alpaca-core/actions/workflows/build.yml/badge.svg)](https://github.com/alpaca-core/alpaca-core/actions/workflows/build.yml)

> [!NOTE]
> This project is still in in an alpha stage of development. Significant changes are very likely and backwards compatibility is disregarded.

The Alpaca Core Local SDK, or *AC Local* for short, is a multi-platform SDK for local AI Inference.

"Local" here means running on the device which executes the code. This could be a server, a desktop, or a mobile device.

It provides a unified API for doing inference with multiple models. The API itself can be split into two layers:

* Programming language specific (Language API): The API which one calls writing code in a specific programming language. It's just a means to call the:
* Inference API: A JSON/CBOR/POJO-like API which is used to communicate with the underlying inference engines following their specific API schema.

Read the [full introduction here](doc/intro.md).

## Supported models

The SDK on its own does not support any models. It contains the tools for building and loading plugins which provide inference for specific models. 

Some libraries which have AC Local Plugins include:

* By Alpaca Core:
    * [ilib-llama.cpp](https://github.com/alpaca-core/ilib-llama.cpp): Multiple LLM-s by wrapping [ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp)
    * [ilib-whisper.cpp](https://github.com/alpaca-core/ilib-whisper.cpp): Whisper ASR by wrapping [ggerganov/whisper.cpp](https://github.com/ggerganov/whisper.cpp)
    * [ilib-sd.cpp](https://github.com/alpaca-core/ilib-sd.cpp): Image generation with Stable Diffusion by wrapping [leejet/stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)
    * [ilib-ac-tortoise](https://github.com/alpaca-core/ilib-ac-tortoise): TTS with tortoise. Based on [balisujohn/tortoise.cpp](https://github.com/balisujohn/tortoise.cpp)

*This list will be updated as new models are added.*

## Bindings, Wrappers, and Integrations

This repo contains the Inference SDK implementation and Inference API documentation. The Inference SDK is implemented in C++, and thus the C++ Language API and *its* documentation are also hosted here. Additionally there are bindings, wrappers, and integrations for other languages and platforms. Their documentation is hosted in, and accessible from their respective repositories:

* By Alpaca Core:
    * [C wrapper](https://github.com/alpaca-core/ac-local-c)
    * [Java wrapper](https://github.com/alpaca-core/ac-local-java)
    * [Swift wrapper](https://github.com/alpaca-core/ac-local-swift)
    * [Cocoa DictConverter](https://github.com/alpaca-core/ac-dict-cocoa) - Convert `NSDictionary` to `ac::Dict` and back in Objective-C++

## Minimal Example

```cpp
ac::local::Lib::loadAllPlugins();

auto model = ac::local::Lib::createModel(
    {
        .type = "llama.cpp gguf",
        .assets = {
            {.path = "/path/to/model.gguf"}
        }
    }, 
    { /*default params*/ }
);

auto instance = model->createInstance("general", { /*default params*/ });

auto result = instance->runOp("run", {{"prompt", "If you could travel faster than light,"}});

std::cout << result << "\n";
```

## Demos

Most inference libraries with AC Local plugins have simple examples in their respective repositories. Additionally we have some standalone demos:

* AI Chat (LLM-based chatbot)
    * Android: [alpaca-core/demo-android-ai-chat](https://github.com/alpaca-core/demo-android-ai-chat)
    * IOS: [alpaca-core/demo-ios-ai-chat](https://github.com/alpaca-core/demo-ios-ai-chat)

## Usage

Check out the [guide on getting started](doc/getting-started.md).

## Contributing

Check out the [contributing guide](CONTRIBUTING.md).

## License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This software is distributed under the MIT Software License. See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2024 [Alpaca Core, Inc](https://github.com/alpaca-core)

## Third Party Libraries

[A list of the third party libraries used here](third-party.md). Please consider supporting them.

Additionally, if you deploy this software as binary, please include `etc/ac-local-deploy-licenses.md` in your deployment.
