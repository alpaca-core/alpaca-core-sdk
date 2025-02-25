# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
set_target_properties(ac-local PROPERTIES
    EXPORT_NAME ac::local
)

target_link_libraries(ac-local
    PRIVATE
        ac::jalog
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

        ac/local/Provider.hpp
        ac/local/ProviderPtr.hpp
        ac/local/ProviderRegistry.hpp

        ac/local/PluginInterface.hpp
        ac/local/PluginInfo.hpp
        ac/local/LoadPluginCb.hpp
        ac/local/PluginManager.hpp
        ac/local/Resource.hpp
        ac/local/ResourceManager.hpp

        ac/local/Lib.hpp

        ac/local/ProviderScorer.hpp
        ac/local/CommonProviderScorers.hpp

        ac/local/IoCtx.hpp
        ac/local/BlockingSyncIoHelper.hpp
    PRIVATE
        ac/local/Logging.hpp
        ac/local/Logging.cpp

        ac/local/VtableExports.cpp

        ac/local/ProviderRegistry.cpp
        ac/local/PluginManager.cpp
        ac/local/ResourceManager.cpp

        ac/local/Lib.cpp

        ac/local/CommonProviderScorers.cpp

        ac/local/IoCtx.cpp
        ac/local/BlockingSyncIoHelper.cpp
)

install(TARGETS ac-local
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
