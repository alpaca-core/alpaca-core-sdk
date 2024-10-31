# This source file is part of the Swift open source project
#
# Copyright (c) 2023 Apple Inc. and the Swift project authors.
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See https://swift.org/LICENSE.txt for license information


# Generate the bridging header from Swift to C++
#
# target: the name of the target to generate headers for.
#         This target must build swift source files.
# header: the name of the header file to generate.
#
# NOTE: This logic will eventually be unstreamed into CMake.
function(_swift_generate_cxx_header target header)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} not defined.")
    endif()

    cmake_parse_arguments(ARG "" "" "SWIFT_EXPOSABLE_FILES" ${ARGN})

    if(APPLE)
        if(CMAKE_GENERATOR STREQUAL Xcode)
            # Force MacOSX SDK since it's more difficult to get the correct target for iPhone SDK.
            # Otherwise when we use iPhone SDK without setting a target, there is an error:
            # error: unable to load standard library for target 'arm64-apple-macosx15.0'
            execute_process(
                COMMAND xcrun --sdk macosx --show-sdk-path
                OUTPUT_VARIABLE MACOSX_SDK_PATH
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            set(SDK_FLAGS "-sdk" "${MACOSX_SDK_PATH}")
        else()
            set(SDK_FLAGS "-sdk" "${CMAKE_OSX_SYSROOT}")
        endif()
    elseif(WIN32)
        set(SDK_FLAGS "-sdk" "$ENV{SDKROOT}")
    elseif(DEFINED ${CMAKE_SYSROOT})
        set(SDK_FLAGS "-sdk" "${CMAKE_SYSROOT}")
    endif()

    cmake_path(APPEND CMAKE_CURRENT_BINARY_DIR include
        OUTPUT_VARIABLE base_path
    )

    cmake_path(APPEND base_path ${header}
        OUTPUT_VARIABLE header_path
    )

    add_custom_command(OUTPUT ${header_path}
        DEPENDS ${ARG_SWIFT_EXPOSABLE_FILES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND
            ${CMAKE_Swift_COMPILER} -frontend -typecheck
            ${ARG_SWIFT_EXPOSABLE_FILES}
            ${SDK_FLAGS}
            -module-name "${target}"
            -cxx-interoperability-mode=default
            -emit-clang-header-path ${header_path}
        COMMENT
            "Generating '${header_path}'"
        COMMAND_EXPAND_LISTS
    )

    # Add to public interface for dependees to find.
    target_include_directories(${target} PUBLIC ${base_path})
    # Add to the target to ensure target rebuilds if header changes and is used
    # by sources in the target.
    target_sources(${target} PRIVATE ${header_path})
endfunction()
