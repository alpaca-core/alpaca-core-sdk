// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import <Foundation/Foundation.h>

#include "ac/local/Instance.hpp"

#include <memory>

@interface ACInstance : NSObject

- (instancetype)initWithInstance:(std::unique_ptr<ac::local::Instance>)instancePtr;

typedef BOOL (^progressCallback)(NSString* tag, float progress);
- (NSDictionary*)runOp:(NSString*)op :(NSDictionary*)params :(progressCallback)progressCb;

@end
