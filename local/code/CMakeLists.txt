# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_local_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
set_target_properties(ac-local PROPERTIES
    EXPORT_NAME ac::local
)

target_link_libraries(ac-local
    PRIVATE
        ac::jalog
    PUBLIC
        ac::astl
        ac::dict
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

        ac/local/ModelLoader.hpp
        ac/local/ModelLoaderPtr.hpp
        ac/local/ModelLoaderRegistry.hpp

        ac/local/PluginInterface.hpp
        ac/local/PluginInfo.hpp
        ac/local/PluginManager.hpp

        ac/local/Lib.hpp

        ac/local/ModelLoaderScorer.hpp
        ac/local/CommonModelLoaderScorers.hpp
    PRIVATE
        ac/local/Logging.hpp
        ac/local/Logging.cpp

        ac/local/VtableExports.cpp

        ac/local/ModelLoaderRegistry.cpp
        ac/local/PluginManager.cpp

        ac/local/Lib.cpp

        ac/local/CommonModelLoaderScorers.cpp
)

install(TARGETS ac-local
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
