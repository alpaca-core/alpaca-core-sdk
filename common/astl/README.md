# ASTL

C++ header-only libraries which are mostly STL extensions. 

No external dependencies are allowed. If needed, copy the code from the external library here.

## Copied files

The following files were copied from external repositories:

* [iboB/splat](https://github.com/iboB/itlib):
    * `pp_select.h`
    * `symbol_export.h`
    * `warnings.h`
* [iboB/itlib](https://github.com/iboB/itlib), namespace changed from `itlib` to `astl`:
    * `expected.hpp`
    * `flat_map.hpp`
    * `generator.hpp`
    * `make_ptr.hpp`
    * `mem_streambuf.hpp`
    * `pod_vector.hpp`
    * `qalgorithm.hpp`
    * `sentry.hpp`
    * `shared_from_.hpp`
    * `small_vector.hpp`
    * `throw_ex.hpp`
    * `time_t.hpp`
    * `ufunction.hpp`

# Why embed?

The reason for embedding instead of using it as a package can be found [here](https://github.com/alpaca-core/ac-local/issues/177#issuecomment-2456690091)
