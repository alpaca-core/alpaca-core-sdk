# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
# find_package(Vulkan)
# if(Vulkan_FOUND)
#    set(haveVulkan YES)
#    message(STATUS "${CMAKE_PROJECT_NAME}: system Vulkan found")
# endif()

ac_dep(splat)

find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    set(haveCuda YES)
    enable_language(CUDA)
    set(CMAKE_CUDA_ARCHITECTURES 70)
    message(STATUS "${CMAKE_PROJECT_NAME}: system CUDA found")
    # we should add -forward-unknown-to-host-compiler but it somehow works without it
    # could it be that it depends on the CMake version?
endif()

if(haveVulkan)
    set(GGML_VULKAN ON)
endif()

if(haveCuda)
    set(GGML_CUDA ON)
endif()

set(GGML_CCACHE OFF)
add_subdirectory(ggml)

if(AC_INFERENCE_LIBS STREQUAL "all")
    set(libs
        dummy
        llama.cpp
        whisper
    )
else()
    set(libs "${AC_INFERENCE_LIBS}")
endif()

foreach(lib IN LISTS libs)
    add_subdirectory(${lib})
endforeach()
