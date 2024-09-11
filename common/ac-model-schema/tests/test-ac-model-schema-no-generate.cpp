// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#include <ac/SchemaDefinitions.hpp>

BEGIN_SCHEMA(TestSchemaStruct)

BEGIN_PARAMS()
DEFINE_PARAM(test_string, std::string, "A test string parameter", "default")
DEFINE_PARAM(test_int, int, "A test integer parameter", 42)
DEFINE_PARAM(test_float, float, "A test float parameter", 3.14f)
END_PARAMS()

BEGIN_RESULT()
DEFINE_RESULT(result_string, std::string, "A test string result")
DEFINE_RESULT(result_bool, bool, "A test boolean result")
END_RESULT()

END_SCHEMA()

TEST_CASE("TestSchemaStruct without GENERATE_SCHEMA") {
    TestSchemaStruct::RunParams params;
    TestSchemaStruct::RunResult result;

    SUBCASE("Default values") {
        Dict d;
        CHECK(params.test_string(d) == "default");
        CHECK(params.test_int(d) == 42);
        CHECK(params.test_float(d) == doctest::Approx(3.14f));
    }

    SUBCASE("Set and get values") {
        Dict d;
        params.set_test_string(d, "new value");
        params.set_test_int(d, 100);
        params.set_test_float(d, 2.718f);

        CHECK(params.test_string(d) == "new value");
        CHECK(params.test_int(d) == 100);
        CHECK(params.test_float(d) == doctest::Approx(2.718f));

        result.set_result_string(d, "test result");
        result.set_result_bool(d, true);

        CHECK(result.result_string(d) == "test result");
        CHECK(result.result_bool(d) == true);
    }
}
