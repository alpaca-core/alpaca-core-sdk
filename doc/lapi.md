# Language API

The language api is programming language specific. Its main role is to give representations of `Model`, `Instance`, `op`-s, and `Dict`, and a way to create or load models.

The Language API is synchronous. All calls block until they complete. In some cases optional callbacks may be provided, but they're always executed in the caller's call stack. 

The Language API is low level. It's meant to be wrapped in higher-level application-specific abstractions, which normally would not expose the Inference API directly (or at least not completely). For once asynchronicity is very likely to be needed by an implementation and it should be done in a way that best suits the needs, the language, and the platform. Implementing a server or a GUI application have vastly different requirements and thus would require different async implementations.

The main elements of the Language API are:

* An representation for `Dict` which is used to parameters and results
* An object for a model description, characterized by:
    * Inference type: used to route to the correct inference engine
    * Assets: a list of assets needed for the model. For now the assets are just paths to files on the filesystem, but this is likely to change (expanded with more options) in the future.
* A model factory, which creates models based on a description.
* An object for a model which can be used to create instances
* An object for an instance which can run ops
