# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-schema INTERFACE)
add_library(ac::schema ALIAS ac-schema)
target_link_libraries(ac-schema INTERFACE
    ac::astl-public
    ac::dict
    # magic_enum::magic_enum
)
target_sources(ac-schema INTERFACE FILE_SET HEADERS FILES
    ac/schema/ModelSchema.hpp
    ac/schema/ModelSchemaGenHelper.hpp
    # ac/schema/SchemaEnum.hpp
    ac/schema/SchemaItem.hpp
)

install(TARGETS ac-schema
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
