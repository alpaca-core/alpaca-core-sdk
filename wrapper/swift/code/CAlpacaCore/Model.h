// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import <Foundation/Foundation.h>
#import "./Instance.h"

#include "ac/local/ModelDesc.hpp"
#include "ac/local/ModelPtr.hpp"

@interface ACModel : NSObject

- (instancetype)initWithModelPtr:(ac::local::ModelPtr)model;
- (ACInstance*)createInstance:(NSString*)type :(NSDictionary*)params;

@end
