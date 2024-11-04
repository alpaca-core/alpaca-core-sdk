ac_dep(splat)
add_library(astl INTERFACE)
add_library(ac::astl-private ALIAS astl)
target_include_directories(astl INTERFACE .)
target_link_libraries(astl INTERFACE
    splat::splat
    itlib::itlib
    ac::astl-public
)
