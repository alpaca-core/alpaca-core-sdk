// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import <Foundation/Foundation.h>
#import "./Model.h"
#import "./Instance.h"

#include "ac/local/ModelFactory.hpp"
#include "ac/local/ModelLoader.hpp"

@interface AlpacaCore : NSObject

+ (ac::local::ModelFactory*)getModelFactory;
+ (void)addModelLoader:(NSString*)loaderName :(ac::local::ModelLoader*)loader;

typedef BOOL (^progressCallback)(NSString* tag, float progress);
+ (ACModel*)createModel:(ac::local::ModelDesc)description :(NSDictionary*)params :(progressCallback)progressCb;

+ (void)releaseModel:(ACModel*)model;
+ (void)releaseInstance:(ACInstance*)instance;

@end
