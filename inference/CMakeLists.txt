# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
# find_package(Vulkan)
# if(Vulkan_FOUND)
#    set(haveVulkan YES)
#    message(STATUS "${CMAKE_PROJECT_NAME}: system Vulkan found")
# endif()

find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    set(haveCuda YES)
    enable_language(CUDA)
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
add_subdirectory(llama.cpp)
