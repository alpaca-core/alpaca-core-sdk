# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

set(jdir main/java/com/alpacacore) # ridiculous

add_jar(alpaca-core-jar
    SOURCES
        ${jdir}/AlpacaCore.java
        ${jdir}/Instance.java
        ${jdir}/Model.java
        ${jdir}/ModelDesc.java
        ${jdir}/ProgressCallback.java
    OUTPUT_NAME
        alpaca-core
)
