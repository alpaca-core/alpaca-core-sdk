# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(furi)

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
    PRIVATE
        furi::furi
)
add_library(ac::file-download ALIAS file-download)

ac_add_test_subdir()
