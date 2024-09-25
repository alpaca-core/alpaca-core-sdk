// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#import <Foundation/Foundation.h>
#import <iostream>

#include <ac/Model.hpp>
#include <ac/Dict.hpp>s

NSString* runPayload();


@interface MyObjCWrapper : NSObject

+ (int)getIntegerFromCPP; // Expose the C++ function as an Objective-C method

@end

//NS_ASSUME_NONNULL_BEGIN

@interface DictionaryWrapper : NSObject

// Init with a dictionary
- (instancetype)initWithDictionary:(NSDictionary<NSString *, id> *)dictionary;

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONString;

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONStringFromDict:(NSDictionary<NSString *, id> *)dictionary;

@end

//NS_ASSUME_NONNULL_END
