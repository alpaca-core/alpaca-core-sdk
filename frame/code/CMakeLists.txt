# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-frame AC_FRAME)
add_library(ac::frame ALIAS ac-frame)
set_target_properties(ac-frame PROPERTIES
    EXPORT_NAME ac::frame
)

if(NOT TARGET Boost::boost)
    # only add boost if not added from the outside
    CPMAddPackage(gh:iboB/boost-trim@1.85.0)
endif()

if(NOT TARGET Boost::asio)
    # asio is header only, so this is fine
    add_library(Boost::asio ALIAS Boost::boost)
endif()

target_link_libraries(ac-frame
    PUBLIC
        ac::astl
    PRIVATE
        Boost::asio
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
        ac/frameio/Status.hpp
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
        ac/frameio/SessionCoro.hpp

        # frameio-local
        ac/frameio/local/LocalChannel.hpp
        ac/frameio/local/LocalChannelPtr.hpp
        ac/frameio/local/LocalBufferedChannel.hpp
        ac/frameio/local/BlockingIo.hpp
        ac/frameio/local/SyncIo.hpp
        ac/frameio/local/BlockingSyncIoWrapper.hpp
        ac/frameio/local/LocalIoCtx.hpp
        ac/frameio/local/LocalIoRunner.hpp

        # schema
        ac/schema/Field.hpp
        ac/schema/SerializeVisitors.hpp
        ac/schema/SchemaVisitors.hpp
        ac/schema/FrameHelpers.hpp
        ac/schema/OpDispatchHelpers.hpp

    PRIVATE
        ac/frameio/Stream.cpp
        ac/frameio/Io.cpp
        ac/frameio/SessionHandler.cpp
        ac/frameio/SessionCoro.cpp

        ac/frameio/local/LocalChannel.cpp
        ac/frameio/local/LocalBufferedChannel.cpp
        ac/frameio/local/BlockingIo.cpp
        ac/frameio/local/SyncIo.cpp
        ac/frameio/local/BlockingSyncIoWrapper.cpp
        ac/frameio/local/LocalIoCtx.cpp
        ac/frameio/local/LocalIoRunner.cpp
)

install(TARGETS ac-frame
    EXPORT ac-frame-targets
    FILE_SET HEADERS
)
