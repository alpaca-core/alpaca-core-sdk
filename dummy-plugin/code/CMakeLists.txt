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

# install plib to make life easier for external projects
# which want to add simple tests and examples
set_target_properties(aclp-dummy-plib PROPERTIES
    EXPORT_NAME aclp::dummy-plib
)
install(TARGETS aclp-dummy-plib
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
