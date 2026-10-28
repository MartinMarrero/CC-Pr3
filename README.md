# Primitive Recursive Power

This project implements a small language of primitive recursive functions in
C++17. It defines zero, successor, and projection functions, then combines
them with composition and primitive recursion to calculate `x^y`.

## Author

Martín José Marrero Quintans

## Structure

```text
include/   Public class and function declarations
src/       Implementations and command-line entry point
build/     Generated object files and executable
Makefile   Build and run targets
```

## Build and run

```sh
make
./build/potencia -x 2 -y 5
```

The program also supports `-h` and `--help`. Both `-x` and `-y` must be
non-negative integers and must be provided exactly once.

Example output:

```text
Result: 32
Function calls: 332
```

## Design

`Function` is the abstract Composite component. Its non-virtual `Evaluate`
method validates arity, increments a shared `CallCounter`, and delegates the
calculation to the protected virtual `Compute` method. This ensures every
function invocation is counted uniformly.

`Composition` contains an outer function and a list of inner functions.
`PrimitiveRecursion` contains the base and step functions. `MakeSum`,
`MakeProduct`, and `MakePower` are factories that assemble these components
using `std::shared_ptr<const Function>`.

## Mathematical definitions

The recursion variable is always the last argument:

| Function | Base case (`y = 0`) | Step (`y + 1`) |
| --- | --- | --- |
| `sum(x, y)` | `x` | `successor(result)` |
| `product(x, y)` | `0` | `sum(result, x)` |
| `power(x, y)` | `1` | `product(result, x)` |

These are assembled from:

- `Zero`: always returns `0`.
- `Successor`: returns `x + 1`.
- `Projection`: returns one selected argument.

## Call counting

One call means one successful invocation of `Evaluate` on any `Function`
object, including basic functions, compositions, and recursive levels.
Argument validation happens before counting. The count therefore grows quickly:
the implementation uses repeated successor operations for addition, so
products and powers expand into many nested function calls.

## Edge cases and limitations

- `0^0` is `1`, because the base case of `power` is the constant `1`.
- `0^y` is `0` for every positive `y`.
- Arithmetic overflow is detected by `Successor` and reported with exit code 2.
- Primitive recursion is implemented literally with recursive C++ calls.
  Very large exponents can exhaust the call stack.
- Because arithmetic is built from repeated successors, large values can take
  considerable time even when the mathematical result fits in the type.