// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import <Foundation/Foundation.h>

#include "ac/Dict.hpp"
#include "ac/Model.hpp"

@interface ACModel : NSObject

- (void)createInstance:(NSString *)type :(ac::Dict)params :(ac::ResultCb<ac::InstancePtr>*)cb;
- (void)createInstance;

@end
