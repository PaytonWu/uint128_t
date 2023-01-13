# uint128_t

An unsigned 128 bit integer type for C++

Copyright (c) 2013 - 2018 Jason Lee @ calccrypto at gmail.com

Please see LICENSE file for license.

[![uint128_t](https://github.com/calccrypto/uint128_t/actions/workflows/uint128_t.yml/badge.svg)](https://github.com/calccrypto/uint128_t/actions/workflows/uint128_t.yml)

## Acknowledgements
With much help from Auston Sterling

Thanks to Stefan Deigmüller for finding
a bug in operator*.

Thanks to François Dessenne for convincing me
to do a general rewrite of this class.

Thanks to John Skaller for making symbols visible
when compiling as a shared library. This was originally
done in `uint256_t`, which I copied into here.

## Usage
This is simple implementation of an unsigned 128 bit
integer type in C++. It's meant to be used like a standard
`uintX_t`, except with a larger bit size than those provided
by C/C++.

### In Code
All that needs to be done in code is `#include "uint128/uint128_t.h"`

```c++
#include <iostream>
#include "uint128/uint128_t.h"

int main() {
    uint128_t a = 1;
    uint128_t b = 2;
    std::cout << (a | b) << std::endl;
    return 0;
}
```

### Compilation
A C++ compiler supporting at least C++20 is required.

Compilation can be done by using cmake toolchain.

1. `mkdir build && cd build`
2. `cmake ..`
3. `ninja`

It builds static library by default. For building shared library, run `cmake -DBUILD_SHARED_LIBS=ON ..` at step #2.

If you want to build and run tests, pass `-DWITH_TESTS=ON` to cmake at step #2 and executing `ctest` after step #3.
