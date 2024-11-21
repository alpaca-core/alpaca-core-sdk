# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
include("${CMAKE_CURRENT_LIST_DIR}/ac-local-targets.cmake")
message(STATUS "ac-local @PROJECT_VERSION@ @CMAKE_BUILD_TYPE@ found in ${CMAKE_CURRENT_LIST_DIR}")

if(WIN32 AND CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    # on windows we need to have the dlls in the runtime directory
    # so that the executables can find them
    if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ac-local.dll)
        # we're making symlinks, so only do it once
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${CMAKE_CURRENT_LIST_DIR}/bin/ac-local.dll"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ac-local.dll"
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${CMAKE_CURRENT_LIST_DIR}/bin/ac-jalog.dll"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ac-jalog.dll"
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${CMAKE_CURRENT_LIST_DIR}/bin/aclp-dummy-plib.dll"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/aclp-dummy-plib.dll"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
endif()

# also add cmake lib
list(APPEND CMAKE_MODULE_PATH "@CMAKE_CURRENT_SOURCE_DIR@/cmake")
include(ac_local_plugin_util)

# export tools
set(GENERATE_CXX_SCHEMA_RB "@GENERATE_CXX_SCHEMA_RB@")
