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
* Chat templates can't be used to escape special tokens. If the user actually enters some, this just messes-up the resulting formatted text.
* Give vocab more visibility
* Token-to-text can be handled much more elegantly by using plain ol' `string_view` instead of copying strings. It's not like tokens are going to be modified once the model is loaded
    * If we don't reimplement, perhaps keeping a parallel array of all tokens to string would be a good idea
* `llama_batch` being used for both input and output makes it hard to propagate the constness of the input buffer. This leads to code having to use non-const buffers, even if we know they're not going to be modified. We should bind the buffer constness to the batch struct itself.
* The low-level llama context currently takes a rng seed (which is only used for mirostat sampling). A reimplemented context should be deterministic. If an operation requires random numbers, a generator should be provided from the outside.
    * For now we will hide the mirostat sampling altogether and ditch the seed
* As per [this discussion](https://github.com/alpaca-core/alpaca-core/discussions/53) we should take into account how we want to deal with asset storage and whether we want to abstract the i/o away.
