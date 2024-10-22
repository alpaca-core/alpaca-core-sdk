// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "DictConverter.hpp"
#include <string_view>

#include <ac/Dict.hpp>

@implementation DictConverter

// Helper function to determine the type of the value
+ (ac::Dict)convertDictValue:(id)value {
    if ([value isKindOfClass:[NSDictionary class]]) {
        ac::Dict dict = [self convertToACDict:(NSDictionary *)value];
        return dict.is_null() ?  ac::Dict::object() : dict;
    }
    else if ([value isKindOfClass:[NSArray class]]) {
        ac::Dict arrayJson = ac::Dict::array();
        for (id item in (NSArray *)value) {
            arrayJson.push_back([self convertDictValue:item]);
        }
        return arrayJson;
    }
    else if([value isKindOfClass:[NSString class]]){
        return  std::string([(NSString *)value UTF8String]);
    }
    else if ([value isKindOfClass:[NSNumber class]]) {
        // Get the type of the NSNumber
        std::string_view typeStr = [value objCType];

        if (typeStr == @encode(BOOL) || typeStr == @encode(char)) {
            return [value boolValue];
        }
        // Handle integer values
        else if (typeStr == @encode(int) || typeStr == @encode(NSInteger) ||
                typeStr == @encode(long) || typeStr == @encode(long long)) {
            return [value integerValue]; // Handle integer numbers
        }
        else if(typeStr == @encode(unsigned int) || typeStr == @encode(NSUInteger) ||
                typeStr == @encode(unsigned long) || typeStr == @encode(unsigned long long)) {
            return [value unsignedIntegerValue]; // Handle unsigned integer numbers
        }
        // Handle floating-point values
        else if (typeStr == @encode(float) || typeStr == @encode(double)) {
            return [value doubleValue]; // Handle floating point numbers
        }
    }
    else if ([value isKindOfClass:[NSData class]]) {
        const uint8_t* bytes = (const uint8_t*)[value bytes];
        return ac::Dict::binary(ac::Blob(bytes, bytes + [value length]));
    } else if ([value isKindOfClass:[NSNull class]]) {
        return ac::Dict();
    }

    NSException *exception = [NSException exceptionWithName: @"DictConverterException"
                                                    reason: @"Invalid type for dictionary value"
                                                    userInfo: nil];
    @throw exception;

    return ac::Dict();
}

+ (ac::Dict)convertToACDict:(NSDictionary<NSString *, id> *)dictionary {
    ac::Dict jsonObj;

    for (NSString *key in dictionary) {
        id value = dictionary[key];
        jsonObj[key.UTF8String] = [self convertDictValue:value];
    }

    return jsonObj;
}

// Helper function to determine the type of the value
+ (id)convertJSONValue:(const ac::Dict&)json {
    // Handle different JSON types
    if (json.is_object()) {
        return [self convertToDictionary:json];
    }
    else if (json.is_array()) {
        NSMutableArray *array = [NSMutableArray array];
        for (const auto &element : json) {
            [array addObject:[self convertJSONValue:element]];
        }
        return array;
    }
    else if (json.is_binary()) {
        auto& buf = json.get_binary();
        return [NSData dataWithBytes:buf.data() length:buf.size()];
    }
    else if (json.is_string()) {
        return [NSString stringWithUTF8String:json.get<std::string>().c_str()];
    }
    else if (json.is_number_integer()) {
        return [NSNumber numberWithLongLong:json.get<int64_t>()];
    }
    else if (json.is_number_float()) {
        return [NSNumber numberWithDouble:json.get<double>()];
    }
    else if (json.is_boolean()) {
        return [NSNumber numberWithBool:json.get<bool>()];
    }
    else {
        assert(json.is_null());
        return [NSNull null];
    }
}

+ (NSDictionary *)convertToDictionary:(const ac::Dict&)json {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];

    for (auto it = json.begin(); it != json.end(); ++it) {
        NSString *key = [NSString stringWithUTF8String:it.key().c_str()];
        id value = [self convertJSONValue:it.value()];
        dict[key] = value;
    }

    return dict;
}

@end
