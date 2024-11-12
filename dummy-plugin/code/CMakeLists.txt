# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_ac_local_plugin(
    NAME dummy
    SOURCES
        ac/dummy/Logging.hpp
        ac/dummy/Logging.cpp
        ac/dummy/Model.hpp
        ac/dummy/Model.cpp
        ac/dummy/Instance.hpp
        ac/dummy/Instance.cpp
        ac/dummy/LocalDummy.hpp
        ac/dummy/LocalDummy.cpp
)
