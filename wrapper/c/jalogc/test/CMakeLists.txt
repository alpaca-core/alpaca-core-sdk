# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_unity_lib_test(logging jalogc
    t-logging.c
    t-more-logging.cpp
    LIBRARIES
        ac::jalog
)
