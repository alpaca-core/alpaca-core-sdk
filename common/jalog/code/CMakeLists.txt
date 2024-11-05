# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-jalog AC_JALOG)
add_library(ac::jalog ALIAS ac-jalog)

target_sources(ac-jalog PRIVATE
    ac/jalog/API.h

    ac/jalog/Level.hpp
    ac/jalog/ScopeDesc.hpp
    ac/jalog/Entry.hpp

    ac/jalog/Scope.hpp
    ac/jalog/Scope.cpp
    ac/jalog/Logger.hpp
    ac/jalog/Logger.cpp

    ac/jalog/Sink.hpp
    ac/jalog/Sink.cpp

    ac/jalog/_qwrite.hpp
    ac/jalog/_qwrite.cpp

    ac/jalog/BasicStream.hpp

    ac/jalog/Printf.hpp
    ac/jalog/Printf.cpp
    ac/jalog/PrintfWrap.hpp

    ac/jalog/Log.hpp
    ac/jalog/LogPrintf.hpp

    ac/jalog/Instance.hpp
    ac/jalog/Instance.cpp

    ac/jalog/sinks/AnsiColorSink.hpp
    ac/jalog/sinks/AnsiColorSink.cpp
)

if(WIN32)
    target_sources(ac-jalog PRIVATE
        ac/jalog/sinks/WindowsColorSink.hpp
        ac/jalog/sinks/WindowsColorSink.cpp
    )
elseif(ANDROID)
    target_sources(ac-jalog PRIVATE
        ac/jalog/sinks/AndroidSink.hpp
        ac/jalog/sinks/AndroidSink.cpp
    )
    target_link_libraries(ac-jalog PRIVATE log)
elseif(APPLE)
    target_sources(ac-jalog PRIVATE
        ac/jalog/sinks/NSLogSink.hpp
        ac/jalog/sinks/NSLogSink.mm
    )
    find_library(COCOA Cocoa)
    if(COCOA)
        # On iOS Cocoa does not exist as a separate framework
        # so just ignore it
        target_link_libraries(ac-jalog PRIVATE ${COCOA})
    endif()
endif()

target_include_directories(ac-jalog INTERFACE .)
target_compile_definitions(ac-jalog PUBLIC -DAC_JALOG_NO_BUILTIN_ASYNC=1)
target_link_libraries(ac-jalog
    PUBLIC
        ac::astl-public
    PRIVATE
        itlib::itlib
)

