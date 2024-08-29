# Model Infos and Assets

This documents the flow of model infos and assets in the SDK. The majority of the code and handling is situated in `local-provider`.

## Model Info

The model info is a description of a model. A model info is part of a model manifest.

The concrete structure of the info is not yet defined. A discussion about it can be found [here](https://github.com/alpaca-core/alpaca-core/discussions/55)

What is defined is the list of assets associated with the model. Assets are listed by id (and optional tag) only. See how assets are managed below. The important thing here is that model infos only have asset ids.

### Model info flow

A provider would gets model sources, which can provide their model manifests. The provider merges the model manifests to form a single one. Currently, however thera are no model sources. Instead model infos are manually fed to the provider with code. Specifying model sources is a discussion topic [here](https://github.com/alpaca-core/alpaca-core/discussions/61)

The single model manifest does not just contain model infos. It has **materialized** model infos which include:

* Concrete asset data, collected from the asset manager
* Provider-specific data as discussed in the discussion linked above

Materialized model infos are stored as shared pointers which are then provided as query results and to local inference models. Thus if a materialized model info is updated, some endpoints might contain outdated data (the shared pointers to the previous version).

Other questions remain here:

* What is the query method of providers?
* Minor, but still. How are model manifests merged? Some kind of precedence is needed.

## Assets

No centralized asset manager daemon is allowed. Since mobile devices and the browser are targets, we can't have a daemon running. Server/desktop solutions would potentially benefit from one and we can empower them to make it, but we can't rely on it.

Thus assets are managed by a process-local asset manager, which uses potentially multiple asset sources. The implementation must assume that there are other processes which touch the same assets. How this happens is not defined yet, and is discussed [here](https://github.com/alpaca-core/alpaca-core/discussions/52) along with and the potential concurrency issues and races.

The current asset manager and sources do not take this into account.

### Asset sources

The asset sources implement the bulk of the functionality here. They are implemented as an abstract interface which users can extend with concrete implementations according to their needs. Asset sources:

* Determine whether they can provide an asset when queried by an id. How that's done is a matter of user implementation
* Fetch the asset and provide a file system path to it, when requested. 
    * Even now it's clear that providing a file system path is not the best idea. This blocks us from using alternative means of storage (like apk-based one). The further abstraction of asset i/o is discussed [here](https://github.com/alpaca-core/alpaca-core/discussions/53)
    * It is the responsiblity of the implementation to link the asset id to a uri or concrete asset source for it. This can be done by working with some asset manifest or database. The asset manager is not responsible for helping here. At best if could provide an io context in the spirit of `boost::asio` though this is not yet specified.

With this multiple download protocols can be supported. We will eventually provide plain HTTP downloads, but in the future implementations for S3 or more exotic sources can be made.

### Asset manager

The asset manager itself is simply an interface to a collection of asset sources sorted by priority. What it does is simply forward the user requests to the list of sources respecting the priority.

This means it's the user's responsibility to prioritize the sources. If an asset can be found in multiple sources, the user should take care to make the source which is more convenient to them appear before the others.

The motivating example here is having a local server with some models, which can also be found publicly (say on our own server). Since downloading from the local one is considerably faster, the user would want to prioritize it before the public one and only query the public one if the local one doesn't have the asset.

## Misc notes

Some notes of things that have been considered:

* Likely the answer here is no, but do we need a source for model sources? The current idea is to have one hardcoded and if users want a source of sources they can implement it themselves. Still in a future where the SDK is successful, this might be a good idea.
* Many models share certain parts (vocabularies, sometimes weights). Ollama makes use of them and treats models in a way similar to how Docker treats containers (it is made by people from Docker after all). Maybe we should explore this.
