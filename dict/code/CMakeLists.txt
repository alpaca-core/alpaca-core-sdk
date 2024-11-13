# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-dict INTERFACE)
add_library(ac::dict ALIAS ac-dict)
set_target_properties(ac-dict PROPERTIES
    EXPORT_NAME ac::dict
)
target_sources(ac-dict INTERFACE FILE_SET HEADERS FILES
    ac/Dict.hpp
    ac/vendor/nlohmann/json.hpp
    ac/vendor/nlohmann/json_fwd.hpp
)

install(TARGETS ac-dict
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
