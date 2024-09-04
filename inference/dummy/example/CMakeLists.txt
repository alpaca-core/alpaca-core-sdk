# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(tgt example-ac-dummy-basic)
add_executable(${tgt} e-basic.cpp)
target_link_libraries(${tgt} PRIVATE
    ac::dummy
    jalog::jalog
    jalog::sinklib
    ac-test-data::dummy
)
set_target_properties(${tgt} PROPERTIES FOLDER example)
