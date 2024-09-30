// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "Dict.h"

#include <ac/Model.hpp>
#include <ac/Dict.hpp>

@implementation DictionaryWrapper

// Helper method to convert NSDictionary to ac::Dict
+ (ac::Dict)convertToACDict:(NSDictionary<NSString *, id> *)dictionary {
    ac::Dict jsonObj;

    for (NSString *key in dictionary) {
        id value = dictionary[key];

        if ([value isKindOfClass:[NSDictionary class]]) {
            jsonObj[key.UTF8String] = [self convertToACDict:(NSDictionary *)value];
        }

        if ([value isKindOfClass:[NSArray class]]) {
            ac::Dict arrayJson = ac::Dict::array();
            for (id item in (NSArray *)value) {
                if ([item isKindOfClass:[NSDictionary class]]) {
                    arrayJson.push_back([self convertToACDict:(NSDictionary *)item]);
                } else if ([item isKindOfClass:[NSString class]]) {
                    arrayJson.push_back(std::string([(NSString *)item UTF8String]));
                } else if ([item isKindOfClass:[NSNumber class]]) {
                    // Check if the integerValue and doubleValue are the same
                    // If they are the same then we get the integerValue
                    NSInteger intValue = [item integerValue];
                    double doubleValue = [item doubleValue];
                    if (intValue == doubleValue) {
                        arrayJson.push_back([item integerValue]); // Handle integer numbers
                    } else {
                        arrayJson.push_back([item doubleValue]); // Handle floating point numbers
                    }
                }
            }
            jsonObj[key.UTF8String] = arrayJson;
        }
        if ([value isKindOfClass:[NSString class]]) {
            jsonObj[key.UTF8String] = std::string([(NSString *)value UTF8String]);
        }
        if ([value isKindOfClass:[NSNumber class]]) {
            // Check if the integerValue and doubleValue are the same
            NSInteger intValue = [value integerValue];
            double doubleValue = [value doubleValue];
            if (intValue == doubleValue) {
                jsonObj[key.UTF8String] = [value integerValue]; // Handle integer numbers
            } else {
                jsonObj[key.UTF8String] = [value doubleValue]; // Handle floating point numbers
            }
        }
        if ([value isKindOfClass:[NSData class]]) {
            const uint8_t* bytes = (const uint8_t*)[value bytes];
            jsonObj[key.UTF8String] = ac::Dict::binary(ac::Blob(bytes, bytes + [value length]));
        }
    }

    return jsonObj;
}

// Helper function for determine the type of the value
+ (id)convertJSONValue:(const ac::Dict&)json {
    // Handle different JSON types
    if (json.is_object()) {
        return [self convertToDictionary:json];
    } else if (json.is_array()) {
        NSMutableArray *array = [NSMutableArray array];
        for (const auto &element : json) {
            [array addObject:[self convertJSONValue:element]];
        }
        return [array copy];
    } else if (json.is_string()) {
        return [NSString stringWithUTF8String:json.get<std::string>().c_str()];
    } else if (json.is_number_integer()) {
        return [NSNumber numberWithLongLong:json.get<int64_t>()];
    } else if (json.is_number_float()) {
        return [NSNumber numberWithDouble:json.get<double>()];
    } else if (json.is_boolean()) {
        return [NSNumber numberWithBool:json.get<bool>()];
    } else if (json.is_null()) {
        return [NSNull null];
    }
    return nil;
}

+ (NSDictionary *)convertToDictionary:(const ac::Dict&)json {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];

    for (auto it = json.begin(); it != json.end(); ++it) {
        NSString *key = [NSString stringWithUTF8String:it.key().c_str()];
        id value = [self convertJSONValue:it.value()];
        dict[key] = value;
    }

    return [dict copy];
}

@end

void convertAndPrintDictionary(NSDictionary<NSString *, id> *dictionary) {
    ac::Dict dict = [DictionaryWrapper convertToACDict:dictionary];

    std::string jsonString = dict.dump();
    NSLog(@"%s", jsonString.c_str());
}
