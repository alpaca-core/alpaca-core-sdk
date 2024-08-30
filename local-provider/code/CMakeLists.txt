# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
target_include_directories(ac-local INTERFACE .)
target_link_libraries(ac-local
    PRIVATE
        xec::xec
    PUBLIC
        ac::api
)
target_sources(ac-local PRIVATE
    ac/export.h

    ac/LocalProvider.hpp
    ac/LocalProvider.cpp
    ac/LocalInference.hpp
    ac/LocalInference.cpp

    ac/AssetManager.hpp
    ac/AssetManager.cpp

    ac/AssetSourceLocalDir.hpp
    ac/AssetSourceLocalDir.cpp

    ac/local_provider.h
    ac/local_provider.cpp
    ac/asset_source_local_dir.h
    ac/asset_source_local_dir.cpp
)
