# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-dummy AC_DUMMY)
add_library(ac::dummy ALIAS ac-dummy)
target_include_directories(ac-dummy INTERFACE .)
target_link_libraries(ac-dummy
    PRIVATE
        jalog::jalog
    PUBLIC
        splat::splat
        ac::astl-private
)
target_sources(ac-dummy PRIVATE
    ac/dummy/export.h
    ac/dummy/Logging.hpp
    ac/dummy/Logging.cpp
    ac/dummy/Model.hpp
    ac/dummy/Model.cpp
    ac/dummy/Instance.hpp
    ac/dummy/Instance.cpp
)
