// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "DummyACapi.h"

using json = nlohmann::json;

@implementation DictionaryWrapper

// Helper method to convert NSDictionary to nlohmann::json
- (ac::Dict)convertToJSON:(NSDictionary<NSString *, id> *)dictionary {
    //json jsonObj;
    ac::Dict jsonObj;

    for (NSString *key in dictionary) {
        id value = dictionary[key];

        if ([value isKindOfClass:[NSDictionary class]]) {
            jsonObj[key.UTF8String] = [self convertToJSON:(NSDictionary *)value];
        } else if ([value isKindOfClass:[NSArray class]]) {
            json arrayJson = json::array();
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
        } else if ([value isKindOfClass:[NSString class]]) {
            jsonObj[key.UTF8String] = std::string([(NSString *)value UTF8String]);
        } else if ([value isKindOfClass:[NSNumber class]]) {
            jsonObj[key.UTF8String] = [value doubleValue]; // Handle numbers
        }
    }

    return jsonObj;
}

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONStringFromDict:(NSDictionary<NSString *, id> *)dictionary {
    json jsonObj = [self convertToJSON:dictionary];

    // Convert nlohmann::json to string and then to NSString
    std::string jsonString = jsonObj.dump();
    return [NSString stringWithUTF8String:jsonString.c_str()];
}

@end
