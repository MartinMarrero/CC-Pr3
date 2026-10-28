# Functionality and Execution Model

This document explains how the project works internally. The program does not
use C++ multiplication or exponentiation to calculate the result. Instead, it
constructs the mathematical function `power(x, y)` from a small set of
primitive recursive building blocks.

The implementation is organized into three layers:

1. **Primitive function objects**: zero, successor, and projection.
2. **Function combinators**: composition and primitive recursion.
3. **Derived functions and application code**: sum, product, power, argument
   parsing, and result reporting.

All source code uses English identifiers, messages, and documentation.

## 1. Program goal

The executable receives two natural numbers:

```text
potencia -x <base> -y <exponent>
```

For example:

```text
./build/potencia -x 2 -y 5
```

It computes:

```text
2^5 = 32
```

It also reports how many function evaluations were performed. The counter is
part of the educational purpose of the project: it makes the cost of building
arithmetic from primitive recursion visible.

## 2. Important concepts

### 2.1 Natural numbers

The project represents natural numbers with:

```cpp
using Natural = unsigned long long;
```

This means values cannot be negative. The maximum representable value depends
on the platform and the C++ implementation.

### 2.2 Function arity

The **arity** of a function is the number of arguments it accepts.

Examples:

```text
Zero with arity 1       accepts one argument
Successor               always has arity 1
Projection(2, 3)        accepts three arguments and returns argument 2
Power                   has arity 2
```

Every call is checked before it is executed. Calling a function with the
wrong number of arguments throws `std::invalid_argument`.

### 2.3 The recursion variable

For all derived functions, the final argument is the recursion variable:

```text
sum(x, y)
product(x, y)
power(x, y)
```

The first argument is held fixed while the second argument is reduced until it
reaches zero.

## 3. The `Function` base class

The central abstraction is [`Function`](include/function.h). It represents any
primitive recursive function, including functions that are themselves built
from other functions.

The class stores an `arity_` value and exposes:

```cpp
Natural Evaluate(const Arguments& args, CallCounter& counter) const;
std::size_t Arity() const noexcept;
```

The actual implementation is supplied by the protected virtual method:

```cpp
virtual Natural Compute(const Arguments& args,
                        CallCounter& counter) const = 0;
```

### 3.1 Why `Evaluate` and `Compute` are separate

`Evaluate` is a non-virtual template-method-style entry point. It always does
the common work in the same order:

1. Check the argument count.
2. Increment the call counter.
3. Call the subclass-specific `Compute` method.

This prevents a subclass from accidentally forgetting to count an invocation.
The subclasses only implement their mathematical behavior in `Compute`.

An invalid call is rejected before the counter is incremented. Therefore, the
counter measures valid function evaluations rather than malformed attempts.

### 3.2 Call counter

[`CallCounter`](include/function.h) is a small stateful object containing one
counter. It is passed by reference to every evaluation instead of being global.

The counter includes calls to:

- The top-level `Power` object.
- Recursive levels of `PrimitiveRecursion`.
- `Composition` objects.
- Basic functions such as `Zero`, `Successor`, and `Projection`.

This gives a consistent count across the entire function tree.

## 4. Basic functions

The basic functions are implemented in
[`basic_functions.h`](include/basic_functions.h) and
[`basic_functions.cc`](src/basic_functions.cc).

### 4.1 Zero

`Zero` returns zero regardless of its arguments:

```text
Zero(x)       = 0
Zero(x, y)    = 0
Zero(x, y, z) = 0
```

Its arity is supplied to the constructor because the same mathematical zero
function is needed in contexts with different numbers of arguments.

For example, the base case of multiplication uses `Zero(1)`.

### 4.2 Successor

`Successor` implements:

```text
successor(x) = x + 1
```

It has fixed arity 1. Before adding one, it checks whether `x` is already
`std::numeric_limits<Natural>::max()`. If so, it throws
`std::overflow_error` rather than silently wrapping around to zero.

Because addition, multiplication, and power are all ultimately built using
successor, this check protects all derived operations.

### 4.3 Projection

`Projection(index, arity)` returns one selected argument. The public index is
one-based to match mathematical notation:

```text
Projection(1, 3)(a, b, c) = a
Projection(2, 3)(a, b, c) = b
Projection(3, 3)(a, b, c) = c
```

The constructor rejects an index outside the range `1 <= index <= arity`.
Internally, the implementation converts the index to zero-based vector
indexing by using `index_ - 1`.

## 5. Composition

[`Composition`](include/composition.h) implements function composition.

Given an outer function `h` and inner functions `g1`, `g2`, ..., `gm`, the
composition is:

```text
f(args) = h(g1(args), g2(args), ..., gm(args))
```

The constructor validates that:

1. The outer function is not null.
2. The inner-function list is not empty.
3. The outer arity equals the number of inner functions.
4. No inner function is null.
5. All inner functions have the same arity.

The composition's arity is the common arity of the inner functions.

### 5.1 Composition evaluation

When a composition is evaluated:

1. Each inner function is evaluated using the original arguments.
2. The intermediate results are stored in a vector.
3. The outer function is evaluated using the intermediate vector.

For example, this composition:

```text
Composition(
    Successor,
    [Projection(3, 3)]
)
```

represents:

```text
f(x, y, r) = successor(projection_3(x, y, r))
            = r + 1
```

## 6. Primitive recursion

[`PrimitiveRecursion`](include/primitive_recursion.h) implements the general
primitive recursion scheme.

It receives two functions:

- `base`, called when the recursion variable is zero.
- `step`, called after the previous recursive result is available.

If `base` has arity `n`, then `PrimitiveRecursion(base, step)` has arity
`n + 1`. The step function must have arity `n + 2` because it receives:

```text
the fixed arguments, the previous recursion value, and the previous result
```

Mathematically:

```text
f(x, 0)   = base(x)
f(x, y+1) = step(x, y, f(x, y))
```

### 6.1 Recursive implementation

For an input vector:

```text
[x1, x2, ..., xn, y]
```

the implementation separates the final value `y` from the fixed arguments.

If `y == 0`:

```text
return base([x1, ..., xn])
```

If `y > 0`, it:

1. Calls itself with `y - 1`.
2. Stores that result as `previous`.
3. Builds `[x1, ..., xn, y - 1, previous]`.
4. Evaluates the step function with that vector.

The recursive call is made through `Evaluate`, not directly through
`Compute`. Consequently, each recursive level is counted as a normal
function evaluation.

This is a literal recursive implementation. A very large recursion argument
can exhaust the C++ call stack.

## 7. Construction of derived functions

The factory functions are implemented in
[`derived_functions.cc`](src/derived_functions.cc). They return
`std::shared_ptr<const Function>`, allowing a constructed function to be
embedded in other functions without manual memory management.

### 7.1 Sum

`MakeSum()` creates:

```text
base(x)       = projection_1(x)
step(x,y,r)   = successor(projection_3(x,y,r))
```

Therefore:

```text
sum(x, 0)   = x
sum(x, y+1) = successor(sum(x, y))
```

Example:

```text
sum(3, 2)
= successor(sum(3, 1))
= successor(successor(sum(3, 0)))
= successor(successor(3))
= 5
```

### 7.2 Product

`MakeProduct()` reuses a sum function:

```text
base(x)       = zero(x)
step(x,y,r)   = sum(projection_3(x,y,r), projection_1(x,y,r))
```

Therefore:

```text
product(x, 0)   = 0
product(x, y+1) = sum(product(x, y), x)
```

Example:

```text
product(3, 2)
= sum(product(3, 1), 3)
= sum(sum(product(3, 0), 3), 3)
= sum(sum(0, 3), 3)
= 6
```

### 7.3 Power

`MakePower()` reuses a product function:

```text
base(x)       = successor(zero(x))
step(x,y,r)   = product(projection_3(x,y,r), projection_1(x,y,r))
```

The base expression is always one:

```text
successor(zero(x)) = 1
```

Therefore:

```text
power(x, 0)   = 1
power(x, y+1) = product(power(x, y), x)
```

Example:

```text
power(2, 3)
= product(power(2, 2), 2)
= product(product(power(2, 1), 2), 2)
= product(product(product(power(2, 0), 2), 2), 2)
= 8
```

This definition intentionally gives:

```text
power(0, 0) = 1
```

because the base case of power is one.

## 8. Execution pipeline

The complete runtime pipeline starts in
[`main.cc`](src/main.cc).

### Step 1: Process the command line

`main` calls `ParseArguments` from [`tools.cc`](src/tools.cc).

The parser recognizes:

```text
-x VALUE
-y VALUE
-h
--help
```

It rejects:

- Missing `-x` or `-y`.
- Repeated options.
- Unknown options.
- Missing option values.
- Negative values.
- Decimal values.
- Alphabetic or otherwise non-numeric values.
- Values outside the `Natural` range.

### Step 2: Handle help

If `-h` or `--help` was supplied, `main` calls `PrintUsage`, prints the
available syntax, and exits successfully without constructing a power
function.

### Step 3: Construct the function tree

For a normal computation, `main` calls `RunPower(x, y)`.

`RunPower`:

1. Calls `MakePower`.
2. Creates a fresh `CallCounter`.
3. Evaluates the power function with `[x, y]`.
4. Returns both the result and the call count.

`MakePower` internally constructs product, and product internally constructs
sum. The tree is therefore assembled before the first evaluation begins.

### Step 4: Evaluate the top-level power function

The first call is conceptually:

```text
Power.Evaluate([x, y], counter)
```

`Evaluate` checks that two arguments were supplied, increments the counter,
and invokes `PrimitiveRecursion::Compute`.

### Step 5: Resolve the recursion

The power recursion repeatedly evaluates lower exponents:

```text
power(x, y)
power(x, y - 1)
power(x, y - 2)
...
power(x, 0)
```

At `power(x, 0)`, the base composition is evaluated:

```text
successor(zero(x))
```

This returns one.

### Step 6: Unwind the recursion

After the base result is available, each recursive level evaluates its step
function:

```text
product(previous_result, x)
```

Each product recursively evaluates sums, and each sum recursively evaluates
successor operations.

The result is propagated back through all recursive levels until the original
power call receives its final value.

### Step 7: Print the result

`RunPower` returns a `PowerResult` containing:

```text
value
calls
```

`PrintResult` prints both fields:

```text
Result: 32
Function calls: <count>
```

### Step 8: Handle errors

`main` maps exceptions to exit codes:

| Exception | Meaning | Exit code |
| --- | --- | --- |
| `std::invalid_argument` | Invalid command-line input or invalid function construction | `1` |
| `std::overflow_error` | Successor would exceed the natural-number range | `2` |
| Other `std::exception` | Unexpected standard exception | `3` |

## 9. Execution example: `power(2, 2)`

At a high level, the calculation is:

```text
power(2, 2)
= product(power(2, 1), 2)
= product(product(power(2, 0), 2), 2)
= product(product(1, 2), 2)
= product(2, 2)
= 4
```

The actual implementation expands every operation:

1. `Power` evaluates its recursive structure.
2. Its step is a `Composition`.
3. That composition evaluates projections for the previous result and base.
4. It calls `Product`.
5. `Product` recursively evaluates its own structure.
6. Its step calls `Sum`.
7. `Sum` recursively evaluates successor operations.

This explains why the number of function calls is much larger than the number
of visible arithmetic operations.

## 10. Runtime class tree

The class inheritance tree is intentionally small:

```text
Function (abstract)
├── Zero
├── Successor
├── Projection
├── Composition
└── PrimitiveRecursion
```

`CallCounter` is not a subclass of `Function`. It is a collaborator passed to
each evaluation.

### 10.1 The constructed object tree for power

The `MakePower()` factory produces a composed object graph like this:

```text
PrimitiveRecursion                    Power
├── base: Composition                  Constant one
│   ├── outer: Successor
│   └── inner[0]: Zero(arity 1)
└── step: Composition                  Power step
    ├── outer: PrimitiveRecursion      Product
    │   ├── base: Zero(arity 1)
    │   └── step: Composition          Product step
    │       ├── outer: PrimitiveRecursion  Sum
    │       │   ├── base: Projection(1, 1)
    │       │   └── step: Composition
    │       │       ├── outer: Successor
    │       │       └── inner[0]: Projection(3, 3)
    │       └── inner:
    │           ├── Projection(3, 3)
    │           └── Projection(1, 3)
    └── inner:
        ├── Projection(3, 3)
        └── Projection(1, 3)
```

The exact C++ type of both `Power` and `Product` is
`PrimitiveRecursion`; their mathematical role is determined by the functions
passed to their constructors.

### 10.2 Relationship between the class tree and object tree

The **class tree** describes inheritance: all function types derive from
`Function`.

The **object tree** describes composition: a `Composition` or
`PrimitiveRecursion` owns shared pointers to other `Function` objects.

These are different structures:

```text
Inheritance:
Function
  ├── Composition
  └── PrimitiveRecursion

Composition:
Power
  └── Product
      └── Sum
```

This separation is what allows the program to treat every component through
the same `Function` interface while still assembling complex behavior.

## 11. Memory ownership

Nested functions use:

```cpp
std::shared_ptr<const Function>
```

The `const` qualifier prevents a composed function from modifying its child
functions during evaluation. `shared_ptr` allows a function to be safely held
by its parent and automatically destroyed when no longer referenced.

No manual `new` or `delete` operations are required.

## 12. Performance and limitations

The implementation is intentionally faithful to the primitive-recursive
definition rather than optimized arithmetic.

### Time cost

Addition is repeated successor application. Multiplication is repeated
addition. Power is repeated multiplication. Consequently, the amount of work
can grow very quickly.

### Stack depth

Primitive recursion uses actual C++ recursion. A very large recursion argument
may cause stack exhaustion.

### Numeric overflow

The first operation that exceeds `Natural` is normally a successor operation.
`Successor` detects this and throws `std::overflow_error`.

### Zero exponent

Every base raised to zero returns one, including:

```text
0^0 = 1
```

This is the convention selected by the primitive-recursive definition used in
this project.
