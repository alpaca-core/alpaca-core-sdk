// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#import <Foundation/Foundation.h>

#include <ac/Dict.hpp>

@interface DictionaryWrapper : NSObject

+ (ac::Dict)convertToACDict:(NSDictionary<NSString *, id> *)dictionary
    NS_SWIFT_UNAVAILABLE("This method is unavailable in Swift");

+ (NSDictionary<NSString *, id> *)convertToDictionary:(const ac::Dict&)json
    NS_SWIFT_UNAVAILABLE("This method is unavailable in Swift");

@end

void convertAndPrintDictionary(NSDictionary<NSString *, id> *dictionary);
