# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
add_library(ac-io INTERFACE)
add_library(ac::io ALIAS ac-io)
set_target_properties(ac-io PROPERTIES
    EXPORT_NAME ac::io
)
target_sources(ac-io INTERFACE FILE_SET HEADERS FILES
    ac/io/status.hpp
)
target_link_libraries(ac-io INTERFACE
    ac::astl
    ac::xec
)

install(TARGETS ac-io
    EXPORT ac-common-targets
    FILE_SET HEADERS
)
