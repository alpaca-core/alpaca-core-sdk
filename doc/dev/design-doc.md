# Design and Architecture

## Assumptions

* This is an SDK which is suitable to become *The* AI Inference SDK
* The target platforms will eventually be **all**: Mobile, Desktop, Browser, Embedded. If it has a C++ compiler, it's a potential target.
* The supported inference runners should be edge *and* server. This means that using the SDK one should be able load models and run inference on the device for which the code is written or on a remote server.
* The SDK should be agnostic about model types. It should provide abstract inputs and outputs, not necessarily bound to classes like "text", or "image", or "audio".

## API

* The API is a first-class C++ API: It uses classes, throws exceptions, and doesn't shy away from C++ library types like `std::function` or `std::shared_ptr`.
    * **Rationale**: While C *is* the Lingua Franka of software engineering, C++ is simply easier to use. It's much easier to design and maintain a C++ API without paying the price for the abstraction. Templates, and destructors provide a way to expose complex behavior without burdening the users with a huge number of specific functions and gotchas. Exceptions provide a natural way to handle errors. Moreover many languages (like Java, Python, Lua, and others) provide ways to bind a C++ API in a much more natural way than a C one (including higher-order C++ constructs). It would be a shame to miss-out on this. Finally this is not banning a C interface. A C interface will necessarily exist. Many languages would have an easier time with C. Still the "natural" programming language of the API and the only way to access low-level features would be C++.
    * We will provide API wrappers for popular languages
    * The C wrapper will be part of this repo.
    * Wrappers for other languages would come in separate repos using this one as a submodule or package.
* SDK Glossary:
    * Provider: an "instance" of the API of sorts.
        * ... which also include a meta-provider: a provider which internally manages multiple concrete providers.
        * A provider might be: "This device", "A remote device/server"
        * A provider maintains a list of available models
        * A provider can be used to download or load models
        * A provider downloads models from a remote.
        * Multiple remotes can be assigned to a provider.
    * Remote: a model repository
        * It has a manifest of available models
        * It supports SQL-like queries for the fields of the model description
    * Model: a collection of parameters.
        * If a model is not loaded it's manifest is available
        * A model must be loaded to create a...
    * Job: an inference job whose state persists while the job is "alive".
        * Multiple jobs can be created from a model.
        * Jobs can (in theory) be concurrent
        * Jobs can execute ops.
    * Op: a computation performed by a job
        * Multiple ops can be executed by a job.
        * The ops in a job are sequential.
        * Ops can change the job's state (say kv cache) and this change affects subsequent ops
            * For example a chat would be composed of a job with multiple ops: one for each input by the user.



## Project Structure

## Library


