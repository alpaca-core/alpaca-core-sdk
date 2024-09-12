# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

if(AC_BUILD_TESTS)
    add_executable(test-ac-model-schema
        test-ac-model-schema.cpp
    )

    add_executable(test-ac-model-schema-no-generate
        test-ac-model-schema-no-generate.cpp
    )

    target_link_libraries(test-ac-model-schema
        PRIVATE
        ac::model-schema
        ac::api
        doctest::main
    )

    target_link_libraries(test-ac-model-schema-no-generate
        PRIVATE
        ac::model-schema
        ac::api
        doctest::main
    )

    add_test(NAME test-ac-model-schema COMMAND test-ac-model-schema)
    add_test(NAME test-ac-model-schema-no-generate COMMAND test-ac-model-schema-no-generate)
endif()
