add_library(astl-public INTERFACE)
add_library(ac::astl-public ALIAS astl-public)
target_sources(astl-public INTERFACE FILE_SET HEADERS FILES
    astl/symbol_export.h
    astl/safe_func.hpp
    astl/transparent_string_hash.hpp
    astl/tsumap.hpp
    astl/tuple_util.hpp
    astl/ufunction.hpp
)

# DO NOT USE THIS
# The library must remain without any dependencies
# Any required dependencies must be copied inside
# target_link_libraries(astl INTERFACE )

install(TARGETS astl-public
    FILE_SET HEADERS
)
