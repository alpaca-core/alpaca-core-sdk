// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#import <Foundation/Foundation.h>

NSString* runPayload();


@interface MyObjCWrapper : NSObject

+ (int)getIntegerFromCPP; // Expose the C++ function as an Objective-C method

@end
