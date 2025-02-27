# Alpaca Core SDK

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) [![Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20) [![Build](https://github.com/alpaca-core/alpaca-core/actions/workflows/build.yml/badge.svg)](https://github.com/alpaca-core/alpaca-core/actions/workflows/build.yml)

> [!IMPORTANT]
> This project is still in in an alpha stage of development. Significant changes are very likely, or rather, certain, and backwards compatibility is disregarded.

The Alpaca Core SDK is a multi-platform SDK for building applications which include an abstract compute-heavy layer. It is the basis of [acord](https://github.com/alpaca-core/acord) and is designed to be the basis of the future Alpaca Core Software Suite.

> [!NOTE]
> So, what currently popular thing is a compute-heavy layer? Yes. Its main purpose is AI inference. It doesn't *have* to be used for AI inference, but that's what we build it for. Read more about the company and our mission [here](https://www.alpacacore.com/company/about).

The main features of the SDK are:

* Specifies the Alpaca Core Stateful Session Protocol.
* An asynchronous socket-like C++ implementation of the protocol.
* Not-just-C++ tooling and utilities for building and consuming ACSSP-based APIs.
* Tooling and utilities for building and loading C++ plugins which implement such APIs.
* Designed to completely abstract away the underlying compute from the application code.

Read the [full introduction here](doc/intro.md).

## Supported Compute APIs

The SDK on its own does not implement any computation (and thus no AI models or inference engines). It only contains the utilities for building and loading plugins which implement it. 

Some libraries which have AC Plugins include:

* By Alpaca Core:
    * [ilib-llama.cpp](https://github.com/alpaca-core/ilib-llama.cpp): Multiple LLM-s by wrapping [ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp)
    * [ilib-whisper.cpp](https://github.com/alpaca-core/ilib-whisper.cpp): Whisper ASR by wrapping [ggerganov/whisper.cpp](https://github.com/ggerganov/whisper.cpp)
    * [ilib-sd.cpp](https://github.com/alpaca-core/ilib-sd.cpp): Image generation with Stable Diffusion by wrapping [leejet/stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)
    * [ilib-ac-tortoise](https://github.com/alpaca-core/ilib-ac-tortoise): TTS with tortoise. Based on [balisujohn/tortoise.cpp](https://github.com/balisujohn/tortoise.cpp)

*This list will be updated as new models are added.*

## Bindings, Wrappers, and Integrations

This repo contains the reference implementation of the Stateful Session Protocol. It is implemented in C++, and thus the C++ public libraries and tools and *their* documentation are also hosted here. Additionally there are bindings, wrappers, and integrations for other languages and platforms. Their documentation is hosted in, and accessible from their respective repositories:

* By Alpaca Core:
    * *Coming soon*

## Minimal Example

```cpp
ac::local::Lib::loadAllPlugins(); // load all plugins
ac::frameio::BlockingIoCtx blockingCtx; // context for blocking IO from our side 
                                        // (easier to write demos in)
ac::local::IoCtx io; // io context for the plugins

auto& llamaProvider = ac::local::Lib::getProvider("llama"); // get the compute provider

// create a connection and attach it to a schema-based io helper
// it being schema based allows us to use strong types below
ac::schema::BlockingIoHelper llama(io.connect(dummyProvider), blockingCtx); 

namespace schema = ac::schema::llama; // shorthand for the llama schema

// await the connection and load a model
llama.expectState<schema::StateInitial>();
llama.call<schema::StateInitial::OpLoadModel>({.ggufPath = "/path/to/model.gguf"});

// start an inference instance
llama.expectState<schema::StateModelLoaded>();
llama.call<schema::StateModelLoaded::OpStartInstance>({.instanceType = "general"});

// run the inference to complete a piece of text
llama.expectState<schema::StateInstance>();
auto result = llama.call<schema::StateInstance::OpRun>({
    .prompt = "If you could travel faster than light,"
});

std::cout << result.text << "\n";
```

## Demos

Most inference libraries with AC plugins have simple examples in their respective repositories. Additionally:

* [alpaca-core/acord](https://github.com/alpaca-core/acord), our only product so far, makes use of all SDK features.
* [alpaca-core/tpl-ac-local-app](https://github.com/alpaca-core/tpl-ac-local-app) is a bare-bones template for building a C++ application with the SDK.
* The plugins listed above demonstrate how to create plugins.
* We're working on more standalone demos, mainly for mobile, to be made available soon.

## Usage

Check out the [guide on getting started](doc/getting-started.md).

## Contributing

Check out the [contributing guide](CONTRIBUTING.md).

## License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This software is distributed under the MIT Software License. See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2024-2025 [Alpaca Core, Inc](https://alpacacore.com).

## Third Party Libraries

[A list of the third party libraries used here](third-party.md). Please consider supporting them.

Additionally, if you deploy this software as binary, please consider including `etc/ac-local-deploy-licenses.md` in your deployment.
