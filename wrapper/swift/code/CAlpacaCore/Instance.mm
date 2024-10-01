// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "./Instance.h"
#import "./Dict.h"

@implementation ACInstance {
    std::unique_ptr<ac::local::Instance> instance;
}

- (instancetype)initWithInstance:(std::unique_ptr<ac::local::Instance>)instancePtr {
    instance.reset(instancePtr.release());
    return self;
}

- (NSDictionary*)runOp:(NSString*)op :(NSDictionary*) params {
    ac::Dict dict = [DictionaryWrapper convertToACDict:params];
    ac::Dict result = instance->runOp([op UTF8String], dict);
    return [DictionaryWrapper convertToDictionary:result];
}

@end
