// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelSchema.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string_view>
#include <charconv>
#include <filesystem>

namespace ac::local::schema {

template <typename Schema>
int generatorMain(int argc, char** argv) try {
    const std::string_view name = argv[0];
    const std::string_view shortName = Schema::id;

    int indent = 2;
    std::string path = "";
    std::string fname = std::string(shortName) + ".json";

    auto printUsage = [&]() {
        std::cout << "usage: $ " << name << " [<args>]\n"
            << '\n'
            << "generate a JSON schema for the model '" << shortName << "'\n"
            << '\n'
            << "args:\n"
            << "  -h, --help       Print this help message and exit\n"
            << "  --indent=<num>   Indent for a pretty JSON. -1 for compact.\n"
            << "                   Default: " << indent << "\n"
            << "  --path=<dir>     Path to the output directory.\n"
            << "                   Default: '" << path << "'\n"
            << "  --fname=<fname>  Filename for the output file. Appended to path.\n"
            << "                   Default: '" << fname << "'\n"
            << "  --stdout         Ignore path and output and print to stdout\n"
            << '\n';
    };

    bool outputToStdout = false;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        }
        else if (arg.starts_with("--indent=")) {
            arg = arg.substr(9);
            auto res = std::from_chars(arg.data(), arg.data() + arg.size(), indent);
            if (res.ec != std::errc()) {
                std::cerr << "Invalid indent: " << arg << '\n';
                return 1;
            }
        }
        else if (arg.starts_with("--path=")) {
            path = std::string(arg.substr(7));
        }
        else if (arg.starts_with("--fname=")) {
            fname = std::string(arg.substr(8));
        }
        else if (arg == "--stdout") {
            outputToStdout = true;
        }
        else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    std::ostream* out = nullptr;
    std::ofstream fout;

    if (outputToStdout) {
        out = &std::cout;
    }
    else {
        out = &fout;

        if (!path.empty()) {
            std::filesystem::create_directories(path);
            fout.open(path + "/" + fname);
        }
        else {
            fout.open(fname);
        }
    }

    OrderedDict d;
    Schema::describe(d);

    *out << d.dump(indent);

    if (indent != -1) {
        *out << '\n';
    }

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}

}  // namespace ac::local::schema
