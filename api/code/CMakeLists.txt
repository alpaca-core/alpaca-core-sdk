# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-api AC_API)
add_library(ac::api ALIAS ac-api)
target_include_directories(ac-api INTERFACE .)
target_link_libraries(ac-api
    PUBLIC
        splat::splat
        itlib::itlib
        nlohmann_json::nlohmann_json
)
target_sources(ac-api PRIVATE
    ac/api/export.h
    ac/api/Callback.hpp
    ac/api/Dict.hpp
    ac/api/Provider.hpp
    ac/api/InstancePtr.hpp
    ac/api/Instance.hpp
    ac/api/Model.hpp
    ac/api/VtableExports.cpp
)
