# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(dep depName)
    include(${PROJECT_SOURCE_DIR}/cmake/deps/${depName}.cmake)
endmacro()
