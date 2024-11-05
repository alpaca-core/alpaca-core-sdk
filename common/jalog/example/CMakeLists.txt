# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
macro(jalog_example example)
    set(ename jalog-example-${example})
    add_executable(${ename} ${ARGN})
    target_link_libraries(${ename} ac-jalog)
endmacro()

jalog_example(HelloWorld e-HelloWorld.cpp)
jalog_example(BasicShowcase e-BasicShowcase.cpp)
jalog_example(StreamStyleLogging e-StreamStyleLogging.cpp)
jalog_example(PrintfStyleLogging e-PrintfStyleLogging.cpp)
jalog_example(CustomStyleLogging e-CustomStyleLogging.cpp)
