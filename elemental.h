//  :copyright: (c) 1999-2001 David Abrahams, Jeremy Siek, Daryle Walker.
//  :copyright: (c) 2002-2017 Daniel Frey.
//  :copyright: (c) 2017-2018 Alex Huszagh.
//  :license: Boost, see licenses/boost.md for more details.
/**
 *  \addtogroup PyCPP
 *  \brief Lightweight, full-featured port of Boost operators.
 *
 *  Many "features", such as base chaining to avoid object bloat,
 *  are no longer relevant for modern compilers, which require
 *  empty-base-class optimization. In addition, partial specialization
 *  dramatically simplifies the implementation, and removes namespace
 *  pollution (all exported names do not have 1 or 2 suffixes, and
 *  no macro hacks are required to make them behave).
 */

#pragma once

#include <pycpp/config.h>
#include <pycpp/stl/cstddef.h>
#include <pycpp/stl/iterator.h>
#include <pycpp/stl/memory.h>

PYCPP_BEGIN_NAMESPACE

// OBJECTS
// -------

// LESS THAN COMPARABLE

template <typename T, typename U = T>
struct less_than_comparable
{
    friend bool operator<=(const T& x, const U& y) { return !static_cast<bool>(x > y); }
    friend bool operator>=(const T& x, const U& y) { return !static_cast<bool>(x < y); }
    friend bool operator>(const U& x, const T& y)  { return y < x; }
    friend bool operator<(const U& x, const T& y)  { return y > x; }
    friend bool operator<=(const U& x, const T& y) { return !static_cast<bool>(y < x); }
    friend bool operator>=(const U& x, const T& y) { return !static_cast<bool>(y > x); }
};

template <typename T>
struct less_than_comparable<T, T>
{
    friend bool operator>(const T& x, const T& y)  { return y < x; }
    friend bool operator<=(const T& x, const T& y) { return !static_cast<bool>(y < x); }
    friend bool operator>=(const T& x, const T& y) { return !static_cast<bool>(x < y); }
};

// EQUALITY COMPARABLE

template <typename T, typename U = T>
struct equality_comparable
{
    friend bool operator==(const U& y, const T& x) { return x == y; }
    friend bool operator!=(const U& y, const T& x) { return !static_cast<bool>(x == y); }
    friend bool operator!=(const T& y, const U& x) { return !static_cast<bool>(y == x); }
};

template <typename T>
struct equality_comparable<T, T>
{
     friend bool operator!=(const T& x, const T& y) { return !static_cast<bool>(x == y); }
};

// BINARY OPERATIONS

// This is the optimal implementation for ISO/ANSI C++,
// but it requires the compiler to implement the NRVO.
// If the compiler has no NRVO, this is the best symmetric
// implementation available.

// **WARNING**
// Instead of append `2_left` to the operator name for reverse operations,
// PyCPP changes the Boost naming convention to prefix `r` to the operation,
// similar to `__radd__` for reverse `__add__`.

#define PYCPP_BINARY_OPERATOR_COMMUTATIVE(name, op)                     \
    template <typename T, typename U = T>                               \
    struct name                                                         \
    {                                                                   \
        friend T operator op(const T& x, const U& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
                                                                        \
        friend T operator op(const U& x, const T& y)                    \
        {                                                               \
            T copy(y);                                                  \
            copy op##= x;                                               \
            return copy;                                                \
        }                                                               \
    };                                                                  \
                                                                        \
    template <typename T>                                               \
    struct name<T, T>                                                   \
    {                                                                   \
        friend T operator op(const T& x, const T& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
    }


#define PYCPP_BINARY_OPERATOR_NON_COMMUTATIVE(name, op)                 \
    template <typename T, typename U = T>                               \
    struct name                                                         \
    {                                                                   \
        friend T operator op(const T& x, const U& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
    };                                                                  \
                                                                        \
    template <typename T>                                               \
    struct name<T, T>                                                   \
    {                                                                   \
        friend T operator op(const T& x, const T& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
    };                                                                  \
                                                                        \
    template <typename T, typename U>                                   \
    struct r##name                                                      \
    {                                                                   \
        friend T operator op(const U& x, const T& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
    }

PYCPP_BINARY_OPERATOR_COMMUTATIVE(multipliable, *);
PYCPP_BINARY_OPERATOR_COMMUTATIVE(addable, +);
PYCPP_BINARY_OPERATOR_NON_COMMUTATIVE(subtractable, -);
PYCPP_BINARY_OPERATOR_NON_COMMUTATIVE(dividable, /);
PYCPP_BINARY_OPERATOR_NON_COMMUTATIVE(modable, %);
PYCPP_BINARY_OPERATOR_COMMUTATIVE(xorable, ^);
PYCPP_BINARY_OPERATOR_COMMUTATIVE(andable, &);
PYCPP_BINARY_OPERATOR_COMMUTATIVE(orable, |);

#define PYCPP_BINARY_OPERATOR(name, op)                                 \
    template <typename T, typename U = T>                               \
    struct name                                                         \
    {                                                                   \
        friend T operator op(const T& x, const U& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
    };                                                                  \
                                                                        \
    template <typename T>                                               \
    struct name<T, T>                                                   \
    {                                                                   \
        friend T operator op(const T& x, const T& y)                    \
        {                                                               \
            T copy(x);                                                  \
            copy op##= y;                                               \
            return copy;                                                \
        }                                                               \
    }

PYCPP_BINARY_OPERATOR(left_shiftable, <<);
PYCPP_BINARY_OPERATOR(right_shiftable, >>);

// CLEANUP
// -------

#undef PYCPP_BINARY_OPERATOR_COMMUTATIVE
#undef PYCPP_BINARY_OPERATOR_NON_COMMUTATIVE
#undef PYCPP_BINARY_OPERATOR

// OTHER OPERATORS
// ---------------

template <typename T>
struct incrementable
{
    friend T operator++(T& x, int)
    {
        T copy(x);
        ++x;
        return copy;
    }
};

template <typename T>
struct decrementable
{
    friend T operator--(T& x, int)
    {
        T copy(x);
        --x;
        return copy;
    }
};

template <typename T, typename P>
struct dereferenceable
{
    P operator->() const
    {
        return PYCPP_NAMESPACE::addressof(*static_cast<const T&>(*this));
    }
};

template <typename T, typename I, typename R>
struct subscriptable
{
    R operator[](I n) const
    {
        return *(static_cast<const T&>(*this) + n);
    }
};

// ORDERING
// --------

template <typename T, typename U = T>
struct equivalent
{
    friend bool operator==(const T& x, const U& y)
    {
        return !static_cast<bool>(x < y) && !static_cast<bool>(x > y);
    }
};

template <typename T>
struct equivalent<T, T>
{
    friend bool operator==(const T&x, const T&y)
    {
        return !static_cast<bool>(x < y) && !static_cast<bool>(y < x);
    }
};

template <typename T, typename U = T>
struct partially_ordered
{
    friend bool operator<=(const T& x, const U& y)
    {
        return static_cast<bool>(x < y) || static_cast<bool>(x == y);
    }

    friend bool operator>=(const T& x, const U& y)
    {
        return static_cast<bool>(x > y) || static_cast<bool>(x == y);
    }

    friend bool operator>(const U& x, const T& y)
    {
        return y < x;
    }

    friend bool operator<(const U& x, const T& y)
    {
        return y > x;
    }

    friend bool operator<=(const U& x, const T& y)
    {
        return static_cast<bool>(y > x) || static_cast<bool>(y == x);
    }

    friend bool operator>=(const U& x, const T& y)
    {
        return static_cast<bool>(y < x) || static_cast<bool>(y == x);
    }
};

template <typename T>
struct partially_ordered<T, T>
{
    friend bool operator>(const T& x, const T& y)
    {
        return y < x;
    }

    friend bool operator<=(const T& x, const T& y)
    {
        return static_cast<bool>(x < y) || static_cast<bool>(x == y);
    }

    friend bool operator>=(const T& x, const T& y)
    {
        return static_cast<bool>(y < x) || static_cast<bool>(x == y);
    }
};

template <typename T, typename U = T>
struct totally_ordered:
    less_than_comparable<T, U>,
    equality_comparable<T, U>
{};

template <typename T>
struct totally_ordered<T, T>:
    less_than_comparable<T>,
    equality_comparable<T>
{};

template <typename T, typename U = T>
struct additive:
    addable<T, U>,
    subtractable<T, U>
{};

template <typename T>
struct additive<T, T>:
    addable<T>,
    subtractable<T>
{};

template <typename T, typename U = T>
struct multiplicative:
    multipliable<T, U>,
    dividable<T, U>
{};

template <typename T>
struct multiplicative<T, T>:
    multipliable<T>,
    dividable<T>
{};

template <typename T, typename U = T>
struct integer_multiplicative:
    multiplicative<T, U>,
    modable<T, U>
{};

template <typename T>
struct integer_multiplicative<T, T>:
    multiplicative<T>,
    modable<T>
{};

template <typename T, typename U = T>
struct arithmetic:
    additive<T, U>,
    multiplicative<T, U>
{};

template <typename T>
struct arithmetic<T, T>:
    additive<T>,
    multiplicative<T>
{};

template <typename T, typename U = T>
struct integer_arithmetic:
    additive<T, U>,
    integer_multiplicative<T, U>
{};

template <typename T>
struct integer_arithmetic<T, T>:
    additive<T>,
    integer_multiplicative<T>
{};

template <typename T, typename U = T>
struct bitwise:
    xorable<T, U>,
    andable<T, U>,
    orable<T, U>
{};

template <typename T>
struct bitwise<T, T>:
    xorable<T>,
    andable<T>,
    orable<T>
{};

template <typename T>
struct unit_steppable:
    incrementable<T>,
    decrementable<T>
{};

template <typename T, typename U = T>
struct shiftable:
    left_shiftable<T, U>,
    right_shiftable<T, U>
{};

template <typename T>
struct shiftable<T, T>:
    left_shiftable<T>,
    right_shiftable<T>
{};

template <typename T, typename U = T>
struct ring_operators:
    additive<T, U>,
    rsubtractable<T, U>,
    multipliable<T, U>
{};

template <typename T>
struct ring_operators<T, T>:
    additive<T>,
    multipliable<T>
{};

template <typename T, typename U = T>
struct ordered_ring_operators:
    ring_operators<T, U>,
    totally_ordered<T, U>
{};

template <typename T>
struct ordered_ring_operators<T, T>:
    ring_operators<T>,
    totally_ordered<T>
{};

template <typename T, typename U = T>
struct field_operators:
    ring_operators<T, U>,
    dividable<T, U>,
    rdividable<T, U>
{};

template <typename T>
struct field_operators<T, T>:
    ring_operators<T>,
    dividable<T>
{};

template <typename T, typename U = T>
struct ordered_field_operators:
    field_operators<T, U>,
    totally_ordered<T, U>
{};

template <typename T>
struct ordered_field_operators<T, T>:
    field_operators<T>,
    totally_ordered<T>
{};

template <typename T, typename U = T>
struct euclidian_ring_operators:
    ring_operators<T, U>,
    dividable<T, U>,
    rdividable<T, U>,
    modable<T, U>,
    rmodable<T, U>
{};

template <typename T>
struct euclidian_ring_operators<T, T>:
    ring_operators<T>,
    dividable<T>,
    modable<T>
{};

template <typename T, typename U = T>
struct ordered_euclidian_ring_operators:
    totally_ordered<T, U>,
    euclidian_ring_operators<T, U>
{};

template <typename T>
struct ordered_euclidian_ring_operators<T, T>:
    totally_ordered<T>,
    euclidian_ring_operators<T>
{};

template <typename T, typename U = T>
struct euclidean_ring_operators:
    ring_operators<T, U>,
    dividable<T, U>,
    rdividable<T, U>,
    modable<T, U>,
    rmodable<T, U>
{};

template <typename T>
struct euclidean_ring_operators<T, T>:
    ring_operators<T>,
    dividable<T>,
    modable<T>
{};

template <typename T, typename U = T>
struct ordered_euclidean_ring_operators:
    totally_ordered<T, U>,
    euclidean_ring_operators<T, U>
{};

template <typename T>
struct ordered_euclidean_ring_operators<T, T>:
    totally_ordered<T>,
    euclidean_ring_operators<T>
{};

template <typename T, typename P>
struct input_iteratable:
    equality_comparable<T>,
    incrementable<T>,
    dereferenceable<T, P>
{};

template <typename T>
struct output_iteratable:
    incrementable<T>
{};

template <typename T, typename P>
struct forward_iteratable:
    input_iteratable<T, P>
{};

template <typename T, typename P>
struct bidirectional_iteratable:
    forward_iteratable<T, P>,
    decrementable<T>
{};

//  To avoid repeated derivation from equality_comparable,
//  which is an indirect base class of bidirectional_iterable,
//  random_access_iteratable must not be derived from totally_ordered
//  but from less_than_comparable only. (Helmut Zeisel, 02-Dec-2001)
template <typename T, typename P, typename D, typename R>
struct random_access_iteratable:
    bidirectional_iteratable<T, P>,
    less_than_comparable<T>,
    additive<T, D>,
    subscriptable<T, D, R>
{};

template <typename T, typename U = T>
struct operators:
    totally_ordered<T, U>,
    integer_arithmetic<T, U>,
    bitwise<T, U>
{};

template <typename T>
struct operators<T, T>:
    totally_ordered<T>,
    integer_arithmetic<T>,
    bitwise<T>,
    unit_steppable<T>
{};

// Iterators

//  Iterator helper classes (contributed by Jeremy Siek) -------------------//
//  (Input and output iterator helpers contributed by Daryle Walker) -------//
//  (Changed to use combined operator classes by Daryle Walker) ------------//
//  (Adapted to C++17 by Daniel Frey) --------------------------------------//
template <
    typename Category,
    typename T,
    typename Distance = ptrdiff_t,
    typename Pointer = T*,
    typename Reference = T&
>
struct iterator_helper
{
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

template <
    typename T,
    typename V,
    typename D = ptrdiff_t,
    typename P = const V*,
    typename R = const V&
>
struct input_iterator_helper:
    input_iteratable<T, P>,
    iterator_helper<input_iterator_tag, V, D, P, R>
{};

template <typename T>
struct output_iterator_helper:
    output_iteratable<T>,
    iterator_helper<output_iterator_tag, void, void, void, void>
{
    T& operator*()
    {
        return static_cast<T&>(*this);
    }

    T& operator++()
    {
        return static_cast<T&>(*this);
    }
};

template <
    typename T,
    typename V,
    typename D = ptrdiff_t,
    typename P = V*,
    typename R = V&
>
struct forward_iterator_helper:
    forward_iteratable<T, P>,
    iterator_helper<forward_iterator_tag, V, D, P, R>
{};

template <
    typename T,
    typename V,
    typename D = ptrdiff_t,
    typename P = V*,
    typename R = V&
>
struct bidirectional_iterator_helper:
    bidirectional_iteratable<T, P>,
    iterator_helper<bidirectional_iterator_tag, V, D, P, R>
{};

template <
    typename T,
    typename V,
    typename D = ptrdiff_t,
    typename P = V*,
    typename R = V&
>
struct random_access_iterator_helper:
    random_access_iteratable<T, P, D, R>,
    iterator_helper<random_access_iterator_tag, V, D, P, R>
{};

PYCPP_END_NAMESPACE
