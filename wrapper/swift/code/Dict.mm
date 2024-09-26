// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "Dict.h"

#include <ac/Model.hpp>
#include <ac/Dict.hpp>

@implementation DictionaryWrapper

// Helper method to convert NSDictionary to ac::Dict
- (ac::Dict)convertToJSON:(NSDictionary<NSString *, id> *)dictionary {
    ac::Dict jsonObj;

    for (NSString *key in dictionary) {
        id value = dictionary[key];

        if ([value isKindOfClass:[NSDictionary class]]) {
            jsonObj[key.UTF8String] = [self convertToJSON:(NSDictionary *)value];
        }

        if ([value isKindOfClass:[NSArray class]]) {
            ac::Dict arrayJson = ac::Dict::array();
            for (id item in (NSArray *)value) {
                if ([item isKindOfClass:[NSDictionary class]]) {
                    arrayJson.push_back([self convertToJSON:(NSDictionary *)item]);
                } else if ([item isKindOfClass:[NSString class]]) {
                    arrayJson.push_back(std::string([(NSString *)item UTF8String]));
                } else if ([item isKindOfClass:[NSNumber class]]) {
                    arrayJson.push_back([item doubleValue]); // Handle numbers
                }
            }
            jsonObj[key.UTF8String] = arrayJson;
        }
        if ([value isKindOfClass:[NSString class]]) {
            jsonObj[key.UTF8String] = std::string([(NSString *)value UTF8String]);
        }
        if ([value isKindOfClass:[NSNumber class]]) {
            jsonObj[key.UTF8String] = [value doubleValue]; // Handle numbers
        }
        if ([value isKindOfClass:[NSData class]]) {
            const uint8_t* bytes = (const uint8_t*)[value bytes];
            jsonObj[key.UTF8String] = ac::Dict::binary(ac::Blob(bytes, bytes + [value length]));
        }
    }

    return jsonObj;
}

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONStringFromDict:(NSDictionary<NSString *, id> *)dictionary {
    ac::Dict jsonObj = [self convertToJSON:dictionary];

    // Convert nlohmann::json to string and then to NSString
    std::string jsonString = jsonObj.dump();
    return [NSString stringWithUTF8String:jsonString.c_str()];
}

@end
