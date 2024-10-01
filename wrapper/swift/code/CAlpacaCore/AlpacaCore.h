// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import <Foundation/Foundation.h>
#import "./Model.h"

#include "ac/local/ModelFactory.hpp"
#include "ac/local/ModelLoader.hpp"

@interface AlpacaCore : NSObject

+ (ac::local::ModelFactory*)getModelFactory;
+ (void)addModelLoader:(NSString*)loaderName :(ac::local::ModelLoader*)loader;
+ (ACModel*)createModel:(ac::local::ModelDesc)description :(NSDictionary*)params;

@end
