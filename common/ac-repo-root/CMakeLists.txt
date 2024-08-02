configure_file(ac-repo-root.in.h ac-repo-root.h @ONLY)
add_library(ac-repo-root INTERFACE)
add_library(ac-dev::repo-root ALIAS ac-repo-root)
target_include_directories(ac-repo-root INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
