# Tuple Arithmetic

This header-only library provides additional functionality to tuple-like objects. The header `tuple_arithmetic.hpp` provides arithmetic operators and some others. `tuple_math.hpp` provides overloaded `cmath` functions and optional comparison operators. The library uses C++20 features.

## Macros
By default all functions are provided in namespace `tpa`, but this can be changed by defining `TP_NAMESPACE` to specify the namespace name, or by defining `TP_NO_NAMESPACE` to not use a namespace. Since the comparison operators will change standard behavior, this operators must be explicitly imported via `using operator#`, or use the macro `TP_USE_OPS`. Alternatively you can also defile `TP_DEFAULT_USE_OPS` before including the header to use them by default.

For all the functions and operators, if the return value is a tuple with all elements having the same non-reference type, the returned tuple will be automatically converted to `std::array`. This behavior can be disabled by defining `TP_DONOT_CONVERT` so that all functions will return a `std::tuple`.

## Broadcasting scalars
For binary and ternary operators, non-tuple operants will be automatically broadcast into a tuple-like object (`tpa::const_tuple`). Note that tuples with different size will cause deduction failure instead of a broadcast. To manually broadcast an object, use `tpa::repeat_as(src, tuple_like_obj)`.

## Unary operators
- `tpa::apply_unary_op(Op&& op, tuple_like&& tp)`: `(a1, a2, ...)` -> `(op(a1), op(a2), ...)`
- `tpa::foreach(tuple_like&& tp, Op&& op)`: `(a1, a2, ...)`: dose the same thing as `tpa::apply_unary_op`, but use `std::forward_as_tuple` instead of `std::make_tuple` to create return value.
- `operator-`: negative operator.
- `tpa::cast<TYPE>(tuple_like&&)`: cast all element of tuple\_like into TYPE. Example:
```cpp
auto a = std::make_tuple(1, 2.0);
auto b = -a;  // std::tuple<int, double>{ -1, -2.0 }
auto c = tpa::cast<int>(a);  // std::array<int, 2>{ 1, 2 }
```
- `tpa::vget<size_t idx>(tuple_like&&)`: apply `get<idx>` to each element. Example:
```cpp
auto a = tpa::vget<1>(std::tuple{std::array{1, 2}, std::tuple{1.0, 2.0}});  // std::tuple<int, double>{ 2, 2.0 }
```
- `tpa::firstN<size_t N>(tuple_like&&)`: select the first N elements. Example:
```cpp
auto a = std::make_tuple(1, 2, 3.0);
auto b = tpa::firstN<2>(a);  // std::array<int, 2>{ 1, 2 }
```

## Binary operators
- `tpa::apply_binary_op(Op&& op, T1&& tp1, T2&& tp2)`: either `T1` or `T2` must be tuple-like.
- Arithmetic operators, including "+", "-", "\*", and "/". Example:
```cpp
auto a = std::make_tuple(1, 2.0),
     b = std::array<float, 2>(3.0, 4.0);
auto c = a + b;  // std::tuple<float, double>{ 4.0, 6.0 }
```
- `tpa::index(tuple_like&&, index)`, `tpa::invoke(tuple_like&&, args)`: broadcast `index`/`args` to tuple-like, than call `operator[]`/`operator()` for each operant pairs. Example:
```cpp
auto a = std::make_tuple( std::array{1, 2}, std::array{3, 4} );
auto b = tpa::index(a, make_tuple(0, 1))/  // std::array<int, 2>{ 0, 4 };

auto c = std::make_tuple( [](int i){ return i; }, [](int i){ return i + 1; } );
auto d = tpa::invoke(c, 2);  // std::array<int, 2>{ 2, 3 };
```
- `tpa::dot(tuple_like, tuple_like)`: dot production.
- `tpa::cross(tuple_like, tuple_like)`: cross production. Operants must have same size of 2 or 3. Example:
```cpp
auto a = std::make_tuple(1, 2.0), b = std::array{3, 4};
auto c = tpa::dot(a, b);  // double 11.0
auto d = tpa::cross(a, b);  // double -2.0

auto e = tpa::cross(make_tuple(1, 2, 3.0), make_tuple(2, 3, 4.0));
// std::tuple<double, double, int>{ -1.0, 2.0, -1 }
```

## Ternary operators
- `tpa::apply_ternary_op(Op&& op, T1&& tp1, T2&& tp2, T3&& tp3)`: either `T1`, `T2`, or `T3` must be tuple-like.
`tpa::select(tuple_like cond, if_true, if_false)`: works like `cond ? if_true : if_false`. The last two operants will be broadcast. Example:
```cpp
auto a = tpa::select(array{0, 1}, 0.0, array{1, 2});  // array<double, 2>{ 1.0, 0.0 }
```

## Reduce operators
- `tpa::reduce(Op&& op, Tp&& tp)`: requires `std::tuple_size(Tp) > 0`. `(a1, a2, ...)` -> `op(a1, op(a2, ...))`, or `(a1,)` -> `(a1,)`.
- `tpa::sum`, `tpa::prod`, `tpa::any`, `tpa::all`, `tpa::reduce_min`, `tpa::reduce_max`. `reduce_min` and `reduce_max` use conditional operator `?:`.
```cpp
auto a = std::make_tuple(1, 2.0, 3, 4);
auto b = tpa::sum(a);  // double 10.0
```
