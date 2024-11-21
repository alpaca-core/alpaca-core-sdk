# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-schema INTERFACE)
add_library(ac::schema ALIAS ac-schema)
target_link_libraries(ac-schema INTERFACE
    ac::astl
    ac::dict
)
target_sources(ac-schema INTERFACE FILE_SET HEADERS FILES
    ac/schema/TupleIndexByItemId.hpp
)

install(TARGETS ac-schema
    EXPORT ac-local-targets
    FILE_SET HEADERS
)

set(GENERATE_CXX_SCHEMA_RB ${CMAKE_CURRENT_SOURCE_DIR}/generate-cpp-for-ac-model-schema.rb
    CACHE INTERNAL "Path to generate-cpp-for-ac-model-schema.rb"
)
install(PROGRAMS generate-cpp-for-ac-model-schema.rb
    DESTINATION bin
)
