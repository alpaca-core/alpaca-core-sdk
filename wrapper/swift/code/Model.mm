// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "Model.h"
#import "Dict.h"

@implementation ACModel {
    ac::Model* _model;
}

//-(id)init() {
//    _model = nullptr;
//    return self;
//}
//
//- (id)initWithModel:(ac::Model*)model {
//    self = [super init];
//    if (self) {
//        _model = model;
//    }
//    return self;
//}

- (void)createInstance:(NSString *)type :(ac::Dict)params :(ac::ResultCb<ac::InstancePtr>*)cb {
    //_model->createInstance(type.UTF8String, params, cb);
}

- (void)createInstance {
    DictionaryWrapper *dw = [DictionaryWrapper alloc];
    NSData *data = [NSData data]; // Replace with actual data if needed

    // Create the NSDictionary that mirrors the Swift dictionary structure
    NSDictionary *dictionary = @{
        @"name": @"Alice",
        @"age": @(28),
        @"height": @(1.7),
        @"isMember": @(YES),
        @"scores": @[@(88), @(92), @(79)],
        @"inner": @{
            @"str": @"hello",
            @"float": @(3.14),
            @"neg": @(-100),
            @"big": @(3000000000),
            @"more_inner": @{
                @"str": @"hello",
                @"float": @(3.14),
                @"neg": @(-100),
                @"big": @(3000000000)
            }
        },
        @"bytes": data,
        @"empty_list": @[],
        @"empty_dict": @{},
        @"empty_object": @{}
    };

    ac::Dict d = [dw convertToJSON:dictionary];
    // Convert nlohmann::json to string and then to NSString
    std::string jsonString = d.dump();
    NSLog(@"%s", jsonString.c_str());
    //_model->createInstance(type.UTF8String, params, cb);
}

@end
