/*
    Copyright (c) 2013-2014 Andrey Goryachev <andrey.goryachev@gmail.com>
    Copyright (c) 2011-2014 Other contributors as noted in the AUTHORS file.

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

#ifndef KORA_DYNAMIC_CONSTRUCTORS_HPP
#define KORA_DYNAMIC_CONSTRUCTORS_HPP

#include "kora/dynamic/error.hpp"
#include "kora/utility.hpp"

KORA_PUSH_VISIBLE
#include <boost/numeric/conversion/cast.hpp>
KORA_POP_VISIBILITY

#include <tuple>
#include <type_traits>
#include <utility>
#include <unordered_map>

namespace kora { namespace dynamic {

namespace detail {

// Helper traits.
template<class T>
struct match_uint_t :
    std::integral_constant<bool,
                           std::is_integral<T>::value &&
                           std::is_unsigned<T>::value &&
                           sizeof(T) <= sizeof(dynamic_t::uint_t)>
{ };

template<class T>
struct match_int_t :
    std::integral_constant<bool,
                           std::is_integral<T>::value &&
                           std::is_signed<T>::value &&
                           sizeof(T) <= sizeof(dynamic_t::int_t)>
{ };

template<class T>
struct match_double_t :
    std::integral_constant<bool,
                           std::is_floating_point<T>::value &&
                           sizeof(T) <= sizeof(dynamic_t::double_t)>
{ };

} // namespace detail

/*!
 * \brief Converts unsigned integer types to dynamic_t.
 *
 * Enabled only for integer types which can be converted to dynamic_t::uint_t without data loss.
 */
#ifdef KORA_DOXYGEN
template<>
struct constructor<SignedInteger>
#else
template<class SignedInteger>
struct constructor<
   SignedInteger,
   typename std::enable_if<detail::match_uint_t<SignedInteger>::value>::type
>
#endif
{
    static const bool enable = true;

    //! \post <tt>to.is_uint() == true && to.as_uint() == static_cast<dynamic_t::uint_t>(from)</tt>
    static inline
    void
    convert(SignedInteger from, dynamic_t& to) KORA_NOEXCEPT {
        to = static_cast<dynamic_t::uint_t>(from);
    }
};

/*!
 * \brief Converts signed integer types to dynamic_t.
 *
 * Enabled only for integer types which can be converted to dynamic_t::int_t without data loss.
 */
#ifdef KORA_DOXYGEN
template<>
struct constructor<UnsignedInteger>
#else
template<class UnsignedInteger>
struct constructor<
    UnsignedInteger,
    typename std::enable_if<detail::match_int_t<UnsignedInteger>::value>::type
>
#endif
{
    static const bool enable = true;

    //! \post <tt>to.is_int() == true && to.as_int() == static_cast<dynamic_t::int_t>(from)</tt>
    static inline
    void
    convert(UnsignedInteger from, dynamic_t& to) KORA_NOEXCEPT {
        to = static_cast<dynamic_t::int_t>(from);

    }
};

/*!
 * \brief Converts floating point types to dynamic_t.
 *
 * Enabled only for types which can be converted to dynamic_t::double_t without data loss.
 */
#ifdef KORA_DOXYGEN
template<>
struct constructor<FloatingPoint>
#else
template<class FloatingPoint>
struct constructor<
    FloatingPoint,
    typename std::enable_if<detail::match_double_t<FloatingPoint>::value>::type
>
#endif // KORA_DOXYGEN
{
    static const bool enable = true;

    //! \post <tt>to.is_double() == true && to.as_double() == static_cast<dynamic_t::double_t>(from)</tt>
    static inline
    void
    convert(FloatingPoint from, dynamic_t& to) KORA_NOEXCEPT {
        to = static_cast<dynamic_t::double_t>(from);
    }
};

//! \brief Converts string literals to dynamic_t.
template<size_t N>
struct constructor<char[N]> {
    static const bool enable = true;

    //! \post <tt>to.is_string() == true && to.as_string() == dynamic_t::string_t(from)</tt>
    //! \throws std::bad_alloc
    static inline
    void
    convert(const char* from, dynamic_t& to) {
        dynamic_t::string_t buffer(from, N - 1);
        to = std::move(buffer);
    }
};

//! \brief Converts C-strings to dynamic_t.
template<>
struct constructor<const char*> {
    static const bool enable = true;

    //! \post <tt>to.is_string() == true && to.as_string() == dynamic_t::string_t(from)</tt>
    //! \throws std::bad_alloc
    static inline
    void
    convert(const char* from, dynamic_t& to) {
        dynamic_t::string_t buffer(from);
        to = std::move(buffer);
    }
};

//! \brief Converts std::vector to dynamic_t.
template<class T>
struct constructor<std::vector<T>> {
    static const bool enable = true;

    //! \post <tt>to.is_array() == true && to.as_array().size() == from.size()</tt>
    //! \post For all <tt>size_t i; i < from.size() ==> to.as_array()[i] == dynamic_t(from[i])</tt>
    //! \throws std::bad_alloc
    //! \throws Any exceptions thrown by <tt>dynamic_t(std::declval<T>())</tt>
    static inline
    void
    convert(const std::vector<T>& from, dynamic_t& to) {
        dynamic_t::array_t buffer;
        buffer.reserve(from.size());

        for (size_t i = 0; i < from.size(); ++i) {
            buffer.emplace_back(from[i]);
        }

        to = std::move(buffer);
    }
};

//! \brief Converts std::tuple to dynamic_t.
template<class... Args>
struct constructor<std::tuple<Args...>> {
    static const bool enable = true;

    //! \post <tt>to.is_array() == true && to.as_array().size() == sizeof...(Args)</tt>
    //! \post For all <tt>size_t i; i < sizeof...(Args) ==> to.as_array()[i] == dynamic_t(std::get<i>(from))</tt>
    //! \throws std::bad_alloc
    //! \throws Any exceptions thrown by <tt>dynamic_t(std::declval<Args>())...</tt>
    static inline
    void
    convert(const std::tuple<Args...>& from, dynamic_t& to) {
        dynamic_t::array_t buffer;
        buffer.reserve(sizeof...(Args));

        copy_tuple_to_vector<sizeof...(Args), 1, Args...>::convert(from, buffer);

        to = std::move(buffer);
    }

private:
    template<size_t N, size_t I, class... Args2>
    struct copy_tuple_to_vector {
        static inline
        void
        convert(const std::tuple<Args2...>& from, dynamic_t::array_t& to) {
            to.emplace_back(std::get<I - 1>(from));
            copy_tuple_to_vector<N, I + 1, Args2...>::convert(from, to);
        }
    };

    template<size_t N, class... Args2>
    struct copy_tuple_to_vector<N, N, Args2...> {
        static inline
        void
        convert(const std::tuple<Args2...>& from, dynamic_t::array_t& to) {
            to.emplace_back(std::get<N - 1>(from));
        }
    };

    template<class... Args2>
    struct copy_tuple_to_vector<0, 1, Args2...> {
        static inline
        void
        convert(const std::tuple<Args2...>&, dynamic_t::array_t&) {
            // Empty.
        }
    };
};

//! \brief Converts std::map<dynamic_t::string_t, dynamic_t> to dynamic_t.
template<>
struct constructor<std::map<dynamic_t::string_t, dynamic_t>> {
    static const bool enable = true;

    //! \post <tt>to.is_object() == true && to.as_object().size() == from.size()</tt>
    //! \post For all <tt>std::string key; from.count(key) == to.as_object().count(key)</tt>
    //! \post For all <tt>std::string key; from.count(key) > 0 ==> to.as_object()[key] == from[key]</tt>
    //! \throws std::bad_alloc
    //! \throws Any exceptions thrown by dynamic_t copy constructor.
    template<class Object>
    static inline
    void
    convert(Object&& from, dynamic_t& to) {
        to = dynamic_t::object_t(std::forward<Object>(from));
    }
};

//! \brief Converts std::map<std::string, T> to dynamic_t.
template<class T>
struct constructor<std::map<std::string, T>> {
    static const bool enable = true;

    //! \post <tt>to.is_object() == true && to.as_object().size() == from.size()</tt>
    //! \post For all <tt>std::string key; from.count(key) == to.as_object().count(key)</tt>
    //! \post For all <tt>std::string key; from.count(key) > 0 ==> to.as_object()[key] == dynamic_t(from[key])</tt>
    //! \throws std::bad_alloc
    //! \throws Any exceptions thrown by <tt>dynamic_t(std::declval<T>())</tt>
    static inline
    void
    convert(const std::map<std::string, T>& from, dynamic_t& to) {
        dynamic_t::object_t buffer;

        for (auto it = from.begin(); it != from.end(); ++it) {
            buffer.insert(dynamic_t::object_t::value_type(it->first, it->second));
        }

        to = std::move(buffer);
    }
};

//! \brief Converts std::unordered_map<std::string, T> to dynamic_t.
template<class T>
struct constructor<std::unordered_map<std::string, T>> {
    static const bool enable = true;

    //! \post <tt>to.is_object() == true && to.as_object().size() == from.size()</tt>
    //! \post For all <tt>std::string key; from.count(key) == to.as_object().count(key)</tt>
    //! \post For all <tt>std::string key; from.count(key) > 0 ==> to.as_object()[key] == dynamic_t(from[key])</tt>
    //! \throws std::bad_alloc
    //! \throws Any exceptions thrown by <tt>dynamic_t(std::declval<T>())</tt>
    static inline
    void
    convert(const std::unordered_map<std::string, T>& from, dynamic_t& to) {
        dynamic_t::object_t buffer;

        for (auto it = from.begin(); it != from.end(); ++it) {
            buffer.insert(dynamic_t::object_t::value_type(it->first, it->second));
        }

        to = std::move(buffer);
    }
};

}} // namespace kora::dynamic

#endif
