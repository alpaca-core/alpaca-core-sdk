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
    ACInstance* instance = [[ACInstance alloc] initWithInstance:model->createInstance([type UTF8String], acParams)];
    return instance;
}

- (void)dealloc {
    [super dealloc];
    model.reset();
}
@end
