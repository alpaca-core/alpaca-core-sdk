# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(xec)

icm_add_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
target_include_directories(ac-local INTERFACE .)
target_link_libraries(ac-local
    PRIVATE
        jalog::jalog
        xec::xec
    PUBLIC
        ac::api
)
target_sources(ac-local PRIVATE
    ac/export.h

    ac/Logging.hpp
    ac/Logging.cpp

    ac/LocalProvider.hpp
    ac/LocalProvider.cpp
    ac/LocalInference.hpp
    ac/LocalInference.cpp

    ac/local_provider.h
    ac/local_provider.cpp
)
