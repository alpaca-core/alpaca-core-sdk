# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(splat)

CPMAddPackage(
    NAME dr_libs
    VERSION 1.0
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG da35f9d
)
add_library(dr_libs INTERFACE)
target_include_directories(dr_libs INTERFACE ${dr_libs_SOURCE_DIR})

icm_add_lib(ac-audio AC_AUDIO)

add_library(ac-dev::audio ALIAS ac-audio)
target_include_directories(ac-audio
    INTERFACE
        code
)
target_link_libraries(ac-audio
    PUBLIC
        splat::splat
        itlib::itlib
    PRIVATE
        ac::astl
        dr_libs
)
target_sources(ac-audio
    PRIVATE
        code/ac-audio.cpp
)

ac_add_test_subdir()
