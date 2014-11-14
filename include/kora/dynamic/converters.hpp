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

#ifndef KORA_DYNAMIC_CONVERTERS_HPP
#define KORA_DYNAMIC_CONVERTERS_HPP

#include "kora/dynamic/dynamic.hpp"
#include "kora/dynamic/error.hpp"
#include "kora/utility.hpp"

KORA_PUSH_VISIBLE
#include <boost/numeric/conversion/cast.hpp>
KORA_POP_VISIBILITY

#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <set>

namespace kora { namespace dynamic {

//! \brief Converts dynamic_t to dynamic_t. It's here for convenience.
template<>
struct converter<dynamic_t> {
    typedef const dynamic_t& result_type;

    //! Doesn't call the controller. Never fails.
    //! \returns \p from
    template<class Controller>
    static inline
    const dynamic_t&
    convert(const dynamic_t& from, Controller&) KORA_NOEXCEPT {
        return from;
    }

    //! \returns \p true
    static inline
    bool
    convertible(const dynamic_t&) KORA_NOEXCEPT {
        return true;
    }
};

//! \brief Converts dynamic_t to bool and dynamic_t::bool_t (they are the same now).
template<>
struct converter<bool> {
    typedef bool result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with expected_bool_t error if <tt>from.is_bool() == false</tt>.\n
    //! \returns \p from.as_bool()
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_bool()) {
            return from.as_bool();
        } else {
            controller.fail(expected_bool_t(), from);
        }
    }

    //! \returns \p from.is_bool()
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_bool();
    }
};

//! \brief Converts dynamic_t to integral types.
#ifdef KORA_DOXYGEN
template<>
struct converter<Integral>
#else
template<class Integral>
struct converter<
    Integral,
    typename std::enable_if<std::is_integral<Integral>::value>::type
>
#endif
{
    typedef Integral result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_integer_t error if <tt>from.is_int() == false && from.is_uint() == false</tt>.\n
    //! Fails with \p numeric_overflow_t<result_type> if the stored number can't be represented in the result type.\n
    //! \returns The stored number casted to the result type
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_int()) {
            try {
                return boost::numeric_cast<result_type>(from.as_int());
            } catch (const boost::numeric::bad_numeric_cast&) {
                controller.fail(numeric_overflow_t<result_type>(), from);
            }
        } else if (from.is_uint()) {
            try {
                return boost::numeric_cast<result_type>(from.as_uint());
            } catch (const boost::numeric::bad_numeric_cast&) {
                controller.fail(numeric_overflow_t<result_type>(), from);
            }
        } else {
            controller.fail(expected_integer_t(), from);
        }
    }

    //! \returns \p true if \p from stores an integer number and this number can be represented
    //! in the result type without data loss. Otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (from.is_int()) {
            boost::numeric::converter<result_type, dynamic_t::int_t> converter;
            return converter.out_of_range(from.as_int()) == boost::numeric::cInRange;
        } else if (from.is_uint()) {
            boost::numeric::converter<result_type, dynamic_t::uint_t> converter;
            return converter.out_of_range(from.as_uint()) == boost::numeric::cInRange;
        }

        return false;
    }
};

//! \brief Converts dynamic_t to floating point types.
#ifdef KORA_DOXYGEN
template<>
struct converter<FloatingPoint>
#else
template<class FloatingPoint>
struct converter<
    FloatingPoint,
    typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type
>
#endif
{
    typedef FloatingPoint result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_number_t error if <tt>!from.is_int() && !from.is_uint() && !from.is_double()</tt>.\n
    //! Fails with \p numeric_overflow_t<result_type> if the stored number can't be represented in the result type.\n
    //! \returns The stored number casted to the result type
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        try {
            if (from.is_int()) {
                return boost::numeric_cast<result_type>(from.as_int());
            } else if (from.is_uint()) {
                return boost::numeric_cast<result_type>(from.as_uint());
            } else if (from.is_double()) {
                return boost::numeric_cast<result_type>(from.as_double());
            } else {
                controller.fail(expected_number_t(), from);
            }
        } catch (const boost::numeric::bad_numeric_cast&) {
            controller.fail(numeric_overflow_t<result_type>(), from);
        }
    }

    //! \returns \p true if \p from stores a number and this number can be represented
    //! in the result type without data loss. Otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (from.is_int()) {
            boost::numeric::converter<result_type, dynamic_t::int_t> converter;
            return converter.out_of_range(from.as_int()) == boost::numeric::cInRange;
        } else if (from.is_uint()) {
            boost::numeric::converter<result_type, dynamic_t::uint_t> converter;
            return converter.out_of_range(from.as_uint()) == boost::numeric::cInRange;
        } else if (from.is_double()) {
            boost::numeric::converter<result_type, dynamic_t::double_t> converter;
            return converter.out_of_range(from.as_double()) == boost::numeric::cInRange;
        }

        return false;
    }
};

//! \brief Converts dynamic_t to std::string and to dynamic_t::string_t (they are the same now).
template<>
struct converter<std::string> {
    typedef const std::string& result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_string_t error if <tt>!from.is_string()</tt>.\n
    //! \returns <tt>from.as_string()</tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_string()) {
            return from.as_string();
        } else {
            controller.fail(expected_string_t(), from);
        }
    }

    //! \returns <tt>from.is_string()</tt>
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_string();
    }
};

//! \brief Converts dynamic_t to C-string.
template<>
struct converter<const char*> {
    typedef const char *result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_string_t error if <tt>!from.is_string()</tt>.\n
    //! \returns <tt>from.as_string().c_str()</tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_string()) {
            return from.as_string().c_str();
        } else {
            controller.fail(expected_string_t(), from);
        }
    }

    //! \returns <tt>from.is_string()</tt>
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_string();
    }
};

//! \brief Converts dynamic_t to std::vector<dynamic_t> and dynamic_t::array_t (they are the same now).
template<>
struct converter<std::vector<dynamic_t>> {
    typedef const std::vector<dynamic_t>& result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_array_t error if <tt>!from.is_array()</tt>.\n
    //! \returns <tt>from.as_array()</tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_array()) {
            return from.as_array();
        } else {
            controller.fail(expected_array_t(), from);
        }
    }

    //! \returns <tt>from.is_array()</tt>
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_array();
    }
};

//! \brief Converts dynamic_t to std::vector.
template<class T>
struct converter<std::vector<T>> {
    typedef std::vector<T> result_type;

    //! Traverses the array stored in \p from. Converts items of the array to \p T.\n
    //! Fails with errors generated by <tt>dynamic_t::to<T>()</tt>.\n
    //! Fails with \p expected_array_t error if <tt>!from.is_array()</tt>.\n
    //! \returns Vector of <tt>from</tt>'s items converted to \p T.
    //! \throws std::bad_alloc
    //! \throws Any exception thrown by <tt>converter<T>::convert</tt>.
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_array()) {
            const dynamic_t::array_t& array = from.as_array();
            std::vector<T> result;

            controller.start_array(from);
            for (size_t i = 0; i < array.size(); ++i) {
                controller.item(i);
                result.emplace_back(array[i].to<T>(controller));
            }
            controller.finish_array();

            return result;
        } else {
            controller.fail(expected_array_t(), from);
        }
    }

    //! \returns \p true if <tt>from.is_array()</tt> and all <tt>from</tt>'s items are
    //! convertible to \p T, otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_array() && std::all_of(
            from.as_array().begin(),
            from.as_array().end(),
            std::bind(&dynamic_t::convertible_to<T>, std::placeholders::_1)
        );
    }
};

//! \brief Converts dynamic_t to std::set.
template<class T>
struct converter<std::set<T>> {
    typedef std::set<T> result_type;

    //! Traverses the array stored in \p from. Converts items of the array to \p T.\n
    //! Fails with errors generated by <tt>dynamic_t::to<T>()</tt>.\n
    //! Fails with \p expected_array_t error if <tt>!from.is_array()</tt>.\n
    //! \returns Set of <tt>from</tt>'s items converted to \p T.
    //! \throws std::bad_alloc
    //! \throws Any exception thrown by <tt>converter<T>::convert</tt>.
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_array()) {
            const dynamic_t::array_t& array = from.as_array();
            std::set<T> result;

            controller.start_array(from);
            for (size_t i = 0; i < array.size(); ++i) {
                controller.item(i);
                result.insert(array[i].to<T>(controller));
            }
            controller.finish_array();

            return result;
        } else {
            controller.fail(expected_array_t(), from);
        }
    }

    //! \returns \p true if <tt>from.is_array()</tt> and all <tt>from</tt>'s items are
    //! convertible to \p T, otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_array() && std::all_of(
            from.as_array().begin(),
            from.as_array().end(),
            std::bind(&dynamic_t::convertible_to<T>, std::placeholders::_1)
        );
    }
};

//! \brief Converts dynamic_t to std::tuple.
template<class... Args>
struct converter<std::tuple<Args...>> {
    typedef std::tuple<Args...> result_type;

    //! Traverses the array stored in \p from. Converts items of the array to <tt>Args...</tt>.\n
    //! Fails with errors generated by <tt>dynamic_t::to<Args>()...</tt>.\n
    //! Fails with \p expected_array_t error if <tt>!from.is_array() || from.as_array().size() != sizeof...(Args)</tt>.\n
    //! \returns Tuple with <tt>from</tt>'s items converted to <tt>Args...</tt>
    //! \throws std::bad_alloc
    //! \throws Any exception thrown by <tt>converter<Args>::convert...</tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_array() && sizeof...(Args) == from.as_array().size()) {
            if (sizeof...(Args) == 0) {
                return result_type();
            } else {
                return range_applier<sizeof...(Args) - 1>::convert(from, controller);
            }
        } else {
            controller.fail(expected_tuple_t(sizeof...(Args)), from);
        }
    }

    //! \returns \p true if <tt>from.is_array()</tt>, <tt>from.as_array().size() == sizeof...(Args)</tt>
    //! and all <tt>from</tt>'s items are convertible to the corresponding tuple items.
    //! Otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (from.is_array() && sizeof...(Args) == from.as_array().size()) {
            if (sizeof...(Args) == 0) {
                return true;
            } else {
                return range_applier<sizeof...(Args) - 1>::is_convertible(from.as_array());
            }
        } else {
            return false;
        }
    }

private:
    template<size_t... Idxs>
    struct range_applier;

    template<size_t First, size_t... Idxs>
    struct range_applier<First, Idxs...> :
        range_applier<First - 1, First, Idxs...>
    {
        static inline
        bool
        is_convertible(const dynamic_t::array_t& from) {
            return from[First].convertible_to<typename std::tuple_element<First, result_type>::type>() &&
                   range_applier<First - 1, First, Idxs...>::is_convertible(from);
        }
    };

    template<size_t... Idxs>
    struct range_applier<0, Idxs...> {
        template<size_t Index, class Controller>
        static inline
        typename std::tuple_element<Index, result_type>::type
        control_and_convert(const dynamic_t::array_t& from, Controller& controller) {
            controller.item(Index);
            return from[Index].to<typename std::tuple_element<Index, result_type>::type>(controller);
        }

        template<class Controller>
        static inline
        result_type
        convert(const dynamic_t& from, Controller& controller) {
            controller.start_array(from);

            std::tuple<Args...> result(
                control_and_convert<0>(from.as_array(), controller),
                control_and_convert<Idxs>(from.as_array(), controller)...
            );

            controller.finish_array();

            return result;
        }

        static inline
        bool
        is_convertible(const dynamic_t::array_t& from) {
            return from[0].convertible_to<typename std::tuple_element<0, result_type>::type>();
        }
    };
};

//! \brief Converts dynamic_t to std::pair.
template<class First, class Second>
struct converter<std::pair<First, Second>> {
    typedef std::pair<First, Second> result_type;

    template<size_t Index, class Controller>
    static inline
    typename std::tuple_element<Index, result_type>::type
    control_and_convert(const dynamic_t::array_t& from, Controller& controller) {
        controller.item(Index);
        return from[Index].to<typename std::tuple_element<Index, result_type>::type>(controller);
    }

    //! Traverses the array stored in \p from. Converts items of the array to \p First and \p Second.\n
    //! Fails with errors generated by <tt>dynamic_t::to<First>()</tt> and <tt>dynamic_t::to<Second>()</tt>.\n
    //! Fails with \p expected_array_t error if <tt>!from.is_array() || from.as_array().size() != 2</tt>.\n
    //! \returns Pair with <tt>from</tt>'s items converted to \p First and \p Second respectively.
    //! \throws std::bad_alloc
    //! \throws Any exception thrown by <tt>converter<First>::convert</tt>.
    //! \throws Any exception thrown by <tt>converter<Second>::convert</tt>.
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_array() && from.as_array().size() == 2) {
            controller.start_array(from);

            auto result = std::make_pair(
                control_and_convert<0>(from.as_array(), controller),
                control_and_convert<1>(from.as_array(), controller)
            );

            controller.finish_array();

            return result;
        } else {
            controller.fail(expected_tuple_t(2), from);
        }
    }

    //! \returns \p true if <tt>from.is_array()</tt>, <tt>from.as_array().size() == 2</tt>
    //! and <tt>from</tt>'s items are convertible to the corresponding pair items.
    //! Otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (from.is_array() && from.as_array().size() == 2) {
            return from.as_array()[0].convertible_to<First>() &&
                   from.as_array()[1].convertible_to<Second>();
        } else {
            return false;
        }
    }
};

//! \brief Converts dynamic_t to dynamic_t::object_t.
template<>
struct converter<dynamic_t::object_t> {
    typedef const dynamic_t::object_t& result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_object_t error if <tt>!from.is_object()</tt>.\n
    //! \returns <tt>from.as_object()</tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_object()) {
            return from.as_object();
        } else {
            controller.fail(expected_object_t(), from);
        }
    }

    //! \returns <tt>from.is_object()</tt>
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_object();
    }
};

//! \brief Converts dynamic_t to std::map<std::string, dynamic_t>.
template<>
struct converter<std::map<std::string, dynamic_t>> {
    typedef const std::map<std::string, dynamic_t>& result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p expected_object_t error if <tt>!from.is_object()</tt>.\n
    //! \returns <tt>from.as_object()</tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_object()) {
            return from.as_object();
        } else {
            controller.fail(expected_object_t(), from);
        }
    }

    //! \returns <tt>from.is_object()</tt>
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        return from.is_object();
    }
};

//! \brief Converts dynamic_t to std::map<std::string, T>.
template<class T>
struct converter<std::map<std::string, T>> {
    typedef std::map<std::string, T> result_type;

    //! Traverses the object stored in \p from. Converts items of the object to \p T.\n
    //! Fails with errors generated by <tt>dynamic_t::to<T>()</tt>.\n
    //! Fails with \p expected_object_t error if <tt>!from.is_object()</tt>.\n
    //! \returns Map of <tt>from</tt>'s key-value pairs where values are converted to \p T.
    //! \throws std::bad_alloc
    //! \throws Any exception thrown by <tt>converter<T>::convert</tt>.
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_object()) {
            result_type result;
            const dynamic_t::object_t& object = from.as_object();

            controller.start_object(from);
            for (auto it = object.begin(); it != object.end(); ++it) {
                controller.item(it->first);
                result.insert(typename result_type::value_type(it->first, it->second.to<T>(controller)));
            }
            controller.finish_object();

            return result;
        } else {
            controller.fail(expected_object_t(), from);
        }
    }

    //! \returns \p true if <tt>from.is_object()</tt> and all <tt>from</tt>'s values are
    //! convertible to \p T, otherwise returns \p false.
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (!from.is_object()) {
            return false;
        }

        const dynamic_t::object_t& object = from.as_object();

        for (auto it = object.begin(); it != object.end(); ++it) {
            if (!it->second.convertible_to<T>()) {
                return false;
            }

            return true;
        }
    }
};

//! \brief Converts dynamic_t to std::unordered_map<std::string, T>.
//! \sa converter<std::map<std::string, T>>
template<class T>
struct converter<std::unordered_map<std::string, T>> {
    typedef std::unordered_map<std::string, T> result_type;

    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.is_object()) {
            result_type result;
            const dynamic_t::object_t& object = from.as_object();

            controller.start_object(from);
            for (auto it = object.begin(); it != object.end(); ++it) {
                controller.item(it->first);
                result.insert(typename result_type::value_type(it->first, it->second.to<T>(controller)));
            }
            controller.finish_object();

            return result;
        } else {
            controller.fail(expected_object_t(), from);
        }
    }

    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (!from.is_object()) {
            return false;
        }

        const dynamic_t::object_t& object = from.as_object();

        for (auto it = object.begin(); it != object.end(); ++it) {
            if (!it->second.convertible_to<T>()) {
                return false;
            }

            return true;
        }
    }
};

}} // namespace kora::dynamic

#endif
