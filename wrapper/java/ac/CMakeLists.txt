# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

set(jdir src/main/java/com/alpacacore) # ridiculous

add_jar(alpaca-core-jar
    SOURCES
        ${jdir}/Sandbox.java
        ${jdir}/Instance.java
        ${jdir}/Model.java
        ${jdir}/ModelDesc.java
        ${jdir}/LocalProvider.java
        ${jdir}/Impl.java
    OUTPUT_NAME
        alpaca-core
)
