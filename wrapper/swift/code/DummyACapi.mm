// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
NSString* runPayload() {
    std::stringstream ss;

    ss << "Running payload" << std::endl;

    return [NSString stringWithUTF8String:ss.str().c_str()];;
}
