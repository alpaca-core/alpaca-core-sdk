// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "FileUri.hpp"
#include <furi/furi.hpp>
#include <astl/throw_stdex.hpp>
#include <charconv>

namespace ac::local::fs {

std::string FileUri_toPath(std::string_view uri) {
    auto p = furi::uri_split::get_path_from_uri(uri);
    if (p.empty() || p.front() != '/') {
        throw_ex{} << "file URI must be absolute: " << uri;
    }
    if (p.size() >= 3 && p[2] == ':') {
        // assume windows path
        p.remove_prefix(1);
    }

    std::string ret;
    ret.reserve(p.length());

    // replace %xx with the actual character
    for (size_t i = 0; i < p.length(); ++i) {
        if (p[i] == '%' && i + 2 < p.length()) {
            char c;
            auto res = std::from_chars(p.data() + i + 1, p.data() + i + 3, c, 16);
            if (res.ec == std::errc{}) {
                ret.push_back(c);
                i += 2;
            }
            else {
                throw_ex{} << "invalid percent encoding in URI: " << uri;
            }
        }
        else {
            ret.push_back(p[i]);
        }
    }

    return ret;
}

std::string FileUri_fromPath(std::string_view path) {
    std::string ret = "file://";
    if (!path.empty() && path.front() != '/') {
        ret += '/';
    }

    // reserve enough space for the path
    // if escapes do happen, we'll need to reallocate
    ret.reserve(ret.size() + path.size());

    for (char c : path) {
        switch(c) {
        case '%':
            ret.push_back('%');
            ret.push_back('2');
            ret.push_back('5');
            break;
        case ' ':
            ret.push_back('%');
            ret.push_back('2');
            ret.push_back('0');
            break;
        default:
            ret.push_back(c);
            break;
        }
    }

    return ret;
}

} // namespace ac::local::fs
