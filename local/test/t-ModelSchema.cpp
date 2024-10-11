// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelSchema.hpp>
#include <doctest/doctest.h>

using namespace ac::local::schema;

struct TestModel : public ModelHelper<TestModel> {
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

extern const std::string_view expectedSchema;

TEST_CASE("schema") {
    OrderedDict d;
    TestModel::describe(d);
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
