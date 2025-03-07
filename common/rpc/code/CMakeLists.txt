# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-rpc AC_RPC)
add_library(ac::rpc ALIAS ac-rpc)
set_target_properties(ac-rpc PROPERTIES
    EXPORT_NAME ac::rpc
)
target_sources(ac-rpc
    INTERFACE FILE_SET HEADERS FILES
        ac/rpc/api.h
        ac/rpc/Frame.hpp
        ac/rpc/Stream.hpp
    PRIVATE
        ac/rpc/Stream.cpp
)
target_link_libraries(ac-rpc PUBLIC
    ac::io
)

install(TARGETS ac-rpc
    EXPORT ac-common-targets
    FILE_SET HEADERS
)
