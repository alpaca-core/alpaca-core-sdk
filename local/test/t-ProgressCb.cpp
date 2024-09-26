// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ProgressCb.hpp>
#include <doctest/doctest.h>
#include <memory>

TEST_CASE("capture non copyable") {
    ac::local::ProgressCb cb = [ptr = std::make_unique<int>(42)](std::string_view tag, float progress) {
        CHECK(*ptr == 42);
        CHECK(tag == "tag");
        CHECK(progress == doctest::Approx(0.5f));
        return true;
    };
    cb("tag", 0.5f);
}
