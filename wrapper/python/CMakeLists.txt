# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_auto_option(AC_PYTHON_WRAPPER "${CMAKE_PROJECT_NAME}: build python wrapper library" ${AC_WRAPPER_DEFAULT_VALUE})

find_package(Python ${AC_PYTHON_WRAPPER_REQUIRED})
