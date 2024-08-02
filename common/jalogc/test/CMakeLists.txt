# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_test(
    NAME jalogc
    TARGET test-ac-jalogc
    LIBRARIES
        unity::framework
        ac::jalogc
        ac-dev::test-util
        jalog::jalog
    SOURCES
        t-logging.c
        t-more-logging.cpp
    FOLDER test
)
