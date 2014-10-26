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

#ifndef LEONIDIA_DYNAMIC_DETAIL_HPP
#define LEONIDIA_DYNAMIC_DETAIL_HPP

#include <boost/variant.hpp>

#include <memory>
#include <type_traits>

namespace leonidia {

class dynamic_t;

namespace detail { namespace dynamic {

// Helps to use STL containers
template<class T>
class incomplete_wrapper {
public:
    // These constructors are needed just to satisfy the requirements of boost::variant.
    incomplete_wrapper() = default;

    incomplete_wrapper(const incomplete_wrapper&) { }

    incomplete_wrapper&
    operator=(const incomplete_wrapper&) {
        return *this;
    }

    T&
    get() {
        return *m_data;
    }

    const T&
    get() const {
        return *m_data;
    }

    template<class... Args>
    void
    set(Args&&... args) {
        m_data.reset(new T(std::forward<Args>(args)...));
    }

private:
    std::unique_ptr<T> m_data;
};

/*
 * If you apply it to some T deduced in template<class T> foo bar(T&&),
 * you'll get non-const reference if the template is called with lvalue reference,
 * and const reference otherwise.
 * It's a helper to write one template function bar(T&&)
 * which behaves like two overloads bar(T&) and bar(const T&).
 */
template<class T>
struct reference_type {
    typedef typename std::add_const<T>::type const_type;
    typedef typename std::add_lvalue_reference<const_type>::type type;
};

template<class Visitor, class Result>
struct dynamic_visitor_applier :
    public boost::static_visitor<Result>
{
    dynamic_visitor_applier(Visitor v) :
        m_visitor(v)
    { }

    template<class T>
    Result
    operator()(T& v) const {
        return m_visitor(v);
    }

    template<class T>
    Result
    operator()(incomplete_wrapper<T>& v) const {
        return m_visitor(v.get());
    }

    template<class T>
    Result
    operator()(const incomplete_wrapper<T>& v) const {
        return m_visitor(v.get());
    }

private:
    Visitor m_visitor;
};

}}} // namespace leonidia::detail::dynamic

#endif
