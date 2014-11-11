/*
Copyright (c) 2013 Andrey Goryachev <andrey.goryachev@gmail.com>
Copyright (c) 2011-2013 Other contributors as noted in the AUTHORS file.

This file is part of Kora.

Kora is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Kora is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KORA_UTILITY_UNDERLYING_TYPE_HPP
#define KORA_UTILITY_UNDERLYING_TYPE_HPP

#include "kora/utility/lazy_false.hpp"
#include "kora/utility/platform.hpp"

#include <cstdint>
#include <type_traits>

/*! \file underlying_type.hpp
 *
 * Provides limited emulation of std::underlying_type.
 */

namespace kora {

namespace detail {

template<class T, class = void>
struct underlying_impl;

// Use general types when possible.
template<class T>
struct underlying_impl<
    T,
    typename std::enable_if<std::is_signed<T>::value && (sizeof(T) <= sizeof(int))>::type
>
{
    typedef int type;
};

template<class T>
struct underlying_impl<
    T,
    typename std::enable_if<std::is_unsigned<T>::value && (sizeof(T) <= sizeof(unsigned int))>::type
>
{
    typedef unsigned int type;
};

// Explicitly support 64-bit integers. Especially for x86.
template<class T>
struct underlying_impl<
    T,
    typename std::enable_if<std::is_signed<T>::value && (sizeof(T) > sizeof(int) && sizeof(T) <= sizeof(int64_t))>::type
>
{
    typedef int64_t type;
};

template<class T>
struct underlying_impl<
    T,
    typename std::enable_if<std::is_unsigned<T>::value && (sizeof(T) > sizeof(unsigned int) && sizeof(T) <= sizeof(uint64_t))>::type
>
{
    typedef uint64_t type;
};

} // namespace detail


#ifdef KORA_DOXYGEN

/*! Provides integer type which can store any value of the enumeration type.
 * It may be undefined for enumerations bigger than 64 bits.
 * \warning It doesn't work on GCC 4.4.
 *
 * \tparam Enum The enumeration type.
 */
template<class Enum>
struct underlying_type {
    //! The underlying integer type.
    typedef unspecified_integer_type type;
};

#else // KORA_DOXYGEN

template<class T, class = void>
struct underlying_type;

template<class Enum>
struct underlying_type<Enum, typename std::enable_if<std::is_enum<Enum>::value>::type> {
#if !defined(KORA_NOT_BAD)
    static_assert(lazy_false<Enum>::value, "Unsupported on GCC 4.4. Suffer.");
#elif defined(KORA_HAVE_GCC46) && !defined(KORA_HAVE_GCC47)
    typedef typename detail::underlying_impl<Enum>::type type;
#else
    typedef typename std::underlying_type<Enum>::type type;
#endif
};

#endif // KORA_DOXYGEN

/*! Converts enum value to its underlying type.
 * \tparam Enum The enumeration type.
 * \param value The enum value.
 * \returns Result of the conversion.
 */
template<class Enum>
typename underlying_type<Enum>::type
to_underlying_type(Enum value) {
    return static_cast<typename underlying_type<Enum>::type>(value);
}

} // namespace kora

#endif
