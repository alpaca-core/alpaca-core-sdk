// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import DummyACModule
import Cxx
import CxxStdlib

public func ACDictNewRoot() -> OpaquePointer {
    return DummyACModule.ac_dict_new_root()
}

public func ACDict() {
    // To use the code below just add
    // the "initMap" function in Dict.hpp as:
    /*
        using Map = std::map<int, int>;
        inline Map initMap() { return {{1, 3}, {2, 2}, {3, 3}}; }
     */

    //var m = DummyACModule.ac.initMap()
    //let at1 = m[1]
    //print(at1)
}

// It's a top level struct with supported Swift->C++ types,
// si it's generated in the C++ header.
// Supported types more here: https://www.swift.org/documentation/cxx-interop/status/#supported-swift-standard-library-types
public struct Point {
    let x: Float
}

// all types used as params and return type are supported
// so the function is properly exported
public func myFunc(x: Float, c: Int) -> Point {
    let p = Point(x: x)

    return p
}

// non-exportable to C++ header since Dict
public func myFunc2() -> [String : String]{
    let a = [String : String]()
    return a
}
