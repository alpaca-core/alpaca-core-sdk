// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelSchema.hpp>
#include <doctest/doctest.h>

using namespace ac::local::schema;

TEST_CASE("test binary null") {
    Binary b;
    CHECK_FALSE(b.getValue());
}

TEST_CASE("test binary val") {
    std::vector<uint8_t> v = {1, 2, 3};
    auto data = v.data();
    Dict d;
    Binary b(d);
    CHECK_FALSE(b.getValue());

    b.setValue(std::move(v));

    auto ptr = b.getValue();
    REQUIRE(ptr);
    CHECK(ptr->data() == data); // same vec
}

struct TestSchema : public ModelHelper<TestSchema> {
    static constexpr std::string_view id = "test-model";

    struct Params : public Object {
        using Object::Object;
        String name{*this, "name", "name of the test", "Test"};
        Bool gpu{*this, "gpu", "use GPU", false};
    };

    struct InstanceAthlete : public InstanceHelper<InstanceAthlete> {
        static constexpr std::string_view id = "athlete";

        struct Params : public Object {
            using Object::Object;
            String name{*this, "name", "name of the athlete", {}, true};
            Int age{*this, "age", "age of the athlete"};
        };

        struct OpJump {
            static constexpr std::string_view id = "jump";

            struct Params : public Object {
                using Object::Object;
                Int howHigh{*this, "how_high", "in meters", 5};
                String whereTo{*this, "where_to", "destination", {}, true};
            };

            struct Return : public Object {
                using Object::Object;
                Bool success{*this, "success", "did it work", {}, true};
            };
        };

        struct OpRun {
            static constexpr std::string_view id = "run";

            struct Params : public Object {
                using Object::Object;
                Int howFast{*this, "how_fast", "in km/h", 10};
            };

            struct Return : public Object {
                using Object::Object;
                Int distance{*this, "distance", "in meters", {}, true};
                Bool success{*this, "success", "did it work", {}, true};
                Array<String> places{*this, "places", "where we went"};
            };
        };

        using Ops = std::tuple<OpJump, OpRun>;
    };

    struct InstanceMusician : public InstanceHelper<InstanceMusician> {
        static constexpr std::string_view id = "musician";

        struct Params : public Object {
            using Object::Object;
            String almaMater{*this, "alma_mater", "university"};
            Int experience{*this, "experience", "experience of the musician"};
        };

        struct OpPlay {
            static constexpr std::string_view id = "play";

            struct Params : public Object {
                using Object::Object;
                String instrument{*this, "instrument", "what to play"};
            };

            struct Return : public Object {
                using Object::Object;
                Bool success{*this, "success", "did it work", {}, true};
                String notes{*this, "notes", "notes played"};
            };
        };

        struct OpSing {
            static constexpr std::string_view id = "sing";

            struct Params : public Object {
                using Object::Object;
                String song{*this, "song", "what to sing"};
                String style{ *this, "style", "how to sing", "pop"};
            };

            struct Return : public Object {
                using Object::Object;
                Bool success{*this, "success", "did it work", {}, true};
                String lytics{*this, "lyrics", "lyrics sung"};
            };
        };

        using Ops = std::tuple<OpPlay, OpSing>;
    };

    using Instances = std::tuple<InstanceAthlete, InstanceMusician>;
};

class Instance {
public:
    virtual ~Instance() = default;
    virtual Dict runOp(std::string_view op, Dict params) = 0;
};

class Athlete final : public Instance {
public:
    using Schema = TestSchema::InstanceAthlete;

    struct Params {
        std::string name;
        std::optional<int> age;
    } initParams;

    explicit Athlete(Dict& params) {
        Schema::Params p(params);
        initParams.name = *p.name.getValue();
        initParams.age = p.age.getValue();
    }

    Dict run(Dict& params) {
        Schema::OpRun::Params p(params);

        auto speed = *p.howFast.getValue();

        Dict retDict;
        Schema::OpRun::Return ret(retDict);

        if (speed == 10) {
            ret.distance.setValue(100);
            ret.success.setValue(true);
        }
        else if (speed > 20) {
            ret.distance.setValue(speed * initParams.age.value_or(10));
            ret.success.setValue(true);
            ret.places.emplace_back().setValue(initParams.name);
            ret.places.emplace_back().setValue("a");
            ret.places.emplace_back().setValue("b");
        }
        else {
            ret.distance.setValue(0);
            ret.success.setValue(false);
        }
        return retDict;
    }

    Dict jump(Dict& params) {
        Schema::OpJump::Params p(params);

        auto height = *p.howHigh.getValue();
        auto dest = *p.whereTo.getValue();

        Dict retDict;
        Schema::OpJump::Return ret(retDict);
        if (height < 5 && dest == "home") {
            ret.success.setValue(true);
        }
        else {
            ret.success.setValue(false);
        }
        return retDict;
    }

    Dict runOp(std::string_view opId, Dict params) override {
        switch (TestSchema::InstanceAthlete::getOpIndexById(opId)) {
        case Schema::opIndex<Schema::OpJump>:
            return jump(params);
        case Schema::opIndex<Schema::OpRun>:
            return run(params);
        default:
            throw std::runtime_error("unknown op");
        }
    }
};

class Model {
public:
    struct ModelParams {
        std::string name;
        bool gpu = false;
    } modelParams;


    explicit Model(Dict dparams) {
        TestSchema::Params p(dparams);
        modelParams.name = *p.name.getValue();
        modelParams.gpu = *p.gpu.getValue();
    }

    std::unique_ptr<Instance> createInstance(std::string_view instanceId, Dict params) {
        switch (TestSchema::getInstanceById(instanceId)) {
        case TestSchema::instanceIndex<TestSchema::InstanceAthlete>:
            return std::make_unique<Athlete>(params);
        case TestSchema::instanceIndex<TestSchema::InstanceMusician>:
            throw std::runtime_error("not available");
        default:
            throw std::runtime_error("unknown instance");
        }
    }
};

TEST_CASE("workflow") {
    Model m({{"gpu", true}});
    CHECK(m.modelParams.name == "Test");
    CHECK(m.modelParams.gpu == true);

    CHECK_THROWS_WITH(m.createInstance("musician", {}), "not available");
    CHECK_THROWS_WITH(m.createInstance("nope", {}), "unknown instance");

    auto alice = m.createInstance("athlete", {{"name", "Alice"}});

    auto res = alice->runOp("run", {{"how_fast", 10}});
    CHECK(res.dump() == R"({"distance":100,"success":true})");

    res = alice->runOp("run", {{"how_fast", 30}});
    CHECK(res.dump() == R"({"distance":300,"places":["Alice","a","b"],"success":true})");

    res = alice->runOp("run", {{"how_fast", 5}});
    CHECK(res.dump() == R"({"distance":0,"success":false})");

    res = alice->runOp("jump", {{"how_high", 4}, {"where_to", "home"}});
    CHECK(res.dump() == R"({"success":true})");

    res = alice->runOp("jump", {{"where_to", "home"}}); // how_high default is 5
    CHECK(res.dump() == R"({"success":false})");

    res = alice->runOp("jump", {{"how_high", 3}, {"where_to", "work"}});
    CHECK(res.dump() == R"({"success":false})");
}

extern const std::string_view expectedSchema;

TEST_CASE("schema") {
    OrderedDict d;
    TestSchema::describe(d);
    CHECK(d.dump(4) == expectedSchema);
}

const std::string_view expectedSchema = R"({
    "params": {
        "type": "object",
        "properties": {
            "name": {
                "description": "name of the test",
                "type": "string",
                "default": "Test"
            },
            "gpu": {
                "description": "use GPU",
                "type": "boolean",
                "default": false
            }
        }
    },
    "instances": {
        "athlete": {
            "params": {
                "type": "object",
                "properties": {
                    "name": {
                        "description": "name of the athlete",
                        "type": "string"
                    },
                    "age": {
                        "description": "age of the athlete",
                        "type": "integer"
                    }
                },
                "required": [
                    "name"
                ]
            },
            "ops": {
                "jump": {
                    "params": {
                        "type": "object",
                        "properties": {
                            "how_high": {
                                "description": "in meters",
                                "type": "integer",
                                "default": 5
                            },
                            "where_to": {
                                "description": "destination",
                                "type": "string"
                            }
                        },
                        "required": [
                            "where_to"
                        ]
                    },
                    "return": {
                        "type": "object",
                        "properties": {
                            "success": {
                                "description": "did it work",
                                "type": "boolean"
                            }
                        },
                        "required": [
                            "success"
                        ]
                    }
                },
                "run": {
                    "params": {
                        "type": "object",
                        "properties": {
                            "how_fast": {
                                "description": "in km/h",
                                "type": "integer",
                                "default": 10
                            }
                        }
                    },
                    "return": {
                        "type": "object",
                        "properties": {
                            "distance": {
                                "description": "in meters",
                                "type": "integer"
                            },
                            "success": {
                                "description": "did it work",
                                "type": "boolean"
                            },
                            "places": {
                                "description": "where we went",
                                "type": "array",
                                "items": {
                                    "type": "string"
                                }
                            }
                        },
                        "required": [
                            "distance",
                            "success"
                        ]
                    }
                }
            }
        },
        "musician": {
            "params": {
                "type": "object",
                "properties": {
                    "alma_mater": {
                        "description": "university",
                        "type": "string"
                    },
                    "experience": {
                        "description": "experience of the musician",
                        "type": "integer"
                    }
                }
            },
            "ops": {
                "play": {
                    "params": {
                        "type": "object",
                        "properties": {
                            "instrument": {
                                "description": "what to play",
                                "type": "string"
                            }
                        }
                    },
                    "return": {
                        "type": "object",
                        "properties": {
                            "success": {
                                "description": "did it work",
                                "type": "boolean"
                            },
                            "notes": {
                                "description": "notes played",
                                "type": "string"
                            }
                        },
                        "required": [
                            "success"
                        ]
                    }
                },
                "sing": {
                    "params": {
                        "type": "object",
                        "properties": {
                            "song": {
                                "description": "what to sing",
                                "type": "string"
                            },
                            "style": {
                                "description": "how to sing",
                                "type": "string",
                                "default": "pop"
                            }
                        }
                    },
                    "return": {
                        "type": "object",
                        "properties": {
                            "success": {
                                "description": "did it work",
                                "type": "boolean"
                            },
                            "lyrics": {
                                "description": "lyrics sung",
                                "type": "string"
                            }
                        },
                        "required": [
                            "success"
                        ]
                    }
                }
            }
        }
    }
})";
