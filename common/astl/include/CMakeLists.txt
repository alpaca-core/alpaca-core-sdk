add_library(astl INTERFACE)
add_library(ac::astl ALIAS astl)
set_target_properties(astl PROPERTIES
    EXPORT_NAME ac::astl
)
target_sources(astl INTERFACE FILE_SET HEADERS FILES
    astl/symbol_export.h
    astl/mem_streambuf.hpp
    astl/safe_func.hpp
    astl/small_vector.hpp
    astl/time_t.hpp
    astl/transparent_string_hash.hpp
    astl/tsumap.hpp
    astl/tuple_util.hpp
    astl/ufunction.hpp
)

# DO NOT USE THIS
# The library must remain without any dependencies
# Any required dependencies must be copied inside
# target_link_libraries(astl INTERFACE )
install(TARGETS astl
    EXPORT ac-local-targets
    FILE_SET HEADERS
)
