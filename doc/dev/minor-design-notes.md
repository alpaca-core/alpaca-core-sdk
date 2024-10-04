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
