// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#import <Foundation/Foundation.h>

#include <ac/Dict.hpp>

@interface DictionaryWrapper : NSObject

- (ac::Dict)convertToJSON:(NSDictionary<NSString *, id> *)dictionary
    NS_SWIFT_UNAVAILABLE("This method is unavailable in Swift");

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONStringFromDict:(NSDictionary<NSString *, id> *)dictionary;

@end
