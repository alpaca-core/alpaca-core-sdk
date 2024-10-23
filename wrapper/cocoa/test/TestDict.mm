#import <XCTest/XCTest.h>
#import <ac/DictConverter.hpp>

// ====== helper function

ac::Dict getDictFromJSON(const std::string& json) {
    if (json.empty()) {
        return {};
    }

    return ac::Dict::parse(json);
}

ac::Dict getDictWithBinary() {
    ac::Dict dict;
    dict["int"] = 3;
    dict["str"] = "hello";

    ac::Blob blob;
    blob.reserve(256);
    for (int i = 0; i < 256; ++i) {
        blob.push_back(uint8_t(i));
    }
    dict["bytes"] = ac::Dict::binary(std::move(blob));
    return dict;
}

// ====== end helper functions

@interface DictionaryTransformerTestCase : XCTestCase
@end

@implementation DictionaryTransformerTestCase

- (void)testEmptyDict {
    ac::Dict dict = getDictFromJSON("");
    XCTAssert(dict.size() == 0);
}

- (void)testBasicJSONDict {
    ac::Dict dict = getDictFromJSON(R"(
                                    {
                                        "bool": true,
                                        "int": 42,
                                        "str": "hello",
                                        "float": 3.14
                                    }
                                    )");
    XCTAssertEqual(int(dict.size()), 4);
    XCTAssertEqual(ac::Dict_optValueAt(dict, "bool", false), true);
    XCTAssertEqual(ac::Dict_optValueAt(dict, "int", 0), 42);
    XCTAssertEqual(std::string(ac::Dict_optValueAt(dict, "str", std::string())), std::string("hello"));
    XCTAssertEqualWithAccuracy(ac::Dict_optValueAt(dict, "float", 0.0f), 3.14f, 0.0001);

    NSDictionary* nsDict = [DictConverter convertToDictionary:dict];
    XCTAssertEqual(int(nsDict.count), 4);
    XCTAssertEqualObjects(nsDict[@"bool"], @YES);
    XCTAssertEqualObjects(nsDict[@"int"], @42);
    XCTAssertEqualObjects(nsDict[@"str"], @"hello");
    XCTAssertEqualObjects(nsDict[@"float"], @3.14);
}
- (void)testDictWithBinary {
    ac::Dict dict = getDictWithBinary();

    NSDictionary* nsDict = [DictConverter convertToDictionary:dict];
    XCTAssertEqual(int(nsDict.count), 3);
    XCTAssertEqualObjects(nsDict[@"int"], @3);
    XCTAssertEqualObjects(nsDict[@"str"], @"hello");

    NSData *bytes = nsDict[@"bytes"];
    XCTAssertNotNil(bytes);
    XCTAssertEqual(int(bytes.length), 256);
    const unsigned char* bytesData = (const unsigned char*)[bytes bytes];
    for (int i = 0; i < int(bytes.length); i++) {
        XCTAssertEqual(int(bytesData[i]), i);
    }
}

- (void) testComplexDict {
    ac::Dict dict = getDictFromJSON(R"(
                                    {
                                        "bool": true,
                                        "int": 42,
                                        "inner": {
                                            "str": "hello",
                                            "float": 3.14,
                                            "neg": -100,
                                            "big": 3000000000
                                        },
                                        "inner2": {
                                            "str": "world",
                                            "ilist": [1, "two", null, false, {"key": 1, "key2": "val"}],
                                            "null": null
                                        },
                                        "empty_list": [],
                                        "empty_dict": {}
                                    }
                                    )");
    NSDictionary* nsDict = [DictConverter convertToDictionary:dict];
    XCTAssertNotNil(nsDict);
    XCTAssertEqual(int(nsDict.count), 6);

    // Test "bool" value
    XCTAssertEqualObjects(nsDict[@"bool"], @YES, @"The 'bool' key should have value true.");

    // Test "int" value
    XCTAssertEqualObjects(nsDict[@"int"], @42, @"The 'int' key should have value 42.");

    // Test "inner" dictionary
    NSDictionary *innerDict = nsDict[@"inner"];
    XCTAssertNotNil(innerDict);
    XCTAssertEqual(int(innerDict.count), 4, @"The 'inner' dictionary should have 4 keys.");
    XCTAssertEqualObjects(innerDict[@"str"], @"hello", @"The 'str' key in 'inner' should be 'hello'.");
    XCTAssertEqualWithAccuracy([innerDict[@"float"] floatValue], 3.14, 0.0001, @"The 'float' key in 'inner' should be 3.14.");
    XCTAssertEqualObjects(innerDict[@"neg"], @-100, @"The 'neg' key in 'inner' should be -100.");
    XCTAssertEqualObjects(innerDict[@"big"], @3000000000, @"The 'big' key in 'inner' should be 3000000000.");

    // Test "inner2" dictionary
    NSDictionary *inner2Dict = nsDict[@"inner2"];
    XCTAssertNotNil(inner2Dict);
    XCTAssertEqual(int(inner2Dict.count), 3, @"The 'inner2' dictionary should have 3 keys.");
    XCTAssertEqualObjects(inner2Dict[@"str"], @"world", @"The 'str' key in 'inner2' should be 'world'.");

    // Test "ilist" array in "inner2"
    NSArray *ilist = inner2Dict[@"ilist"];
    XCTAssertNotNil(ilist);
    XCTAssertEqual(int(ilist.count), 5, @"The 'ilist' array in 'inner2' should have 5 elements.");
    XCTAssertEqualObjects(ilist[0], @1, @"The first element in 'ilist' should be 1.");
    XCTAssertEqualObjects(ilist[1], @"two", @"The second element in 'ilist' should be 'two'.");
    XCTAssertEqualObjects(ilist[2], [NSNull null], @"The third element in 'ilist' should be null, but for now it's handled as empty dict.");
    XCTAssertEqualObjects(ilist[3], @NO, @"The fourth element in 'ilist' should be false.");

    // Test dictionary inside "ilist"
    NSDictionary *ilistDict = ilist[4];
    XCTAssertNotNil(ilistDict);
    XCTAssertEqual(int(ilistDict.count), 2, @"The dictionary in 'ilist' should have 2 keys.");
    XCTAssertEqualObjects(ilistDict[@"key"], @1, @"The 'key' in dictionary of 'ilist' should be 1.");
    XCTAssertEqualObjects(ilistDict[@"key2"], @"val", @"The 'key2' in dictionary of 'ilist' should be 'val'.");

    // Test "null" key in "inner2"
    XCTAssertEqualObjects(inner2Dict[@"null"], [NSNull null], @"The third element in 'inner2' should be null, but for now it's handled as empty dict.");

    // Test "empty_list" key
    NSArray *emptyList = nsDict[@"empty_list"];
    XCTAssertNotNil(emptyList);
    XCTAssertEqual(int(emptyList.count), 0, @"The 'empty_list' should be an empty array.");

    // Test "empty_dict" key
    NSDictionary *emptyDict = nsDict[@"empty_dict"];
    XCTAssertNotNil(emptyDict);
    XCTAssertEqual(int(emptyDict.count), 0, @"The 'empty_dict' should be an empty dictionary.");
}

- (void)testConvertingToACDict {
    NSData *data = [@"Hello, World!" dataUsingEncoding:NSUTF8StringEncoding];

    // Create the NSDictionary that mirrors the Swift dictionary structure
    NSDictionary *dictionary = @{
        @"name": @"Alice",
        @"age": @(28),
        @"height": @(1.7),
        @"isMember": @YES,
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

    ac::Dict dict = [DictConverter convertToACDict:dictionary];
    // Begin assertions to verify the dict structure matches the NSDictionary
    XCTAssertEqual(int(dict.size()), 10, @"Dictionary size should be 11");

    // Verify top-level elements
    XCTAssertEqual(std::string(ac::Dict_optValueAt(dict, "name", std::string())), std::string("Alice"));
    XCTAssertEqual(ac::Dict_optValueAt(dict, "age", 0), 28);
    XCTAssertEqualWithAccuracy(ac::Dict_optValueAt(dict, "height", 0.0), 1.7, 0.001);
    XCTAssertEqual(ac::Dict_optValueAt(dict, "isMember", false), true);

    // Verify array values (scores)
    std::vector<int> scores = ac::Dict_optValueAt(dict, "scores", std::vector<int>{});
    XCTAssertEqual(int(scores.size()), 3);
    XCTAssertEqual(scores[0], 88);
    XCTAssertEqual(scores[1], 92);
    XCTAssertEqual(scores[2], 79);

    // Verify "inner" dictionary
    ac::Dict inner = ac::Dict_optValueAt(dict, "inner", ac::Dict());
    XCTAssertEqual(std::string(ac::Dict_optValueAt(inner, "str", std::string())), std::string("hello"));
    XCTAssertEqualWithAccuracy(ac::Dict_optValueAt(inner, "float", 0.0), 3.14, 0.001);
    XCTAssertEqual(ac::Dict_optValueAt(inner, "neg", 0), -100);
    // There was a bug getting the big number which resulted in -1294967296
    //XCTAssertEqual(ac::Dict_optValueAt(inner, "big", 2000000000), 3000000000);
    XCTAssertEqual(inner["big"], 3000000000);

    // Verify "more_inner" dictionary inside "inner"
    ac::Dict moreInner = ac::Dict_optValueAt(inner, "more_inner", ac::Dict());
    XCTAssertEqual(std::string(ac::Dict_optValueAt(moreInner, "str", std::string())), std::string("hello"));
    XCTAssertEqualWithAccuracy(ac::Dict_optValueAt(moreInner, "float", 0.0), 3.14, 0.001);
    XCTAssertEqual(ac::Dict_optValueAt(moreInner, "neg", 0), -100);
    // There was a bug getting the big number which resulted in -1294967296
    //XCTAssertEqual(ac::Dict_optValueAt(moreInner, "big", 2000000000), 3000000000);
    XCTAssertEqual(moreInner["big"], 3000000000);

    // Verify NSData conversion (bytes)
    std::string expectedBytes("Hello, World!");
    std::vector<unsigned char> binaryData = dict["bytes"].get_binary();
    XCTAssertEqual(std::string(binaryData.begin(), binaryData.end()), expectedBytes);

    // Verify empty structures
    std::vector<unsigned char>  emptyList = ac::Dict_optValueAt(dict, "empty_list", std::vector<unsigned char>{});
    XCTAssertEqual(int(emptyList.size()), 0);

    ac::Dict emptyDict = ac::Dict_optValueAt(dict, "empty_dict", ac::Dict());
    XCTAssertEqual(int(emptyDict.size()), 0);

    ac::Dict emptyObject = ac::Dict_optValueAt(dict, "empty_object", ac::Dict());
    XCTAssertEqual(int(emptyObject.size()), 0);
}

- (void)testConvertingDictToACDictToDict {
    NSData *data = [@"Hello, World!" dataUsingEncoding:NSUTF8StringEncoding];

    NSDictionary *dictionary = @{
        @"name": @"Alice",
        @"age": @(28),
        @"height": @(1.7),
        @"isMember": @YES,
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

    ac::Dict dict = [DictConverter convertToACDict:dictionary];
    NSDictionary* converted = [DictConverter convertToDictionary:dict];

    // Assertions to check if the converted dictionary matches the original dictionary
    XCTAssertNotNil(converted, @"Converted dictionary should not be nil.");

    // Test if the objects are the same:
    XCTAssertEqualObjects(dictionary, converted, @"The dictionaries should be the same");

    // Check each property

    // Check top-level values
    XCTAssertEqualObjects(converted[@"name"], dictionary[@"name"], @"Names should match.");
    XCTAssertEqualObjects(converted[@"age"], dictionary[@"age"], @"Ages should match.");
    XCTAssertEqualWithAccuracy([converted[@"height"] doubleValue], [dictionary[@"height"] doubleValue], 0.0001, @"Heights should match.");
    XCTAssertEqualObjects(converted[@"isMember"], dictionary[@"isMember"], @"Membership status should match.");

    // Check scores array
    NSArray *originalScores = dictionary[@"scores"];
    NSArray *convertedScores = converted[@"scores"];
    XCTAssertEqual(originalScores.count, convertedScores.count, @"Scores array count should match.");
    for (NSUInteger i = 0; i < originalScores.count; i++) {
        XCTAssertEqualObjects(convertedScores[i], originalScores[i], @"Score at index %lu should match.", (unsigned long)i);
    }

    // Check inner dictionary values
    NSDictionary *originalInner = dictionary[@"inner"];
    NSDictionary *convertedInner = converted[@"inner"];
    XCTAssertNotNil(convertedInner, @"Inner dictionary should not be nil.");

    XCTAssertEqualObjects(convertedInner[@"str"], originalInner[@"str"], @"Inner string should match.");
    XCTAssertEqualWithAccuracy([convertedInner[@"float"] doubleValue], [originalInner[@"float"] doubleValue], 0.0001, @"Inner float should match.");
    XCTAssertEqualObjects(convertedInner[@"neg"], originalInner[@"neg"], @"Inner neg value should match.");
    XCTAssertEqualObjects(convertedInner[@"big"], originalInner[@"big"], @"Inner big value should match.");

    // Check more_inner dictionary
    NSDictionary *originalMoreInner = originalInner[@"more_inner"];
    NSDictionary *convertedMoreInner = convertedInner[@"more_inner"];
    XCTAssertNotNil(convertedMoreInner, @"More inner dictionary should not be nil.");

    XCTAssertEqualObjects(convertedMoreInner[@"str"], originalMoreInner[@"str"], @"More inner string should match.");
    XCTAssertEqualWithAccuracy([convertedMoreInner[@"float"] doubleValue], [originalMoreInner[@"float"] doubleValue], 0.0001, @"More inner float should match.");
    XCTAssertEqualObjects(convertedMoreInner[@"neg"], originalMoreInner[@"neg"], @"More inner neg value should match.");
    XCTAssertEqualObjects(convertedMoreInner[@"big"], originalMoreInner[@"big"], @"More inner big value should match.");

    // Check binary data
    XCTAssertEqualObjects(converted[@"bytes"], data, @"Bytes data should match.");

    // Check empty collections
    XCTAssert([dictionary[@"empty_list"] isKindOfClass:[NSArray class]], "Empty list should be an NSArray");
    XCTAssert([dictionary[@"empty_dict"] isKindOfClass:[NSDictionary class]], "Empty doct should be an NSDictionary");
    XCTAssert([dictionary[@"empty_object"] isKindOfClass:[NSDictionary class]], "Empty list should be an NSDictionary");

    XCTAssertEqualObjects(dictionary[@"empty_list"], converted[@"empty_list"], @"Empty list should match.");
    XCTAssertEqualObjects(dictionary[@"empty_dict"], converted[@"empty_dict"], @"Empty dict should match.");
    XCTAssertEqualObjects(dictionary[@"empty_object"], converted[@"empty_object"], @"Empty object should match.");
}

@end
