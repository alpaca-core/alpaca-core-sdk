# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
set_target_properties(ac-local PROPERTIES
    EXPORT_NAME ac::local
)

ac_dep(furi)

target_link_libraries(ac-local
    PRIVATE
        ac::jalog
        furi::furi
    PUBLIC
        ac::frame
        ac::astl
)

configure_file(
    version.in.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ac/local/version.h
    @ONLY
)

target_sources(ac-local
    INTERFACE FILE_SET HEADERS FILES
        ac/local/export.h
        ac/local/version.h

        ac/local/fs/FileUri.hpp
        ac/local/fs/FsUtil.hpp

        ac/local/PluginInterface.hpp
        ac/local/PluginInfo.hpp
        ac/local/LoadPluginCb.hpp
        ac/local/PluginManager.hpp
        ac/local/Resource.hpp
        ac/local/ResourceManager.hpp
        ac/local/ResourceLock.hpp
        ac/local/ResourceCache.hpp

        ac/local/Backend.hpp
        ac/local/DefaultBackend.hpp
        ac/local/Lib.hpp

        ac/local/ServiceScorer.hpp
        ac/local/CommonServiceScorers.hpp

        ac/local/schema/AssetMgrInterface.hpp
    PRIVATE
        ac/local/Logging.hpp
        ac/local/Logging.cpp

        ac/local/fs/FileUri.cpp
        ac/local/fs/FsUtil.cpp

        ac/local/VtableExports.cpp

        ac/local/PluginManager.cpp

        ac/local/Backend.cpp
        ac/local/DefaultBackend.cpp
        ac/local/Lib.cpp

        ac/local/CommonServiceScorers.cpp

        ac/local/AssetMgrService.hpp
        ac/local/AssetMgrService.cpp
)

install(TARGETS ac-local
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
