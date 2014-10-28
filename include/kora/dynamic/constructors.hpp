/*
    Copyright (c) 2013-2014 Andrey Goryachev <andrey.goryachev@gmail.com>
    Copyright (c) 2011-2014 Other contributors as noted in the AUTHORS file.

    This file is part of Leonidia.

    Leonidia is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Leonidia is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LEONIDIA_DYNAMIC_CONSTRUCTORS_HPP
#define LEONIDIA_DYNAMIC_CONSTRUCTORS_HPP

#include "kora/dynamic/error.hpp"
#include "kora/utility.hpp"

LEONIDIA_PUSH_VISIBILITY
#include <boost/numeric/conversion/cast.hpp>
LEONIDIA_POP_VISIBILITY

#include <tuple>
#include <type_traits>
#include <utility>
#include <unordered_map>

namespace kora {

template<class From>
struct dynamic_constructor<
    From,
    typename std::enable_if<std::is_integral<From>::value && std::is_unsigned<From>::value>::type
>
{
    static const bool enable = true;

    static inline
    void
    convert(From from, dynamic_t& to) {
        try {
            to = boost::numeric_cast<dynamic_t::uint_t>(from);
        } catch (const boost::numeric::bad_numeric_cast&) {
            throw bad_numeric_cast_t();
        }
    }
};

template<class From>
struct dynamic_constructor<
    From,
    typename std::enable_if<std::is_integral<From>::value && std::is_signed<From>::value>::type
>
{
    static const bool enable = true;

    static inline
    void
    convert(From from, dynamic_t& to) {
        try {
            to = boost::numeric_cast<dynamic_t::int_t>(from);
        } catch (const boost::numeric::bad_numeric_cast&) {
            throw bad_numeric_cast_t();
        }
    }
};

#ifdef LEONIDIA_NOT_BAD
template<class From>
struct dynamic_constructor<
    From,
    typename std::enable_if<std::is_enum<From>::value>::type
>
{
    static const bool enable = true;

    static inline
    void
    convert(const From& from, dynamic_t& to) {
        to = static_cast<dynamic_t::int_t>(from);
    }
};
#endif

template<class From>
struct dynamic_constructor<
    From,
    typename std::enable_if<std::is_floating_point<From>::value>::type
>
{
    static const bool enable = true;

    static inline
    void
    convert(From from, dynamic_t& to) {
        try {
            to = boost::numeric_cast<dynamic_t::double_t>(from);
        } catch (const boost::numeric::bad_numeric_cast&) {
            throw bad_numeric_cast_t();
        }
    }
};

template<size_t N>
struct dynamic_constructor<char[N]> {
    static const bool enable = true;

    static inline
    void
    convert(const char* from, dynamic_t& to) {
        dynamic_t::string_t buffer(from, N - 1);
        to = std::move(buffer);
    }
};

template<>
struct dynamic_constructor<const char*> {
    static const bool enable = true;

    static inline
    void
    convert(const char* from, dynamic_t& to) {
        dynamic_t::string_t buffer(from);
        to = std::move(buffer);
    }
};

template<class T>
struct dynamic_constructor<
    std::vector<T>,
    typename std::enable_if<!std::is_same<T, dynamic_t>::value>::type
>
{
    static const bool enable = true;

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

template<class... Args>
struct dynamic_constructor<std::tuple<Args...>> {
    static const bool enable = true;

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

    static inline
    void
    convert(const std::tuple<Args...>& from, dynamic_t& to) {
        dynamic_t::array_t buffer;
        buffer.reserve(sizeof...(Args));

        copy_tuple_to_vector<sizeof...(Args), 1, Args...>::convert(from, buffer);

        to = std::move(buffer);
    }
};

template<>
struct dynamic_constructor<std::map<std::string, dynamic_t>> {
    static const bool enable = true;

    template<class Object>
    static inline
    void
    convert(Object&& from, dynamic_t& to) {
        to = dynamic_t::object_t(std::forward<Object>(from));
    }
};

template<class T>
struct dynamic_constructor<
    std::map<std::string, T>,
    typename std::enable_if<!std::is_same<T, dynamic_t>::value>::type
>
{
    static const bool enable = true;

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

template<class T>
struct dynamic_constructor<std::unordered_map<std::string, T>> {
    static const bool enable = true;

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

} // namespace kora

#endif
