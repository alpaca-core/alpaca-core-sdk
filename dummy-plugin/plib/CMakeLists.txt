# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_local_lib(ac-plib-dummy AC_PLIB_DUMMY)
add_library(ac::plib-dummy ALIAS ac-plib-dummy)
set_target_properties(ac-plib-dummy PROPERTIES
    EXPORT_NAME plib-dummy
)

target_link_libraries(ac-plib-dummy
    PUBLIC
        ac::local
    PRIVATE
        ac::dummy-lib
)

target_sources(ac-plib-dummy
    INTERFACE FILE_SET HEADERS FILES
        ac/plib/Dummy.hpp
    PRIVATE
        ac/plib/Dummy.cpp
)

install(TARGETS ac-plib-dummy
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
