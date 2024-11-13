# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

function(add_ac_local_plugin)
    cmake_parse_arguments(ARG "" "NAME" "SOURCES;PLUGIN_SOURCES;PLIB_SOURCES;LIBRARIES" ${ARGN})

    # private names for targets which are not installed and never leave the project
    set(privateName aclp-${ARG_NAME})
    set(baselibTargetName ${privateName}-baselib)
    set(plibTargetName ${privateName}-plib)
    string(MAKE_C_IDENTIFIER ${ARG_NAME} nameSym)

    # public names for targets which may be installed
    set(publicName aclp-${ARG_NAME})
    set(pluginTargetName ${publicName}-plugin)
    set(infoTargetName ${publicName}-info)
    string(MAKE_C_IDENTIFIER ${ARG_NAME} publicNameSym)

    # configure version
    file(CONFIGURE
        OUTPUT ${privateName}-version.h
        CONTENT [=[
// Generated file. Do not edit!
#pragma once

#define ACLP_@nameSym@_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define ACLP_@nameSym@_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define ACLP_@nameSym@_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define ACLP_@nameSym@_VERSION_TAG   "@PROJECT_VERSION_TAG@"

#define ACLP_@nameSym@_VERSION_STRING "@PROJECT_VERSION@@PROJECT_VERSION_TAG@"
]=]
        @ONLY
    )

    # add base lib
    add_library(${baselibTargetName} STATIC
        ${ARG_SOURCES}
        ${privateName}-version.h
        ${ARG_NAME}-ac-local-interface.hpp
        ${ARG_NAME}-ac-local-interface.cpp
    )
    target_link_libraries(${baselibTargetName} PUBLIC
        ac::local
        ac::jalog
        ${ARG_LIBRARIES}
    )
    set_target_properties(${baselibTargetName} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    target_include_directories(${baselibTargetName}
        PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
        INTERFACE .
    )

    # add plib
    file(CONFIGURE
        OUTPUT ${privateName}-plib.hpp
        CONTENT [=[
// Generated file. Do not edit!
#pragma once

#include <astl/symbol_export.h>

#if ACLPLIB_@nameSym@_SHARED
#   if BUILDING_ACLPLIB_@nameSym@
#       define ACLPLIB_@nameSym@_API SYMBOL_EXPORT
#   else
#       define ACLPLIB_@nameSym@_API SYMBOL_IMPORT
#   endif
#else
#   define ACLPLIB_@nameSym@_API
#endif

namespace ac::local { class ModelFactory; }

extern "C" ACLPLIB_@nameSym@_API
void add_@nameSym@_to_ac_local(ac::local::ModelFactory& factory);
]=]
        @ONLY
    )
    file(CONFIGURE
        OUTPUT ${privateName}-plib.cpp
        CONTENT [=[
// Generated file. Do not edit!
#include "@privateName@-plib.hpp"
#include "${ARG_NAME}-ac-local-interface.hpp"

extern "C"
void add_@nameSym@_to_ac_local(ac::local::ModelFactory& factory) {
    ac::@nameSym@::addToAcLocal(factory);
}
]=]
    )
    add_ac_local_lib(${plibTargetName} ACLPLIB_${nameSym}
        ${privateName}-plib.hpp
        ${privateName}-plib.cpp
        ${ARG_PLIB_SOURCES}
    )
    add_library(aclp::${ARG_NAME}-plib ALIAS ${plibTargetName})
    target_link_libraries(${plibTargetName}
        PUBLIC
            ac::local
        PRIVATE
            ${baselibTargetName}
    )
    target_include_directories(${plibTargetName}
        PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
        INTERFACE .
    )

    # add plugin
    file(CONFIGURE
        OUTPUT ${privateName}-entrypoint.cpp
        CONTENT [=[
// Generated file. Do not edit!
#include <astl/symbol_export.h>
#include "@privateName@-version.h"
#include "@ARG_NAME@-ac-local-interface.hpp"
#include <ac/local/PluginInterface.hpp>

namespace ac::local {

extern "C" SYMBOL_EXPORT
PluginInterface acLocalPluginLoad() {
    constexpr Version ownVersion(
        ACLP_@nameSym@_VERSION_MAJOR, ACLP_@nameSym@_VERSION_MINOR, ACLP_@nameSym@_VERSION_PATCH, ACLP_@nameSym@_VERSION_TAG
    );

    return {
        .acLocalVersion = ac::local::Project_Version,
        .pluginVersion = ownVersion,
        .addLoadersToFactory = ac::@nameSym@::addToAcLocal,
    };
}
static_assert(std::is_same_v<decltype(&acLocalPluginLoad), PluginInterface::PluginLoadFunc>);

} // namespace ac::local
]=]
        @ONLY
    )
    add_library(${pluginTargetName} MODULE
        ${privateName}-entrypoint.cpp
        ${ARG_PLUGIN_SOURCES}
    )
    add_library(aclp::${ARG_NAME} ALIAS ${pluginTargetName})
    set_target_properties(${pluginTargetName} PROPERTIES
        PREFIX ""
    )
    if(NOT WIN32)
        target_compile_options(${pluginTargetName} PRIVATE
            -fvisibility=hidden
            -fvisibility-inlines-hidden
        )
    endif()
    target_link_libraries(${pluginTargetName} PRIVATE
        ${baselibTargetName}
    )

    install(TARGETS ${pluginTargetName}
        COMPONENT plugins
        LIBRARY DESTINATION lib/ac-local
    )

    # add helper target for tests and examples which want to use the plugin directly
    file(GENERATE
        OUTPUT ${infoTargetName}.h
        CONTENT [=[
// Generated file. Do not edit!
#pragma once
#define ACLP_@publicNameSym@_PLUGIN_FILE "$<TARGET_PROPERTY:FILE>"
]=]
        TARGET ${pluginTargetName}
    )
    add_library(${infoTargetName} INTERFACE)
    add_library(aclp::${ARG_NAME}-info ALIAS ${infoTargetName})
    target_sources(${infoTargetName}
        INTERFACE FILE_SET HEADERS FILES
            ${infoTargetName}.h
    )
endfunction()
