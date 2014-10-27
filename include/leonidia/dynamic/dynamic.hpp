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

#ifndef LEONIDIA_DYNAMIC_DYNAMIC_HPP
#define LEONIDIA_DYNAMIC_DYNAMIC_HPP

#include "leonidia/dynamic/detail.hpp"

LEONIDIA_PUSH_VISIBILITY
#include <boost/variant.hpp>
LEONIDIA_POP_VISIBILITY

#include <string>
#include <utility>
#include <vector>

namespace leonidia {

template<class From, class = void>
struct dynamic_constructor {
    static const bool enable = false;
};

template<class To, class = void>
struct dynamic_converter { };

class dynamic_t {
public:
    typedef bool                   bool_t;
    typedef int64_t                int_t;
    typedef uint64_t               uint_t;
    typedef double                 double_t;
    typedef std::string            string_t;
    typedef std::vector<dynamic_t> array_t;

    struct null_t { };

    class object_t;

    // Just useful constants which may be accessed by reference from any place of the program.
    LEONIDIA_API static const dynamic_t null;
    LEONIDIA_API static const dynamic_t empty_string;
    LEONIDIA_API static const dynamic_t empty_array;
    LEONIDIA_API static const dynamic_t empty_object;

public:
    LEONIDIA_API
    dynamic_t() LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(const dynamic_t& other);

    LEONIDIA_API
    dynamic_t(dynamic_t&& other);

    LEONIDIA_API
    dynamic_t(null_t value) LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(bool_t value) LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(int_t value) LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(uint_t value) LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(double_t value) LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(string_t value) LEONIDIA_NOEXCEPT;

    LEONIDIA_API
    dynamic_t(array_t value);

    LEONIDIA_API
    dynamic_t(object_t value);

    template<class T>
    dynamic_t(
        T&& from,
        typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable>::type* = 0
    );

    LEONIDIA_API
    dynamic_t&
    operator=(const dynamic_t& other);

    LEONIDIA_API
    dynamic_t&
    operator=(dynamic_t&& other);

    LEONIDIA_API
    dynamic_t&
    operator=(null_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(bool_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(int_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(uint_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(double_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(string_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(array_t value);

    LEONIDIA_API
    dynamic_t&
    operator=(object_t value);

    template<class T>
    typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable, dynamic_t&>::type
    operator=(T&& from);

    LEONIDIA_API
    bool
    operator==(const dynamic_t& other) const;

    LEONIDIA_API
    bool
    operator!=(const dynamic_t& other) const;

    template<class Visitor>
    typename std::decay<Visitor>::type::result_type
    apply(Visitor&& visitor) {
        typedef detail::dynamic::dynamic_visitor_applier<Visitor> applier_type;
        return boost::apply_visitor(applier_type(&visitor), m_value);
    }

    template<class Visitor>
    typename std::decay<Visitor>::type::result_type
    apply(Visitor&& visitor) const {
        typedef detail::dynamic::dynamic_visitor_applier<Visitor> applier_type;
        return boost::apply_visitor(applier_type(&visitor), m_value);
    }

    LEONIDIA_API
    bool
    is_null() const;

    LEONIDIA_API
    bool
    is_bool() const;

    LEONIDIA_API
    bool
    is_int() const;

    LEONIDIA_API
    bool
    is_uint() const;

    LEONIDIA_API
    bool
    is_double() const;

    LEONIDIA_API
    bool
    is_string() const;

    LEONIDIA_API
    bool
    is_array() const;

    LEONIDIA_API
    bool
    is_object() const;

    LEONIDIA_API
    bool_t
    as_bool() const;

    LEONIDIA_API
    int_t
    as_int() const;

    LEONIDIA_API
    uint_t
    as_uint() const;

    LEONIDIA_API
    double_t
    as_double() const;

    LEONIDIA_API
    const string_t&
    as_string() const;

    LEONIDIA_API
    const array_t&
    as_array() const;

    LEONIDIA_API
    const object_t&
    as_object() const;

    LEONIDIA_API
    string_t&
    as_string();

    LEONIDIA_API
    array_t&
    as_array();

    LEONIDIA_API
    object_t&
    as_object();

    template<class T>
    bool
    convertible_to() const;

    template<class T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to() const;

public:
    LEONIDIA_API
    static
    dynamic_t
    from_json(std::istream &input);

    LEONIDIA_API
    void
    to_json(std::ostream &output) const;

private:
    template<class T>
    T&
    get() {
        T* ptr = boost::get<T>(&m_value);

        if (ptr) {
            return *ptr;
        } else {
            throw std::bad_cast();
        }
    }

    template<class T>
    const T&
    get() const {
        const T* ptr = boost::get<T>(&m_value);

        if (ptr) {
            return *ptr;
        } else {
            throw std::bad_cast();
        }
    }

    template<class T>
    bool
    is() const {
        return static_cast<bool>(boost::get<T>(&m_value));
    }

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

template<class T>
dynamic_t::dynamic_t(
    T&& from,
    typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable>::type*
) :
    m_value(null_t())
{
    dynamic_constructor<typename pristine<T>::type>::convert(std::forward<T>(from), *this);
}

template<class T>
typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable, dynamic_t&>::type
dynamic_t::operator=(T&& from) {
    dynamic_constructor<typename pristine<T>::type>::convert(std::forward<T>(from), *this);
    return *this;
}

template<class T>
bool
dynamic_t::convertible_to() const {
    return dynamic_converter<typename pristine<T>::type>::convertible(*this);
}

template<class T>
typename dynamic_converter<typename pristine<T>::type>::result_type
dynamic_t::to() const {
    return dynamic_converter<typename pristine<T>::type>::convert(*this);
}

LEONIDIA_API
std::ostream&
operator<<(std::ostream& stream, const dynamic_t& value);

} // namespace leonidia

#include "leonidia/dynamic/object.hpp"
#include "leonidia/dynamic/constructors.hpp"
#include "leonidia/dynamic/converters.hpp"

#endif
