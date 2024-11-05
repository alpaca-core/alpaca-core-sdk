# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local AC_LOCAL)
add_library(ac::local ALIAS ac-local)

target_link_libraries(ac-local
    PRIVATE
        ac::jalog
        ac::astl-private
    PUBLIC
        ac::astl-public
        ac::dict
        ac::schema
)
target_sources(ac-local
    INTERFACE FILE_SET HEADERS FILES
        ac/local/export.h

        ac/local/ModelDesc.hpp
        ac/local/ProgressCb.hpp
        ac/local/ModelPtr.hpp
        ac/local/ModelLoader.hpp
        ac/local/Model.hpp
        ac/local/Instance.hpp

        ac/local/ModelFactory.hpp
    PRIVATE
        ac/local/Logging.hpp
        ac/local/Logging.cpp

        ac/local/VtableExports.cpp

        ac/local/ModelFactory.cpp
)

install(TARGETS ac-local
    FILE_SET HEADERS
)
