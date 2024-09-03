# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
dep(nl-json)

icm_add_lib(ac-api AC_API)
add_library(ac::api ALIAS ac-api)
target_include_directories(ac-api INTERFACE .)
target_link_libraries(ac-api
    PUBLIC
        splat::splat
        itlib::itlib
        ac::astl
        nlohmann_json::nlohmann_json
)
target_sources(ac-api PRIVATE
    ac/export.h

    ac/Callback.hpp
    ac/Dict.hpp
    ac/Provider.hpp
    ac/InstancePtr.hpp
    ac/Instance.hpp
    ac/Model.hpp
    ac/VtableExports.cpp

    ac/dict_ref.h
    ac/dict.h
    ac/dict-c.cpp
    ac/api.h
    ac/api-c.cpp
)
