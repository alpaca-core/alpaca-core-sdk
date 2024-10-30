# Minor Design Notes

This is a loose and not yet structured collection of notes on the design decisions of various components, but not the API or SDK as a whole.

## Using submodules *and* packages

Usually it's one of the two. Usually it's packages. Submodules are often a bad choice.

In our case we have public forks of existing open source repos (such as llama.cpp, ggml, etc...). We also might have custom changes to them (ideally we would make PRs but if they are not accepted or until they are accepted we still need to use the changes). 

That's why for these cases - code that would normally be in the repo itself, but it is a part of a fork of a public repo - we use submodules.

For non-fork third party cases we use packages (or, occasionally, code spliced right in here)

## Using SDL2/ImGui for examples

We want to avoid platform-specific code here, but still want to create some GUI demos. 

There are many multi-platform graphical libraries which would allow Us to boot OpenGL + ImGui (in fact my first choice would have been [sokol](https://github.com/floooh/sokol) and my second [GLFW](https://www.glfw.org/)) but SDL also gives us access to audio and video (including av input), which will be useful when we get to these types of inputs.

## Coroutine-based inference

Streaming inference will definitely be a requirement (if it isn't already). Coroutines are a good way to handle streaming i/o. Otherwise we would end up either with callback hell or with classes which essentially reimplement coroutines.

## Seemingly excessive subdirs

This is to restrict access to headers of targets which are not link-libraries of a given target. You won't be able to accidentally include `ac/local/Model.hpp` if you don't link against `ac::local`.

## Synchronous API

It can be argued that an async API would be better. On first glance it would:

* Allow us to abastract away the actual inference runner (local, or multiple local, or remote). 
* Make it easier to integrate ac-local in a GUI or a Web application.

Because of this we initially did go with an async API. Here's the thought process which led to the current design:

* On edge devices the inference usually takes up almost all system resources. Running multiple inferences in parallel is not a good idea. Thus load balancing and multiple local workers are practically not an option. If you only have a single worker, plugging the sync API into an async implementation is trivial. Moreover, it would allow the wrapping async interface to be tailored to the specific needs of the application. 
* Wrapping the local inference API for multiple languages and platforms has always been the plan. Since different languages have different async paradigms or standards, creating an async API that would suit them all would enforce tradeoffs in usability. Our initial implementation used callbacks, as callbacks are the only C++ construct that can be translated to anything. But turning callbacks into coroutines, for example, is pretty unpleasant. Turning them into futures would require a lot of boilerplate. So again the synchronous API makes more sense here as it would allow users to wrap it in a way that best suits their language or platform.
* An inference server would almost certainly provide a different API than AC Local. AC Local's API is low level. It requires manual management of models, instances, and sessions. It makes no sense to use this low level interface for a remote inference server. So while abstracting away locality is certainly a goal for the future, it would be a separate project with a different, higher level API
* Writing the server itself would make use of AC Local. The usage there would be asynchronous. It would potentially have multiple workers and load balancing. These features and their configurations simply make no sense for edge devices. So, they will just be a separate project. A project which does not have to conform to potential wrappers in other languages, edge use cases, and the like.
