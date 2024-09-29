// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <iostream>
#include <stdexcept>
#include <ahttp/ahttp.hpp>
#include <vector>

int main() try {
    auto dl = ahttp::get_sync("https://huggingface.co/alpaca-core/ac-test-data-llama/resolve/main/gpt2-117m-q6_k.gguf");

    std::vector<uint8_t> chunk(1024);
    auto ret = dl.get_next_chunk(chunk);

    std::cout << ret.size() << " bytes\n";

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}