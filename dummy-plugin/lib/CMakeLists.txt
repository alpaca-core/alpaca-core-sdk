# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-dummy-lib STATIC)
add_library(ac::dummy-lib ALIAS ac-dummy-lib)

target_include_directories(ac-dummy-lib INTERFACE .)
target_link_libraries(ac-dummy-lib
    PUBLIC
        ac::local
        ac::jalog
)
target_sources(ac-dummy-lib PRIVATE
    ac/dummy/Logging.hpp
    ac/dummy/Logging.cpp
    ac/dummy/Model.hpp
    ac/dummy/Model.cpp
    ac/dummy/Instance.hpp
    ac/dummy/Instance.cpp
    ac/dummy/LocalDummy.hpp
    ac/dummy/LocalDummy.cpp
)

set_target_properties(ac-dummy-lib PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME dummy-lib
)
