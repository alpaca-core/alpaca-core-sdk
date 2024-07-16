# llama.cpp

This is a wrapper of llama.cpp implemented as per the discussion [Integration of llama.cpp and whisper.cpp](https://github.com/alpaca-core/alpaca-core/discussions/5):

* Use the llama.cpp C interface in llama.h
* Reimplement the common library

As mentioned in the discussion the (maybe distant) future plan is to ditch llama.cpp by reimplementing entirely with vanilla ggml and a C++ interface.

## Reimplementation Notes:

* Better error handling, please
* GGUF metadata access (`llama_model_meta_*`) is not great. We should provide a better interface
* `llama_chat_apply_template` does not handle memory allocation optimally. There's a lot of room for improvement
