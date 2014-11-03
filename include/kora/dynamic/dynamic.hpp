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

#ifndef KORA_DYNAMIC_DYNAMIC_HPP
#define KORA_DYNAMIC_DYNAMIC_HPP

#include "kora/dynamic/detail.hpp"

KORA_PUSH_VISIBLE
#include <boost/variant.hpp>
KORA_POP_VISIBILITY

#include <string>
#include <utility>
#include <vector>

namespace kora {

template<class From, class = void>
struct dynamic_constructor {
    static const bool enable = false;
};

template<class To, class = void>
struct dynamic_converter { };

class dynamic_t {
public:
    typedef boost::blank           null_t;
    typedef bool                   bool_t;
    typedef int64_t                int_t;
    typedef uint64_t               uint_t;
    typedef double                 double_t;
    typedef std::string            string_t;
    typedef std::vector<dynamic_t> array_t;

    class object_t;

    // Just useful constants which may be accessed by reference from any place of the program.
    KORA_API static const dynamic_t null;
    KORA_API static const dynamic_t empty_string;
    KORA_API static const dynamic_t empty_array;
    KORA_API static const dynamic_t empty_object;

public:
    KORA_API
    dynamic_t() KORA_NOEXCEPT;

    KORA_API
    dynamic_t(const dynamic_t& other);

    KORA_API
    dynamic_t(dynamic_t&& other) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(null_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(bool_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(int_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(uint_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(double_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(const string_t& value);

    KORA_API
    dynamic_t(string_t&& value);

    KORA_API
    dynamic_t(array_t value);

    KORA_API
    dynamic_t(object_t value);

    template<class T>
    dynamic_t(
        T&& from,
        typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable>::type* = 0
    );

    KORA_API
    dynamic_t&
    operator=(const dynamic_t& other);

    KORA_API
    dynamic_t&
    operator=(dynamic_t&& other) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(null_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(bool_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(int_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(uint_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(double_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(const string_t& value);

    KORA_API
    dynamic_t&
    operator=(string_t&& value);

    KORA_API
    dynamic_t&
    operator=(array_t value);

    KORA_API
    dynamic_t&
    operator=(object_t value);

    template<class T>
    typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable, dynamic_t&>::type
    operator=(T&& from);

    template<class Visitor>
    typename std::decay<Visitor>::type::result_type
    apply(Visitor&& visitor);

    template<class Visitor>
    typename std::decay<Visitor>::type::result_type
    apply(Visitor&& visitor) const;

    KORA_API
    bool
    is_null() const;

    KORA_API
    bool
    is_bool() const;

    KORA_API
    bool
    is_int() const;

    KORA_API
    bool
    is_uint() const;

    KORA_API
    bool
    is_double() const;

    KORA_API
    bool
    is_string() const;

    KORA_API
    bool
    is_array() const;

    KORA_API
    bool
    is_object() const;

    KORA_API
    bool_t
    as_bool() const;

    KORA_API
    int_t
    as_int() const;

    KORA_API
    uint_t
    as_uint() const;

    KORA_API
    double_t
    as_double() const;

    KORA_API
    const string_t&
    as_string() const;

    KORA_API
    const array_t&
    as_array() const;

    KORA_API
    const object_t&
    as_object() const;

    KORA_API
    string_t&
    as_string();

    KORA_API
    array_t&
    as_array();

    KORA_API
    object_t&
    as_object();

    template<class T>
    bool
    convertible_to() const;

    template<class T, class Controller>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to(Controller&& controller) const;

    template<class T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to() const;

public:
    KORA_API
    static
    dynamic_t
    from_json(std::istream &input);

    KORA_API
    void
    to_json(std::ostream &output) const;

private:
    template<class T>
    T&
    get();

    template<class T>
    const T&
    get() const;

    template<class T>
    bool
    is() const;

    struct move_visitor;

private:
    typedef boost::variant<
        null_t,
        bool_t,
        int_t,
        uint_t,
        double_t,
        string_t,
        detail::dynamic::incomplete_wrapper<array_t>,
        detail::dynamic::incomplete_wrapper<object_t>
    > value_t;

    value_t m_value;
};

KORA_API
bool
operator==(const dynamic_t& left, const dynamic_t& right);

KORA_API
bool
operator!=(const dynamic_t& left, const dynamic_t& right);

KORA_API
std::ostream&
operator<<(std::ostream& stream, const dynamic_t& value);

} // namespace kora

#include "kora/dynamic/dynamic.impl"
#include "kora/dynamic/object.hpp"
#include "kora/dynamic/constructors.hpp"
#include "kora/dynamic/converters.hpp"

#endif
