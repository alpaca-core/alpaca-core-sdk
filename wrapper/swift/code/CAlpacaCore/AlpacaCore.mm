// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#import "./AlpacaCore.h"
#import "./Dict.h"



static ac::local::ModelFactory modelFactory;
@implementation AlpacaCore

+ (ac::local::ModelFactory*)getModelFactory {
    return &modelFactory;
}

+ (void)addModelLoader:(NSString*)loaderName :(ac::local::ModelLoader*)loader {
    modelFactory.addLoader([loaderName UTF8String], *loader);
}

+ (ACModel*)createModel:(ac::local::ModelDesc)description :(NSDictionary*)params {
    NSLog(@"%s", "Model creation started!");

    ac::Dict dict = [DictionaryWrapper convertToACDict:params];
    ac::local::ModelPtr model = modelFactory.createModel(description, dict);
    return [[ACModel alloc] initWithModelPtr:model];
}

@end
