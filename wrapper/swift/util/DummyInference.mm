// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "DummyInference.h"

#include "ac/local/LocalDummy.hpp"

@implementation DummyInference

+ (void)addDummyInference:(ac::local::ModelFactory*)factory {
    ac::local::addDummyInference(*factory);
}

@end
