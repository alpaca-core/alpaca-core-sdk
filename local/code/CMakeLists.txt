# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_local_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
set_target_properties(ac-local PROPERTIES
    EXPORT_NAME ac::local
)

if(NOT TARGET Boost::boost)
    # only add boost if not added from the outside
    CPMAddPackage(gh:iboB/boost-trim@1.85.0)
endif()

if(NOT TARGET Boost::asio)
    # asio is header only, so this is fine
    add_library(Boost::asio ALIAS Boost::boost)
endif()

target_link_libraries(ac-local
    PRIVATE
        ac::jalog
    PUBLIC
        ac::frame
        ac::astl
        ac::dict
        Boost::asio
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

        ac/local/ModelDesc.hpp
        ac/local/ProgressCb.hpp
        ac/local/ModelPtr.hpp
        ac/local/Model.hpp
        ac/local/Instance.hpp

        ac/local/Provider.hpp
        ac/local/ProviderPtr.hpp
        ac/local/ProviderRegistry.hpp

        ac/local/PluginInterface.hpp
        ac/local/PluginInfo.hpp
        ac/local/LoadPluginCb.hpp
        ac/local/PluginManager.hpp

        ac/local/Session.hpp
        ac/local/SyncSession.hpp
        ac/local/LocalConnection.hpp
        ac/local/TestBlockingSession.hpp

        ac/local/Lib.hpp

        ac/local/ProviderScorer.hpp
        ac/local/CommonProviderScorers.hpp
    PRIVATE
        ac/local/Logging.hpp
        ac/local/Logging.cpp

        ac/local/VtableExports.cpp

        ac/local/ProviderRegistry.cpp
        ac/local/PluginManager.cpp

        ac/local/Session.cpp
        ac/local/SyncSession.cpp
        ac/local/LocalConnection.cpp
        ac/local/TestBlockingSession.cpp

        ac/local/Lib.cpp

        ac/local/CommonProviderScorers.cpp
)

install(TARGETS ac-local
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
