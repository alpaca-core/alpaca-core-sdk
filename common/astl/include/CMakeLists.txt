add_library(astl INTERFACE)
add_library(ac::astl ALIAS astl)
set_target_properties(astl PROPERTIES
    EXPORT_NAME ac::astl
)
target_sources(astl INTERFACE FILE_SET HEADERS FILES
    astl/expected.hpp
    astl/flat_map.hpp
    astl/generator.hpp
    astl/iile.h
    astl/mem_ext.hpp
    astl/mem_streambuf.hpp
    astl/move.hpp
    astl/move_capture.hpp
    astl/pp_select.h
    astl/qalgorithm.hpp
    astl/safe_func.hpp
    astl/sentry.hpp
    astl/small_vector.hpp
    astl/symbol_export.h
    astl/throw_ex.hpp
    astl/throw_stdex.hpp
    astl/time_t.hpp
    astl/tuple_util.hpp
    astl/u8c.h
    astl/ufunction.hpp
    astl/version.hpp
    astl/workarounds.h
)

# DO NOT USE THIS
# The library must remain without any dependencies
# Any required dependencies must be copied inside
# target_link_libraries(astl INTERFACE )

install(TARGETS astl
    EXPORT ac-common-targets
    FILE_SET HEADERS
)
