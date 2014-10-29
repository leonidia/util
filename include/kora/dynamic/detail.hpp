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

#ifndef KORA_DYNAMIC_DETAIL_HPP
#define KORA_DYNAMIC_DETAIL_HPP

#include "kora/utility.hpp"

KORA_PUSH_VISIBILITY
#include <boost/variant.hpp>
KORA_POP_VISIBILITY

#include <memory>
#include <type_traits>
#include <utility>

namespace kora {

namespace detail { namespace dynamic {

// Helps to use STL containers with incomplete dynamic_t.
template<class T>
class incomplete_wrapper {
public:
    // These constructors are needed just to satisfy the requirements of boost::variant.
    incomplete_wrapper() = default;

    incomplete_wrapper(const incomplete_wrapper&) KORA_NOEXCEPT { }

    incomplete_wrapper&
    operator=(const incomplete_wrapper&) KORA_NOEXCEPT {
        return *this;
    }

    T&
    get() KORA_NOEXCEPT {
        return *m_data;
    }

    const T&
    get() const KORA_NOEXCEPT {
        return *m_data;
    }

    void
    set(T* object) KORA_NOEXCEPT {
        m_data.reset(object);
    }

    T*
    release() KORA_NOEXCEPT {
        return m_data.release();
    }

private:
    std::unique_ptr<T> m_data;
};

template<class VisitorReference>
struct dynamic_visitor_applier :
    public boost::static_visitor<typename std::decay<VisitorReference>::type::result_type>
{
    typedef typename std::remove_reference<VisitorReference>::type visitor_type;
    typedef typename std::decay<VisitorReference>::type::result_type result_type;

    dynamic_visitor_applier(visitor_type *v) :
        m_visitor(v)
    { }

    template<class T>
    result_type
    operator()(T& v) const {
        return std::forward<VisitorReference>(*m_visitor)(v);
    }

    template<class T>
    result_type
    operator()(incomplete_wrapper<T>& v) const {
        return std::forward<VisitorReference>(*m_visitor)(v.get());
    }

    template<class T>
    result_type
    operator()(const incomplete_wrapper<T>& v) const {
        return std::forward<VisitorReference>(*m_visitor)(v.get());
    }

private:
    visitor_type *m_visitor;
};

}}} // namespace kora::detail::dynamic

#endif
