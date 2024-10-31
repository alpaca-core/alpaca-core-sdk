# Alpaca Core Local SDK Swift Wrapper

Here we have both a high level and low level wrapper.

## `AlpacaCoreSwift`

A high level wrapper. It's interface should feel as idiomatic Swift. `Dict` is passed as `Dictionary`.

It's implemented via...

## `CxxAlpacaCore`

A low-level wrapper which exposes more C++ internals in the pseudo-structure `AC`.

`Dict` is accessible as `AC.DictRoot` and `AC.DictRef` in a manner similar to the C API. 
