# Dict

`Dict` (short for dictionary) is more or less a JSON object with the notable addition of the data type `binary`. In terms of data, it can be called a [CBOR](https://cbor.io/).

`Dict` itself can be a key-value store, where the keys are strings and the values can be one of the supported types, but it can also just a value type of its own, much like `5`, or `"hello"`, or `null` on their own are valid JSONs.

The supported types are:

* `null` - a null value
* `boolean` - a boolean value (`true` or `false`)
* `number` - which is one of:
    * `int` - a 32-bit signed integer
    * `unsinged` - a 32-bit unsigned integer
    * `double` - a 64-bit floating point number
* `string` - a string value
* `object` - a nested key-value store
* `array` - an ordered list of values
* `binary` - a contiguous memory buffer
