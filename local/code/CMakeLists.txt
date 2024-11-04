# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)
target_include_directories(ac-local INTERFACE .)
target_link_libraries(ac-local
    PRIVATE
        jalog::jalog
        ac::astl-private
    PUBLIC
        ac::astl-public
        ac::dict
        ac::schema
)
target_sources(ac-local PRIVATE
    ac/local/export.h

    ac/local/Logging.hpp
    ac/local/Logging.cpp

    ac/local/ModelDesc.hpp
    ac/local/ProgressCb.hpp
    ac/local/ModelPtr.hpp
    ac/local/ModelLoader.hpp
    ac/local/Model.hpp
    ac/local/Instance.hpp
    ac/local/VtableExports.cpp

    ac/local/ModelFactory.hpp
    ac/local/ModelFactory.cpp
)
