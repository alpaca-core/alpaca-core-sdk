# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
if(TARGET OpenSSL::SSL)
    # comes from "above"
    return()
endif()

if(ANDROID)
    set(OPENSSL_USE_STATIC_LIBS YES) # related to #90

    # https://github.com/viaduck/openssl-cmake
    CPMAddPackage(
        NAME openssl-android
        VERSION 3.3.1
        URL https://builds.viaduck.org/prebuilts/openssl/3.3.1/${CMAKE_ANDROID_ARCH_ABI}-android.tar.gz
    )

    list(APPEND CMAKE_FIND_ROOT_PATH ${openssl-android_SOURCE_DIR})
    set(OPENSSL_ROOT_DIR ${openssl-android_SOURCE_DIR}/usr/local)
endif()

find_package(OpenSSL)
