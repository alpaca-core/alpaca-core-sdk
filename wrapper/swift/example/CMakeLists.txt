# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

function(ac_add_swift_executable target)
    add_executable(${target} ${ARGN})

    # generate a valid swift module name
    string(REPLACE - _ moduleName ${target})
    
    set_target_properties(${target} PROPERTIES 
    	COMPILE_OPTIONS ""
    	Swift_MODULE_NAME ${moduleName}
   	)
    target_link_libraries(${target}
      PRIVATE
        AlpacaCoreSwift
        ac-dev::audio
    )
    target_compile_options(${target}
      PUBLIC
        $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
    )
    target_include_directories(${target}
      PUBLIC
        ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
        ${CMAKE_SOURCE_DIR}/wrapper/swift/example
    )
    target_compile_options(${target} PRIVATE -parse-as-library)
endfunction()

function(ac_swift_example name)
    ac_add_swift_executable(example-ac-swift-${name} ${ARGN})
endfunction()

ac_swift_example(Sandbox e-Sandbox.swift)
ac_swift_example(Whisper e-Whisper.swift)
ac_swift_example(Dummy e-Dummy.swift)
