# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
include_guard(GLOBAL)

include(ac_local_lib)

function(add_ac_local_plugin)
    cmake_parse_arguments(ARG "" "NAME;PUBLIC_SUFFIX" "SOURCES;PLUGIN_SOURCES;PLIB_SOURCES;LIBRARIES" ${ARGN})

    # private names for targets which are not installed and never leave the project
    set(aclpName aclp-${ARG_NAME})
    set(baselibTargetName ${aclpName}-baselib)
    set(plibTargetName ${aclpName}-plib)
    set(infoTargetName ${aclpName}-info)
    string(MAKE_C_IDENTIFIER ${ARG_NAME} nameSym)

    # public names for targets which may be installed
    set(publicName ${aclpName}${ARG_PUBLIC_SUFFIX})
    set(pluginTargetName ${publicName})
    # string(MAKE_C_IDENTIFIER ${ARG_NAME} publicNameSym)

    # configure version
    file(CONFIGURE
        OUTPUT ${aclpName}-version.h
        CONTENT [=[
// Generated file. Do not edit!
#pragma once

#define ACLP_@nameSym@_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define ACLP_@nameSym@_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define ACLP_@nameSym@_VERSION_PATCH @PROJECT_VERSION_PATCH@

#define ACLP_@nameSym@_VERSION_STRING "@PROJECT_VERSION@"
]=]
        @ONLY
    )

    # add base lib
    add_library(${baselibTargetName} STATIC
        ${ARG_SOURCES}
        ${aclpName}-version.h
        ${aclpName}-interface.hpp
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
        OUTPUT ${aclpName}-plib.hpp
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
        OUTPUT ${aclpName}-plib.cpp
        CONTENT [=[
// Generated file. Do not edit!
#include "@aclpName@-plib.hpp"
#include "${aclpName}-interface.hpp"

extern "C"
void add_@nameSym@_to_ac_local(ac::local::ModelFactory& factory) {
    ac::@nameSym@::addToAcLocal(factory);
}
]=]
    )
    add_ac_local_lib(${plibTargetName} ACLPLIB_${nameSym})
    target_sources(${plibTargetName}
        PUBLIC FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${aclpName}-plib.hpp"
        PRIVATE
            ${aclpName}-plib.cpp
            ${ARG_PLIB_SOURCES}
    )
    add_library(aclp::${ARG_NAME}-plib ALIAS ${plibTargetName})
    target_link_libraries(${plibTargetName}
        PUBLIC
            ac::local
        PRIVATE
            ${baselibTargetName}
    )

    # add plugin
    file(CONFIGURE
        OUTPUT ${aclpName}-entrypoint.cpp
        CONTENT [=[
// Generated file. Do not edit!
#include <astl/symbol_export.h>
#include "@aclpName@-version.h"
#include "@aclpName@-interface.hpp"
#include <ac/local/PluginInterface.hpp>

namespace ac::local {

extern "C" SYMBOL_EXPORT
PluginInterface acLocalPluginLoad() {
    constexpr Version ownVersion(
        ACLP_@nameSym@_VERSION_MAJOR, ACLP_@nameSym@_VERSION_MINOR, ACLP_@nameSym@_VERSION_PATCH
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
        ${aclpName}-entrypoint.cpp
        ${ARG_PLUGIN_SOURCES}
    )
    add_library(aclp::${ARG_NAME} ALIAS ${pluginTargetName})
    set_target_properties(${pluginTargetName} PROPERTIES
        PREFIX "" # no lib on unix-like platforms
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
    string(CONFIGURE [=[
// Generated file. Do not edit!
#pragma once
#define ACLP_@nameSym@_PLUGIN_FILE "$<TARGET_FILE:@pluginTargetName@>"
]=]
        infoFileContentConfigured
        @ONLY
    )

    file(GENERATE
        OUTPUT ${infoTargetName}.h
        CONTENT ${infoFileContentConfigured}
        TARGET ${pluginTargetName}
    )
    add_library(${infoTargetName} INTERFACE)
    add_library(aclp::${ARG_NAME}-info ALIAS ${infoTargetName})
    target_sources(${infoTargetName}
        INTERFACE FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${infoTargetName}.h"
    )
endfunction()

function(make_ac_local_plugin_available)
    cmake_parse_arguments(ARG "SAVE_TARGET_NAME_TO" "NAME;VERSION;MONO_DIR;GITHUB" "OPTIONS" ${ARGN})

    set(pluginId "${ARG_NAME}-${ARG_VERSION}-")

    if(NOT ARG_OPTIONS)
        set(idSuffix "default")
    else()
        string(MD5 idSuffix "${ARG_OPTIONS}")
    endif()

    set(pluginId "${pluginId}${idSuffix}")

    if(ARG_SAVE_TARGET_NAME_TO)
        set(${ARG_SAVE_TARGET_NAME_TO} ${pluginId} PARENT_SCOPE)
    endif()

    if(TARGET ${pluginId})
        # already available
        return()
    endif()

    # get source directory
    if(NOT ACP_${pluginId}_SRC)

        # fetch
        if(AC_BUILD_MONO)
            if(NOT ARG_MONO_DIR)
                set(ARG_MONO_DIR ${ARG_NAME})
            endif()
            set(srcDir "${CMAKE_SOURCE_DIR}/../${ARG_MONO_DIR}")
            # TODO: check version
        else()
            CPMAddPackage(
                NAME ${ARG_NAME}
                VERSION ${ARG_VERSION}
                GITHUB_REPOSITORY ${ARG_GITHUB}
                DOWNLOAD_ONLY # don't add as a subdirectory
            )
            set(srcDir "${${ARG_NAME}_SOURCE_DIR}")
        endif()

        set(ACP_${pluginId}_SRC "${srcDir}" CACHE PATH "AC Plugin ${pluginId} source")
    else()
        set(srcDir "${ACP_${pluginId}_SRC}")
    endif()

    # get binary directory
    if(NOT ACP_${pluginId}_BIN)
        set(binDir "${CMAKE_BINARY_DIR}/_ac-plugins/${pluginId}")
        set(ACP_${pluginId}_BIN "${binDir}" CACHE PATH "AC Plugin ${pluginId} binary")
    else()
        set(binDir "${ACP_${pluginId}_BIN}")
    endif()

    # now we build a config file based on our configuration to set with cmake -C
    set(cfg "# generated cache")
    get_cmake_property(cacheVars CACHE_VARIABLES)
    foreach(var ${cacheVars})
        if(${var} STREQUAL "")
            # ignore empty
            continue()
        endif()

        if(var STREQUAL "CMAKE_ROOT")
            # not our root :)
            continue()
        endif()
        if(var STREQUAL "CMAKE_INSTALL_PREFIX")
            # we'll be overwriting that
            continue()
        endif()

        get_property(help CACHE ${var} PROPERTY HELPSTRING)
        get_property(type CACHE ${var} PROPERTY TYPE)

        if(type STREQUAL STATIC)
            # managed by CMake
            continue()
        endif()

        set(propagate NO)
        if(var STREQUAL "CMAKE_BUILD_TYPE")
            set(propagate YES)
        elseif(var MATCHES "CMAKE_GENERATOR")
            set(propagate YES)
        elseif(var MATCHES "_ROOT$")
            # ac-build root or other package
            set(propagate YES)
        elseif(var MATCHES "^AC_BUILD")
            # ac-build options
            set(propagate YES)
        elseif(var MATCHES "_CFG_HASH$")
            # don't thrash our cache
            set(propagate NO)
        elseif(var MATCHES "^ACP_")
            # ac plugin options
            set(propagate YES)
        else()
            if(help STREQUAL "No help, variable specified on the command line.")
                # provided by command line of parent project
                set(propagate YES)
            endif()
        endif()

        if(propagate)
            string(APPEND cfg "\nset(${var}\n  \"${${var}}\"\n  CACHE ${type} \"${help}\" FORCE)")
        endif()
    endforeach()

    # also propagate options
    foreach(option ${ARG_OPTIONS})
        cpm_parse_option("${option}") # does the best possible thing
        set(${OPTION_KEY} "${OPTION_VALUE}")
        string(APPEND cfg "\nset(${OPTION_KEY}\n  \"${OPTION_VALUE}\"\n CACHE STRING \"User option\" FORCE)")
    endforeach()

    # message(${cfg})

    # instead of recofiguring every time, check whether we have already configured
    # set config hash
    string(MD5 cfgHash "${cfg}")
    set(cachedCfgHash "${ACP_${pluginId}_CFG_HASH}")
    set(cfgFile "${CMAKE_BINARY_DIR}/_ac-plugins/${pluginId}-cfg.cmake")

    set(optCfgCmd)

    if(NOT EXISTS "${binDir}" OR NOT cfgHash STREQUAL cachedCfgHash)
        # update hash
        file(WRITE "${cfgFile}" "${cfg}")
        set(ACP_${pluginId}_CFG_HASH "${cfgHash}" CACHE STRING "AC Plugin ${pluginId} config hash" FORCE)
    endif()

    # configure command which depends on the (possibly update cache cfg file)
    add_custom_command(
        COMMENT "Configuring AC Plugin ${pluginId}"
        OUTPUT "${binDir}/CMakeCache.txt"
        DEPENDS "${cfgFile}"
        COMMAND ${CMAKE_COMMAND}
            -S "${srcDir}"
            -B "${binDir}"
            -C "${cfgFile}"
            -DAC_BUILD_COMPONENT=1 # override cache value
    )

    add_custom_target(cfg-${pluginId}
        ALL
        DEPENDS "${binDir}/CMakeCache.txt"
    )

    # add the target
    add_custom_target(${pluginId}
        ALL # always build
        COMMAND ${CMAKE_COMMAND}
            --build "${binDir}"
            --config $<CONFIG>
        COMMAND ${CMAKE_COMMAND}
            --install "${binDir}"
            --config $<CONFIG>
            --prefix ${CMAKE_BINARY_DIR}/_ac-plugins
        COMMENT "Building AC Plugin ${pluginId}"
    )

    add_dependencies(${pluginId}
        cfg-${pluginId}
        ac::local
    )
endfunction()
