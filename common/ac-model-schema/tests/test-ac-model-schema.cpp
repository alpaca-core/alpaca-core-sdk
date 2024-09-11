// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#define GENERATE_SCHEMA
#include <ac/SchemaDefinitions.hpp>

BEGIN_SCHEMA(TestSchema)

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

TEST_CASE("TestSchema generation") {
    nlohmann::json schema = TestSchema();

    SUBCASE("Params") {
        CHECK(schema["params"].is_object());
        CHECK(schema["params"]["test_string"]["type"] == "string");
        CHECK(schema["params"]["test_string"]["description"] == "A test string parameter");
        CHECK(schema["params"]["test_string"]["default"] == "default");

        CHECK(schema["params"]["test_int"]["type"] == "integer");
        CHECK(schema["params"]["test_int"]["description"] == "A test integer parameter");
        CHECK(schema["params"]["test_int"]["default"] == 42);

        CHECK(schema["params"]["test_float"]["type"] == "number");
        CHECK(schema["params"]["test_float"]["description"] == "A test float parameter");
        CHECK(schema["params"]["test_float"]["default"] == doctest::Approx(3.14f));
    }

    SUBCASE("Results") {
        CHECK(schema["result"].is_object());
        CHECK(schema["result"]["result_string"]["type"] == "string");
        CHECK(schema["result"]["result_string"]["description"] == "A test string result");

        CHECK(schema["result"]["result_bool"]["type"] == "boolean");
        CHECK(schema["result"]["result_bool"]["description"] == "A test boolean result");
    }
}
