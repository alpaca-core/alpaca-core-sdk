// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <iostream>
#include <stdexcept>

int main() try {
   return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}