// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import <Foundation/Foundation.h>

#include "ac/Dict.hpp"

@interface ACModel : NSObject

- (void)createInstance:(NSString *)type :(ac::Dict)params;
- (void)createInstance;

@end
