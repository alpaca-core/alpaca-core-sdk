# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
include(${CMAKE_CURRENT_LIST_DIR}/ac-local-targets.cmake)
message(STATUS "ac-local dev @CMAKE_BUILD_TYPE@ found in ${CMAKE_CURRENT_LIST_DIR}")

if(WIN32 AND CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    # on windows we need to have the dlls in the runtime directory
    # so that the executables can find them
    if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ac-local.dll)
        # we're making symlinks, so only do it once
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_CURRENT_LIST_DIR}/bin/ac-local.dll
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ac-local.dll
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                ${CMAKE_CURRENT_LIST_DIR}/bin/ac-jalog.dll
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ac-jalog.dll
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
endif()
