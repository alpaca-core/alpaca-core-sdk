# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-xec AC_XEC)
add_library(ac::xec ALIAS ac-xec)
set_target_properties(ac-xec PROPERTIES
    EXPORT_NAME ac::xec
)

ac_dep(boost)

target_link_libraries(ac-xec
    PUBLIC
        ac::astl
    PRIVATE
        Boost::asio
)

target_sources(ac-xec
    INTERFACE FILE_SET HEADERS FILES
        ac/xec/api.h

        ac/xec/context.hpp
        ac/xec/context_work_guard.hpp
        ac/xec/post.hpp
        ac/xec/strand.hpp
        ac/xec/task.hpp
        ac/xec/timer.hpp
        ac/xec/ufunc.hpp

        ac/xec/notifiable.hpp

        ac/xec/thread_name.hpp
    PRIVATE
        ac/xec/xec-asio-impl.cpp
        ac/xec/notifiable.cpp
        ac/xec/thread_name.cpp
)

install(TARGETS ac-xec
    EXPORT ac-common-targets
    FILE_SET HEADERS
)
