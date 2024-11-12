add_library(ac-test-data-dummy INTERFACE)
add_library(ac-test-data::dummy ALIAS ac-test-data-dummy)
configure_file(ac-test-data-dummy-dir.in.h "${PROJECT_SOURCE_DIR}/gen/ac-test-data-dummy-dir.h")
target_include_directories(ac-test-data-dummy INTERFACE
    .
    "${PROJECT_SOURCE_DIR}/gen"
)
