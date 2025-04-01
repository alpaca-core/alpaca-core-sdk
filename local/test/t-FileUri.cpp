// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/fs/FileUri.hpp>
#include <doctest/doctest.h>
#include <vector>
#include <utility>


TEST_CASE("FileUri") {
    using pvec = std::vector<std::pair<std::string, std::string>>;

    pvec dualTests = {
        {"file:///tmp/foo", "/tmp/foo"},
        {"file:///tmp/foo%20bar", "/tmp/foo bar"},
        {"file:///home/user/file.txt", "/home/user/file.txt"},
        {"file:///home/user/file%20with%20spaces.txt", "/home/user/file with spaces.txt"},
        {"file:///C:/Users/x/My%20Documents/some%20file.doc", "C:/Users/x/My Documents/some file.doc"},
    };

    pvec monoTests = {
        {"file:///D:/my%20dir%20with%20spaces/file?query=1", "D:/my dir with spaces/file" },
        {"file:///C:/Users/x/My%20Documents/some%20file.doc#fragment", "C:/Users/x/My Documents/some file.doc" },
        {"file:///C:/Users/x/My%20Documents/some%20file.doc?query=1#fragment%20with%20space", "C:/Users/x/My Documents/some file.doc" },
    };

    using namespace ac::local::fs;

    for (const auto& [uri, path] : dualTests) {
        CHECK(FileUri_toPath(uri) == path);
        CHECK(FileUri_fromPath(path) == uri);
    }

    for (const auto& [uri, path] : monoTests) {
        CHECK(FileUri_toPath(uri) == path);
    }
}
