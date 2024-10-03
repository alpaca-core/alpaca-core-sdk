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

+ (ACModel*)createModel:(ac::local::ModelDesc)description :(NSDictionary*)params :(progressCallback)progressCb {
    NSLog(@"%s", "Model creation started!");

    ac::Dict dict = [DictionaryWrapper convertToACDict:params];
    ac::local::ModelPtr model = modelFactory.createModel(description, dict, ^(std::string_view tag, float progress){
        NSString* tagStr = [[NSString alloc] initWithCString:tag.data() encoding:NSUTF8StringEncoding];
        BOOL result = progressCb(tagStr, progress);
        [tagStr release];
        return result;
    });
    return [[ACModel alloc] initWithModelPtr:model];
}

+ (void)releaseModel:(ACModel*)model {
    [model release];
}

+ (void)releaseInstance:(ACInstance*)instance {
    [instance release];
}

+ (void)checkRetainValue:(id)object {
    if (object == nullptr) {
        return;
    }

    NSLog(@"Retain count: %ld", CFGetRetainCount((__bridge CFTypeRef)(object)));
}

@end
