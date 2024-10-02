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

- (NSDictionary*)runOp:(NSString*)op :(NSDictionary*) params :(progressCallback)progressCb {
    ac::Dict dict = [DictionaryWrapper convertToACDict:params];
    ac::Dict result = instance->runOp([op UTF8String], dict, ^(std::string_view tag, float progress){
        NSString* tagStr = [[NSString alloc] initWithCString:tag.data() encoding:NSUTF8StringEncoding];
        BOOL result = progressCb(tagStr, progress);
        [tagStr release];
        return result;
    });
    return [DictionaryWrapper convertToDictionary:result];
}

- (void)dealloc {
    [super dealloc];
}

@end
