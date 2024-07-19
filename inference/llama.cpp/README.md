# llama.cpp

This is a wrapper of llama.cpp implemented as per the discussion [Integration of llama.cpp and whisper.cpp](https://github.com/alpaca-core/alpaca-core/discussions/5):

* Use the llama.cpp C interface in llama.h
* Reimplement the common library

As mentioned in the discussion the (maybe distant) future plan is to ditch llama.cpp by reimplementing entirely with vanilla ggml and a C++ interface.

## Reimplementation Notes:

* Better error handling, please
* GGUF metadata access (`llama_model_meta_*`) is not great. We should provide a better interface
* `llama_chat_apply_template` does not handle memory allocation optimally. There's a lot of room for improvement
    * as a whole, chat management is not very efficient. `llama_chat_format_single` doing a full chat format for a single message is terrible
* Give vocab more visibility
* The sampling prev buffer is a `vector` but is essentially a ring buffer. That's because `llama_sample_repetition_penalties` accepts a span. Can we do something about that and use an actual ring buffer?
* Token-to-text can be handled much more elegantly by using plain ol' `string_view` instead of copying strings. It's not like tokens are going to be modified once the model is loaded
    * If we don't reimplement, perhaps keeping a parallel array of all tokens to string would be a good idea