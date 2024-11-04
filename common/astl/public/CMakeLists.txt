add_library(astl-public INTERFACE)
add_library(ac::astl-public ALIAS astl-public)
target_include_directories(astl-public INTERFACE .)

# DO NOT USE THIS
# The library must remain without any dependencies
# Any required dependencies must be copied inside
# target_link_libraries(astl INTERFACE )
