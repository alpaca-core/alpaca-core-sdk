# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-dict INTERFACE)
add_library(ac::dict ALIAS ac-dict)
target_sources(ac-dict
    PUBLIC FILE_SET HEADERS FILES
        ac/Dict.hpp
        ac/vendor/nlohmann/json.hpp
        ac/vendor/nlohmann/json_fwd.hpp
)

install(TARGETS ac-dict
    FILE_SET HEADERS
)
