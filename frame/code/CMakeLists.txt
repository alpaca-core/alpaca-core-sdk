# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-frame AC_FRAME)
add_library(ac::frame ALIAS ac-frame)
set_target_properties(ac-frame PROPERTIES
    EXPORT_NAME ac::frame
)

ac_dep(boost)

target_link_libraries(ac-frame
    PUBLIC
        ac::astl
        ac::xec
        ac::io
)

configure_file(
    version.in.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ac/version.h
    @ONLY
)

target_sources(ac-frame
    INTERFACE FILE_SET HEADERS FILES
        ac/export.h
        ac/version.h

        # dict
        ac/Dict.hpp
        ac/vendor/nlohmann/json.hpp
        ac/vendor/nlohmann/json_fwd.hpp

        # frame
        ac/Frame.hpp

        # frameio
        ac/frameio/Stream.hpp
        ac/frameio/StreamPtr.hpp
        ac/frameio/StreamEndpoint.hpp
        ac/frameio/FrameWithStatus.hpp
        ac/frameio/Io.hpp
        ac/frameio/IoExecutor.hpp
        ac/frameio/IoPtr.hpp
        ac/frameio/IoCommon.hpp
        ac/frameio/BasicStreamIo.hpp
        ac/frameio/SessionHandler.hpp
        ac/frameio/SessionHandlerPtr.hpp
        ac/frameio/IoException.hpp
        ac/frameio/SessionCoro.hpp

        # frameio-local
        ac/frameio/local/BufferedChannel.hpp
        ac/frameio/local/BufferedChannelStream.hpp
        ac/frameio/local/BlockingIo.hpp
        ac/frameio/local/BlockingSyncIoWrapper.hpp
        ac/frameio/local/LocalIoCtx.hpp
        ac/frameio/local/LocalIoRunner.hpp

        # schema
        ac/schema/Field.hpp
        ac/schema/SerializeVisitors.hpp
        ac/schema/SchemaVisitors.hpp
        ac/schema/FrameHelpers.hpp
        ac/schema/OpDispatchHelpers.hpp
        ac/schema/FrameHelpers.hpp
        ac/schema/BlockingIoHelper.hpp

    PRIVATE
        ac/frameio/Stream.cpp
        ac/frameio/SessionHandler.cpp
        ac/frameio/IoException.cpp
        ac/frameio/SessionCoro.cpp

        ac/frameio/local/BlockingSyncIoWrapper.cpp
        ac/frameio/local/LocalIoCtx.cpp
        ac/frameio/local/LocalIoRunner.cpp
)

install(TARGETS ac-frame
    EXPORT ac-frame-targets
    FILE_SET HEADERS
)
