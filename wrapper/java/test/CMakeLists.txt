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

add_jar(test-ac-java-Adder
    SOURCES
        TestAdder.java
    INCLUDE_JARS
        alpaca-core-java
        ${junitJar}
)

if(WIN32)
    set(sep ";")
else()
    set(sep ":")
endif()

add_test(NAME ac-java-Adder COMMAND ${Java_JAVA_EXECUTABLE}
    -Djava.library.path=$<TARGET_FILE_DIR:ac-jni>
    -cp "$<TARGET_PROPERTY:alpaca-core-java,JAR_FILE>${sep}${junitJar}${sep}$<TARGET_PROPERTY:test-ac-java-Adder,JAR_FILE>"
    org.junit.runner.JUnitCore com.alpacacore.api.TestAdder
)
