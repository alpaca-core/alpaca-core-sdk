# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# misc config
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LINK_DEPENDS_NO_SHARED ON) # only relink exe if so interface changes
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # use solution folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # binaries to bin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # modules to bin

# install config
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path")
else()
    set(CMAKE_INSTALL_RPATH "\$ORIGIN")
endif()

if(MSVC)
    # /Zc:preprocessor - incompatible with Windows.h
    add_compile_options(
        -W4
        -D_CRT_SECURE_NO_WARNINGS -Zc:__cplusplus -permissive-
        -volatile:iso -Zc:throwingNew -Zc:templateScope -utf-8 -DNOMINMAX=1
        -wd4251 -wd4275
    )
else()
    add_compile_options(-Wall -Wextra)
endif()

# sanitizers
option(SAN_THREAD "${CMAKE_PROJECT_NAME}: sanitize thread" OFF)
option(SAN_ADDR "${CMAKE_PROJECT_NAME}: sanitize address" OFF)
option(SAN_UB "${CMAKE_PROJECT_NAME}: sanitize undefined behavior" OFF)
option(SAN_LEAK "${CMAKE_PROJECT_NAME}: sanitize leaks" OFF)

if(MSVC)
    if(SAN_ADDR)
        add_compile_options(-fsanitize=address)
    endif()
    if(SAN_THREAD OR SAN_UB OR SAN_LEAK)
        message(WARNING "Unsupported sanitizers requested for msvc. Ignored")
    endif()
else()
    if(SAN_THREAD)
        set(sanitizerFlags -fsanitize=thread -g)
        if(SAN_ADDR OR SAN_UB OR SAN_LEAK)
            message(WARNING "Incompatible sanitizer combination requested. Only 'SAN_THREAD' will be respected")
        endif()
    else()
        if(SAN_ADDR)
            list(APPEND sanitizerFlags -fsanitize=address -pthread)
        endif()
        if(SAN_UB)
            list(APPEND sanitizerFlags -fsanitize=undefined)
        endif()
        if(SAN_LEAK)
            if(APPLE)
                message(WARNING "Unsupported leak sanitizer requested for Apple. Ignored")
            else()
                list(APPEND sanitizerFlags -fsanitize=leak)
            endif()
        endif()
    endif()
    if(sanitizerFlags)
        add_compile_options(${sanitizerFlags})
        add_link_options(${sanitizerFlags})
    endif()
endif()
