# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(furi)
ac_dep(boost)

icm_add_lib(file-download FILE_DOWNLOAD
    code/dl/export.h
    code/dl/FileDownload.hpp
    code/dl/FileDownload.cpp
)
target_include_directories(file-download INTERFACE code)
target_link_libraries(file-download
    PUBLIC
        splat::splat
        itlib::itlib
        ac::astl
    PRIVATE
        furi::furi
        Boost::beast
)
add_library(ac::file-download ALIAS file-download)

ac_add_test_subdir()
