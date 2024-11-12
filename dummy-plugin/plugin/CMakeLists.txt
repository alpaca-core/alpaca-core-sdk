# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# we need a project with a version to make add_ac_plugin work
project(dummy-plugin
    VERSION ${ac-local_VERSION}
)

include(ac_build_plugin_util)

add_ac_plugin(
    NAME dummy
    SOURCES
        DummyPlugin.cpp
    LIBRARIES
        ac::dummy-lib
)
