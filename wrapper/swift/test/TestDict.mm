#import <XCTest/XCTest.h>
#import <Dict.h>

@interface DictTestCase : XCTestCase
@end

@implementation DictTestCase

- (void)test_dict {
    NSData *data = [NSData data]; // Replace with actual data if needed

    // Create the NSDictionary that mirrors the Swift dictionary structure
    NSDictionary *dictionary = @{
        @"name": @"Alice",
        @"age": @(28),
        @"height": @(1.7),
        @"isMember": @(YES),
        @"scores": @[@(88), @(92), @(79)],
        @"inner": @{
            @"str": @"hello",
            @"float": @(3.14),
            @"neg": @(-100),
            @"big": @(3000000000),
            @"more_inner": @{
                @"str": @"hello",
                @"float": @(3.14),
                @"neg": @(-100),
                @"big": @(3000000000)
            }
        },
        @"bytes": data,
        @"empty_list": @[],
        @"empty_dict": @{},
        @"empty_object": @{}
    };

    ac::Dict d = [DictionaryWrapper convertToACDict:dictionary];
    NSDictionary* unwrapped = [DictionaryWrapper convertToDictionary:d];
}

@end
