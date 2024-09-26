// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#import <Foundation/Foundation.h>
#import <iostream>

#include <ac/Model.hpp>
#include <ac/Dict.hpp>

@interface DictionaryWrapper : NSObject

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONStringFromDict:(NSDictionary<NSString *, id> *)dictionary;

@end
