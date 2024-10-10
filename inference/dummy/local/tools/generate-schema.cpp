// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/DummyModelSchema.hpp>
#include <iostream>

int main() {
    ac::schema::OrderedDict out;
    ac::local::schema::Dummy::describe(out);
    std::cout << out.dump(2) << "\n";
    return 0;
}
