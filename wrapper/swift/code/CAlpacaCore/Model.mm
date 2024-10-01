// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "Model.h"
#import "Dict.h"
#import "Instance.h"

#include "ac/local/Model.hpp"

@implementation ACModel {
    ac::local::ModelPtr model;
}

- (instancetype)initWithModelPtr:(ac::local::ModelPtr)modelPtr {
    model = modelPtr;
    return self;
}

- (ACInstance*)createInstance:(NSString *)type :(NSDictionary*)params {
    ac::Dict acParams = [DictionaryWrapper convertToACDict:params];
    return [[ACInstance alloc] initWithInstance:model->createInstance([type UTF8String], acParams)];
}

//- (void)createInstance {
//    NSData *data = [NSData data]; // Replace with actual data if needed
//
//    // Create the NSDictionary that mirrors the Swift dictionary structure
//    NSDictionary *dictionary = @{
//        @"name": @"Alice",
//        @"age": @(28),
//        @"height": @(1.7),
//        @"isMember": @(YES),
//        @"scores": @[@(88), @(92), @(79)],
//        @"inner": @{
//            @"str": @"hello",
//            @"float": @(3.14),
//            @"neg": @(-100),
//            @"big": @(3000000000),
//            @"more_inner": @{
//                @"str": @"hello",
//                @"float": @(3.14),
//                @"neg": @(-100),
//                @"big": @(3000000000)
//            }
//        },
//        @"bytes": data,
//        @"empty_list": @[],
//        @"empty_dict": @{},
//        @"empty_object": @{}
//    };
//
//    ac::Dict d = [DictionaryWrapper convertToACDict:dictionary];
//
//    NSDictionary *dww= [DictionaryWrapper convertToDictionary:d];
//    NSLog(@"Dictionary: %@", dww);
//    // Convert nlohmann::json to string and then to NSString
//    std::string jsonString = d.dump();
//    NSLog(@"%s", jsonString.c_str());
//    //_model->createInstance(type.UTF8String, params, cb);
//}

@end
