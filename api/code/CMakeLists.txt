# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_local_lib(ac-api AC_API)
add_library(ac::api ALIAS ac-api)
set_target_properties(ac-api PROPERTIES
    EXPORT_NAME ac::api
)

target_link_libraries(ac-api
    PUBLIC
        ac::astl
        ac::dict
)

configure_file(
    version.in.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ac/version.h
    @ONLY
)

target_sources(ac-api
    INTERFACE FILE_SET HEADERS FILES
        ac/export.h
        ac/version.h

        ac/Session.hpp
        ac/SessionHandler.hpp

        ac/Provider.hpp
    PRIVATE
        ac/SessionHandler.cpp

        ac/Session.cpp
)

install(TARGETS ac-api
    EXPORT ac-api-targets
    FILE_SET HEADERS
)
