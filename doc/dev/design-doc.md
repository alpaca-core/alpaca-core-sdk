# Design and Architecture

A collection of somewhat loose notes on the design and architecture of the SDK. This document is not yet completely structured and will likely be transformed into multiple documents in the future.

## Assumptions

* This is an SDK which is suitable to become *The* AI Inference SDK
* The target platforms will eventually be **all**: Mobile, Desktop, Browser, Embedded. If it has a C++ compiler, it's a potential target.
* The supported inference runners should be edge *and* server. This means that using the SDK one should be able load models and run inference on the device for which the code is written or on a remote server.
* The SDK should be agnostic about model types. It should provide abstract inputs and outputs, not necessarily bound to classes like "text", or "image", or "audio".

## API

### Some scenarios

* User loads an LLM, creates an instance and has a chat session
* User loads an LLM and prompts it to generate some text, then closes the instance
* User loads Whisper, partially and only encodes audio
    * Then in the future only loads the Whisper decoder and decodes the saved results

### API Design

* The API is a first-class C++ API: It uses classes, throws exceptions, and doesn't shy away from C++ library types like `std::function` or `std::shared_ptr`.
    * **Rationale**:
      * While C *is* the Lingua Franka of software engineering, C++ is simply easier to use.
      * It's much easier to design and maintain a C++ API without paying the price for the abstraction. Templates, and destructors provide a way to expose complex behavior without burdening the users with a huge number of specific functions and gotchas. Exceptions provide a natural way to handle errors.
      * Moreover many languages (like Java, Python, Lua, and others) provide ways to bind a C++ API in a much more natural way than a C one (including higher-order C++ constructs). It would be a shame to miss-out on this.
      * This is not to ban a C interface. A C interface will necessarily exist. Many languages would have an easier time with C. Still the "natural" programming language of the API and the only way to access low-level features would be C++.
      * There are multiple arguments in favor of going C first if we're shipping a closed-source library. Things to consider in this case would be binaries for special instruction sets, using the library as a plugin with `dlopen`/`dlsym` and more. However this is not the case here. Moreover even if were, we could still cover all the needs by using [DynaMix](https://github.com/iboB/dynamix)
    * We will provide API wrappers for popular languages
    * Eventually we will provide REST and WebSocket interfaces to the API.
    * The C wrapper will be part of this repo.
    * Wrappers for other languages would come in separate repos using this one as a submodule or package.
* SDK Glossary:
    * Provider: an "instance" of the API of sorts.
        * ... which also include a meta-provider: a provider which internally manages multiple concrete providers.
        * A provider might be: "This device", "A remote device/server"
        * A provider maintains a list of available models
        * A provider can be used to download or load models
        * A provider manages models and assets.
        * Multiple model sources can be assigned to a provider.
    * Model source: a model manifest source
        * It has a manifest of available models
        * It supports queries for the fields of the model description. Think SQL relational databases, though the query language will likely not be SQL.
    * Model: a collection of parameters.
        * The model manifest can be available regardless of whether the model is loaded.
        * A model must be loaded to create a...
    * Intance: an inference instance whose state persists while the instance is still "alive".
        * Multiple instances can be created from a model.
        * Instances can have inference sessions. Multiple consecutive sessions are allowed for an instance.
        * Different instances and their respective sessions can be concurrent. At least in theory. Code should treat them as such.
        * Sessions can execute ops.
    * Op: a computation performed by an instance session
        * Multiple ops can be executed during a session.
        * The ops in a session are sequential
        * Queueing multiple ops is supported.
        * Ops can change the session state (say kv cache) and this change affects subsequent ops
            * For example a chat would be composed of a session with multiple ops: one for each input by the user.
        * Some ops can, however, clear the state or parts of it
            * The most basic example is starting a new session
            * Thus instances can be reused to save time spent on initialization (kv cache, etc)
* All api calls are async, with callbacks
    * Callbacks are not necessarily the best way to approach async programming in C++, but they are the most portable and the easiest to bind to other languages. Moreover they can be wrapped by futures or coroutines (yes, a C++ wrapper of the C++ API, yay).
    * Having all calls async will make the use of the API seamless, regardless of whether the inference is local or remote.
* Somewhat stringly typed arguments
    * Given the vast and impossible to foresee number of combinations for API calls, we're providing most params to most ops as key-value maps/dictionaries.
    * The carrier for these is [nlohmann::json](https://github.com/nlohmann/json)
        * The type is essentially a key-value BSON compatible map
        * Values are number (optionally integer), string, array, object, null, and crucially *blob*.
        * It supports [JSON Schema](https://json-schema.org/) via [pboettch/json-schema-validator](https://github.com/pboettch/json-schema-validator)
    * In the initial implementation the C API will use `const char*` JSON strings, with blobs as base64 strings. Later we will provide a C wrapper to set values
    * The C++ API will use `nlohmann::json` directly `typedef`-ed to `Dict`.
    * In the future we may provide ways to supply params via Protobuf, Flatbuffers, or even [WIT](https://github.com/WebAssembly/component-model/blob/main/design/mvp/CanonicalABI.md) ([bindgen](https://github.com/bytecodealliance/wit-bindgen))

## Project Structure

* The project is composed of multiple libraries and tools. Their build can be controlled by build flavors. For example, one wouldn't (usually) build the server for mobile targets.
* Inference libraries can be used separately form the API above. One would be able to instantiate, say LLaMa, if they have weights and call functions directly, without worrying about Providers and model or asset soruces.
* Third party libraries which are likely to be modified by us are forked and added as submodules.
* Ones that are not likely to be modified are added as dependencies with [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) (in the future other package managers may be used if there is a need)

## Other

* Coding style:
    * 4-space indentation
    * [Karisik](https://github.com/iboB/karisik-coding-style) for C++:
        * `.cpp`, `.hpp` file extensions
        * `PascalCase` for types
        * `camelCase` for functions and variables
        * `ALL_CAPS` for macros
        * `Mixed_Case` for constants.
        * No `get` prefix on getters.
        * `m_` prefix on members.
        * `PascalCase` for filenames.
        * `PascalCase.camelCase` for functional extensions
    * Simple for C
        * `.c`, `.h` file extensions
        * `c_case_for_everything` including filenames
        * ...except macros which are still `ALL_CAPS`
* Commit messages:
    * use imperative mood: "add feature", "move functionality to foo"
    * `topic: short description`
        * Except when introducing a new topic, where the new topic the message: `add llama inference`. In such cases the implied topic is "project".
    * Use `, ref #<issue number>` if the commit references an issue
    * Use `, closes #<issue number>` if the commit closes an issue
    * Use `, fixes #<issue number>` if the commit fixes a bug
* _TODO_ sections:
    * create an issue, when adding _TODO_ in the code
    * use similar syntax to `TODO: <description>, ref #<issue number>`