# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(nl-json)
ac_dep(xec)

icm_add_lib(ac-asset-mgr AC_ASSET)
add_library(ac::asset-mgr ALIAS ac-asset-mgr)
target_include_directories(ac-asset-mgr INTERFACE .)
target_link_libraries(ac-asset-mgr
    PRIVATE
        nlohmann_json::nlohmann_json
        jalog::jalog
        ac::xxhash-cpp
        ac::http
        xec::xec
    PUBLIC
        splat::splat
        ac::astl
)
target_sources(ac-asset-mgr PRIVATE
    ac/asset/export.h

    ac/asset/Logging.hpp
    ac/asset/Logging.cpp

    ac/asset/FsUtil.hpp
    ac/asset/FsUtil.cpp

    ac/asset/AssetManager.hpp
    ac/asset/AssetManager.cpp
    ac/asset/AssetSourceLocalDir.hpp
    ac/asset/AssetSourceLocalDir.cpp
    ac/asset/AssetSourceLocalFiles.hpp
    ac/asset/AssetSourceLocalFiles.cpp
    ac/asset/AssetSourceHttp.hpp
    ac/asset/AssetSourceHttp.cpp

    ac/asset/asset_source.h

    ac/asset/asset_source_local_dir.h
    ac/asset/asset_source_local_dir.cpp
)
