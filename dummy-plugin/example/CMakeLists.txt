# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(tgt example-ac-local-gen-dummy-schema)
add_executable(${tgt}
    e-gen-dummy-schema.cpp
)
target_link_libraries(${tgt}
    aclp-dummy-baselib
)
