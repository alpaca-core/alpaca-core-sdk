// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#import <Foundation/Foundation.h>

#include <ac/Dict.hpp>

@interface DictConverter : NSObject

// Convert NSDictionary to ac::Dict
+ (ac::Dict)convertToACDict:(NSDictionary<NSString *, id> *)dictionary
    NS_SWIFT_UNAVAILABLE("This method is unavailable in Swift");

// Convert ac::Dict to NSDictionary
+ (NSDictionary<NSString *, id> *)convertToDictionary:(const ac::Dict&)dict
    NS_SWIFT_UNAVAILABLE("This method is unavailable in Swift");

@end
