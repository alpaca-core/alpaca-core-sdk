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
    LIBRARIES
        ac::schema
)

# install plib and info  to make life easier for external projects
# which want to add simple tests and examples
set_target_properties(aclp-dummy-plib PROPERTIES
    EXPORT_NAME aclp::dummy-plib
)
set_target_properties(aclp-dummy-info PROPERTIES
    EXPORT_NAME aclp::dummy-info
)
install(TARGETS aclp-dummy-plib aclp-dummy-info
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
