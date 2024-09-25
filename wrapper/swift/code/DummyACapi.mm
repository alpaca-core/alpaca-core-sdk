// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "DummyACapi.h"

NSString* runPayload() {
    return @"pesho";
}

@implementation MyObjCWrapper

+ (int)getIntegerFromCPP {
    return 5;
}

@end


using json = nlohmann::json;

@implementation DictionaryWrapper {
    NSDictionary<NSString *, id> *_dictionary;
}

// Initialize with a dictionary
- (instancetype)initWithDictionary:(NSDictionary<NSString *, id> *)dictionary {
    self = [super init];
    if (self) {
        _dictionary = dictionary;
    }
    return self;
}

// Helper method to convert NSDictionary to nlohmann::json
- (json)convertToJSON:(NSDictionary<NSString *, id> *)dictionary {
    json jsonObj;

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
- (NSString *)toJSONString {
    json jsonObj = [self convertToJSON:_dictionary];

    // Convert nlohmann::json to string and then to NSString
    std::string jsonString = jsonObj.dump();
    return [NSString stringWithUTF8String:jsonString.c_str()];
}

// Convert to JSON string using nlohmann::json
- (NSString *)toJSONStringFromDict:(NSDictionary<NSString *, id> *)dictionary {
    json jsonObj = [self convertToJSON:dictionary];

    // Convert nlohmann::json to string and then to NSString
    std::string jsonString = jsonObj.dump();
    return [NSString stringWithUTF8String:jsonString.c_str()];
}

@end



