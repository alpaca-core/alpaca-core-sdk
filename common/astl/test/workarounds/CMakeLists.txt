# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# test various workaround from workarounds.h
# each one requires a separate test case to avoid false positives

if(MSVC)
    # test with a compilation failure which expects the warning
    icm_add_build_failure_test(
        NAME astl-bf-msvc_wo_10766806
        SOURCES PARSE t-msvc_wo_10766806.cpp
        FOLDER test
    )
endif()
