// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/SerializeVisitors.hpp>
#include <ac/schema/SchemaVisitor.hpp>
#include <doctest/doctest.h>

using namespace ac::schema;

struct Person {
    Field<std::string> name;
    Field<int> age = Default(0);

    template <typename V>
    void visitFields(V& v) {
        v(name, "name", "Name of the person");
        v(age, "age", "Age of the person");
    }
};

struct Company {
    Field<std::string> name;
    Field<std::string> mission = std::nullopt;
    Field<float> revenue = Default(0.f);
    Field<Person> ceo;
    Field<std::vector<Person>> employees;
    Field<std::vector<std::string>> products = std::nullopt;

    template <typename V>
    void visitFields(V& v) {
        v(name, "name", "Company name");
        v(mission, "mission", "Mission statement");
        v(revenue, "revenue", "Yearly revenue");
        v(ceo, "ceo", "CEO");
        v(employees, "employees", "List of employees");
        v(products, "products", "List of products");
    }
};

Company makeAc() {
    return {
        .name = "Alpaca Core",
        .ceo = Person {.name = "John Doe", .age = 42 },
        .employees = std::vector {
            Person {.name = "Alice", .age = 25 },
            Person {.name = "Bob", .age = 30 },
            Person {.name = "Charlie" }
        },
        .products = std::vector<std::string> {"ac-local", "acord", "ilib-foo"},
    };
}

void checkAc(const Company& c) {
    CHECK(c.name == "Alpaca Core");
    CHECK_FALSE(c.mission.hasValue());
    CHECK(c.revenue == 0.f);
    CHECK(c.ceo->name == "John Doe");
    CHECK(c.ceo->age == 42);
    CHECK(c.employees->size() == 3);
    CHECK(c.employees->at(0).name == "Alice");
    CHECK(c.employees->at(0).age == 25);
    CHECK(c.employees->at(1).name == "Bob");
    CHECK(c.employees->at(1).age == 30);
    CHECK(c.employees->at(2).name == "Charlie");
    CHECK(c.employees->at(2).age == 0);
    CHECK(c.products->size() == 3);
    CHECK(c.products->at(0) == "ac-local");
    CHECK(c.products->at(1) == "acord");
    CHECK(c.products->at(2) == "ilib-foo");
}

TEST_CASE("io visitors") {
    ac::Dict dict;
    {
        auto c = makeAc();
        checkAc(c);
        dict = Struct_toDict(std::move(c));
    }

    auto cc = Struct_fromDict<Company>(std::move(dict));
    checkAc(cc);
}

TEST_CASE("schema") {
    acnl::ordered_json dict;
    Struct_toSchema<Company>(dict);

    auto schema = dict.dump(2);

    CHECK(schema == R"json({
  "type": "object",
  "properties": {
    "name": {
      "type": "string",
      "description": "Company name"
    },
    "mission": {
      "type": "string",
      "description": "Mission statement"
    },
    "revenue": {
      "type": "number",
      "description": "Yearly revenue",
      "default": 0.0
    },
    "ceo": {
      "type": "object",
      "properties": {
        "name": {
          "type": "string",
          "description": "Name of the person"
        },
        "age": {
          "type": "integer",
          "description": "Age of the person",
          "default": 0
        }
      },
      "required": [
        "name"
      ],
      "description": "CEO"
    },
    "employees": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "name": {
            "type": "string",
            "description": "Name of the person"
          },
          "age": {
            "type": "integer",
            "description": "Age of the person",
            "default": 0
          }
        },
        "required": [
          "name"
        ]
      },
      "description": "List of employees"
    },
    "products": {
      "type": "array",
      "items": {
        "type": "string"
      },
      "description": "List of products"
    }
  },
  "required": [
    "name",
    "ceo",
    "employees"
  ]
})json");
}