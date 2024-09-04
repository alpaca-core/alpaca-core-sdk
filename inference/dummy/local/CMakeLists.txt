# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local-dummy AC_LOCAL_DUMMY)
add_library(ac::local-dummy ALIAS ac-local-dummy)

target_include_directories(ac-local-dummy PUBLIC code)

target_sources(ac-local-dummy PRIVATE
    code/ac/export.h
    code/ac/LocalDummy.hpp
    code/ac/LocalDummy.cpp
    code/ac/local_dummy.h
    code/ac/local_dummy.cpp
)

target_link_libraries(ac-local-dummy
    PRIVATE
        ac::dummy
        ac::local
)

ac_add_test_subdir()
ac_add_example_subdir()
