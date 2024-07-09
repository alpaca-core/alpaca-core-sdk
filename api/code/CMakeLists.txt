# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-api AC_API)
add_library(ac::api ALIAS ac-api)
target_include_directories(ac-api INTERFACE .)
target_link_libraries(ac-api
    PUBLIC
        ac::core
        splat::splat
        itlib::itlib
)
target_sources(ac-api PRIVATE
    ac/api/export.h
    ac/api/Callback.hpp
    ac/api/Provider.hpp
    ac/api/Provider.cpp
)
