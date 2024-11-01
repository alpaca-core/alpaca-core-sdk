# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# This version here is important. Don't be tempted to add later ones
# Later versions require additional packages, and we don't actually want to build a Java package manager here
set(junitVersion 4.10)
set(junitJar "${CMAKE_CURRENT_SOURCE_DIR}/lib/junit-${junitVersion}.jar")
if(NOT EXISTS ${junitJar})
    # download
    message(STATUS "Downloading ${junitJar}")
    file(DOWNLOAD https://repo1.maven.org/maven2/junit/junit/${junitVersion}/junit-${junitVersion}.jar
        "${junitJar}"
        EXPECTED_HASH SHA1=e4f1766ce7404a08f45d859fb9c226fc9e41a861
    )
endif()

if(WIN32)
    set(sep ";")
else()
    set(sep ":")
endif()

function(add_java_test name)
    cmake_parse_arguments(ARG "" "" "SOURCES;JARS;LIBS" ${ARGN})
    set(tgt test-ac-java-${name})
    add_jar(${tgt}
        SOURCES
            src/main/com/alpacacore/testutil
            ${ARG_SOURCES}
        INCLUDE_JARS
            ${junitJar}
            ${ARG_JARS}
    )

    set(libPaths)
    foreach(lib IN LISTS ARG_LIBS)
        list(APPEND libPaths $<TARGET_FILE_DIR:${lib}>)
    endforeach()
    list(JOIN libPaths "${sep}" javaLibraryPath)

    set(jarPaths
        ${junitJar}
        $<TARGET_PROPERTY:${tgt},JAR_FILE>
    )
    foreach(jar IN LISTS ARG_JARS)
        list(APPEND jarPaths $<TARGET_PROPERTY:${jar},JAR_FILE>)
    endforeach()
    list(JOIN jarPaths "${sep}" javaClassPath)

    add_test(
        NAME
            ac-java-${name}
        COMMAND
            ${Java_JAVA_EXECUTABLE}
            "-Djava.library.path=${javaLibraryPath}"
            -cp "${javaClassPath}"
            org.junit.runner.JUnitCore com.alpacacore.Test${name}
    )
    if(WIN32)
        # with no rpath on windows we need to set the path to the related dlls
        set_tests_properties(ac-java-${name} PROPERTIES ENVIRONMENT_MODIFICATION "Path=path_list_prepend:${javaLibraryPath}")
    endif()
endfunction()

ac_add_module(test-ac-java-dict t-JniDict.cpp)
target_link_libraries(test-ac-java-dict PRIVATE ac-jni-dict)
add_java_test(Dict
    SOURCES TestDict.java
    LIBS test-ac-java-dict
)

set(testFixturesDir "../src/testFixtures/java/com/alpacacore") # ridiculous

add_java_test(LocalDummy
    SOURCES ${testFixturesDir}/TestLocalDummy.java
    JARS alpaca-core-jar
    LIBS ac-jni
)
