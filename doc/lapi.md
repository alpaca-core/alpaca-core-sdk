# Language API

The language api is programming language specific, but all implementations share some common points:

## Elements

* A representation of `Dict` which is used to parameters and results
    * This includes representations of `Dict` values like `string`, `number`, `null`
    * ... and cruically also `binary`, which is used for arbitrary Inference-API-specific binary data
* An way to define a model description, characterized by:
    * Inference type: used to route to the correct inference engine
    * Assets: a list of assets needed for the model. For now the assets are just paths to files on the filesystem, but this is likely to change (expanded with more options) in the future.
* A model factory, which creates models based on a description.
* An representation of `Model` which can be used to create instances
* An representation of `Instance` 
    * ... which can be used to run `op`-s. Essentially `Dict -> Dict` functions

## Characteristics

The Language API is synchronous. It's meant to be wrapped in higher-level application-specific abstractions, which normally would not expose the Inference API directly (or at least not completely). For once asynchronicity is very likely to be needed by an implementation and it should be done in a way that best suits the needs, the language, and the platform. Implementing a server or a GUI application have vastly different requirements and thus would require different async implementations.

* **Synchronous**: All calls block until they complete. 
    * In some cases optional callbacks may be provided, but they're always executed in the caller's call stack. 
* **Manual lifetime management**:

