# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
add_ac_lib(ac-jalog AC_JALOG)
add_library(ac::jalog ALIAS ac-jalog)
set_target_properties(ac-jalog PROPERTIES
    EXPORT_NAME ac::jalog
)

target_sources(ac-jalog
    INTERFACE FILE_SET HEADERS FILES
        ac/jalog/API.h

        ac/jalog/Level.hpp
        ac/jalog/ScopeDesc.hpp
        ac/jalog/Entry.hpp

        ac/jalog/Scope.hpp
        ac/jalog/Logger.hpp
        ac/jalog/Sink.hpp
        ac/jalog/SinkPtr.hpp

        ac/jalog/_qwrite.hpp

        ac/jalog/BasicStream.hpp

        ac/jalog/Printf.hpp
        ac/jalog/PrintfWrap.hpp

        ac/jalog/Log.hpp
        ac/jalog/LogStream.hpp
        ac/jalog/LogPrintf.hpp

        ac/jalog/Instance.hpp

        ac/jalog/sinks/AnsiColorSink.hpp
        ac/jalog/sinks/SimpleOStreamSink.hpp
        ac/jalog/sinks/SimpleStdioSink.hpp

        ac/jalog/AsyncLogging.hpp
        ac/jalog/AsyncLoggingThread.hpp
    PRIVATE

        ac/jalog/Scope.cpp
        ac/jalog/Logger.cpp
        ac/jalog/Sink.cpp

        ac/jalog/_qwrite.cpp

        ac/jalog/Printf.cpp

        ac/jalog/Instance.cpp

        ac/jalog/sinks/AnsiColorSink.cpp

        ac/jalog/AsyncLogging.cpp
)

if(WIN32)
    target_sources(ac-jalog
        INTERFACE FILE_SET HEADERS FILES
            ac/jalog/sinks/WindowsColorSink.hpp
        PRIVATE ac/jalog/sinks/WindowsColorSink.cpp
    )
elseif(ANDROID)
    target_sources(ac-jalog
        INTERFACE FILE_SET HEADERS FILES
            ac/jalog/sinks/AndroidSink.hpp
        PRIVATE ac/jalog/sinks/AndroidSink.cpp
    )
    target_link_libraries(ac-jalog PRIVATE log)
elseif(APPLE)
    target_sources(ac-jalog
        INTERFACE FILE_SET HEADERS FILES
            ac/jalog/sinks/NSLogSink.hpp
        PRIVATE ac/jalog/sinks/NSLogSink.mm
    )
    find_library(COCOA Cocoa)
    if(COCOA)
        # On iOS Cocoa does not exist as a separate framework
        # so just ignore it
        target_link_libraries(ac-jalog PRIVATE ${COCOA})
    endif()
endif()

target_compile_definitions(ac-jalog PUBLIC -DAC_JALOG_NO_BUILTIN_ASYNC=0)
target_link_libraries(ac-jalog
    PUBLIC
        ac::astl
        ac::xec
)

install(TARGETS ac-jalog
    EXPORT ac-common-targets
    FILE_SET HEADERS
)