# ASTL

C++ header-only libraries which are mostly STL extensions. 

Think of this as alpaca-core boost (though much more humble)

This directory defines two targets

* `astl-public`: A library which is suitable to be exposed to plugins. It has no external dependencies other than the standard library.
* `astl-private`: A library which is suitable to be used internally by the project (or plugins). It depends on `astl-public` and third party libraries.

> IMPORTANT!
> Do not publicly expose `astl-private` in the Language API. It is a recipe for target clashes and ODR violations.
