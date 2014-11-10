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

#include "kora/dynamic/dynamic.hpp"

using namespace kora;

const dynamic_t dynamic_t::null;
const dynamic_t dynamic_t::empty_string = dynamic_t::string_t();
const dynamic_t dynamic_t::empty_array = dynamic_t::array_t();
const dynamic_t dynamic_t::empty_object = dynamic_t::object_t();

// Should be applied directly to the underlying variant.
struct dynamic_t::move_visitor:
    public boost::static_visitor<>
{
    move_visitor(dynamic_t::value_t& destination) :
        m_destination(destination)
    { }

    template<class T>
    void
    operator()(T& v) const {
        m_destination = v;
    }

    void
    operator()(std::string& v) const {
        m_destination = std::string();
        boost::get<std::string>(m_destination) = std::move(v);
    }

    template<class T>
    void
    operator()(detail::dynamic::incomplete_wrapper<T>& v) const {
        m_destination = detail::dynamic::incomplete_wrapper<T>();
        boost::get<detail::dynamic::incomplete_wrapper<T>>(m_destination).set(v.release());
    }

private:
    dynamic_t::value_t& m_destination;
};

namespace {

struct assign_visitor:
    public boost::static_visitor<>
{
    assign_visitor(dynamic_t& destination) :
        m_destination(destination)
    { }

    template<class T>
    void
    operator()(T& v) const {
        m_destination = v;
    }

private:
    dynamic_t& m_destination;
};

struct equals_visitor:
    public boost::static_visitor<bool>
{
    equals_visitor(const dynamic_t& other) :
        m_other(other)
    { }

    bool
    operator()(const dynamic_t::null_t&) const {
        return m_other.is_null();
    }

    bool
    operator()(const dynamic_t::bool_t& v) const {
        return m_other.is_bool() && m_other.as_bool() == v;
    }

    bool
    operator()(const dynamic_t::int_t& v) const {
        if (m_other.is_int()) {
            return v == m_other.as_int();
        } else if (m_other.is_uint()) {
            return v >= 0 && static_cast<dynamic_t::uint_t>(v) == m_other.as_uint();
        } else {
            return m_other.is_double() && dynamic_t::double_t(v) == m_other.as_double();
        }
    }

    bool
    operator()(const dynamic_t::uint_t& v) const {
        if (m_other.is_uint()) {
            return v == m_other.as_uint();
        } else if (m_other.is_int()) {
            return m_other.as_int() >= 0 && v == m_other.to<dynamic_t::uint_t>();
        } else {
            return m_other.is_double() && dynamic_t::double_t(v) == m_other.as_double();
        }
    }

    bool
    operator()(const dynamic_t::double_t& v) const {
        if (m_other.is_uint()) {
            return v == dynamic_t::double_t(m_other.as_uint());
        } else if (m_other.is_int()) {
            return v == dynamic_t::double_t(m_other.as_int());
        } else {
            return m_other.is_double() && m_other.as_double() == v;
        }
    }

    bool
    operator()(const dynamic_t::string_t& v) const {
        return m_other.is_string() && m_other.as_string() == v;
    }

    bool
    operator()(const dynamic_t::array_t& v) const {
        return m_other.is_array() && m_other.as_array() == v;
    }

    bool
    operator()(const dynamic_t::object_t& v) const {
        return m_other.is_object() && m_other.as_object() == v;
    }

private:
    const dynamic_t& m_other;
};

} // namespace

dynamic_t::dynamic_t() KORA_NOEXCEPT :
    m_value(null_t())
{ }

dynamic_t::dynamic_t(const dynamic_t& other) :
    m_value(null_t())
{
    other.apply(assign_visitor(*this));
}

dynamic_t::dynamic_t(dynamic_t&& other) KORA_NOEXCEPT :
    m_value(null_t())
{
    boost::apply_visitor(dynamic_t::move_visitor(m_value), other.m_value);
}

dynamic_t::dynamic_t(dynamic_t::null_t value) KORA_NOEXCEPT :
    m_value(value)
{ }

dynamic_t::dynamic_t(dynamic_t::bool_t value) KORA_NOEXCEPT :
    m_value(value)
{ }

dynamic_t::dynamic_t(dynamic_t::int_t value) KORA_NOEXCEPT :
    m_value(value)
{ }

dynamic_t::dynamic_t(dynamic_t::uint_t value) KORA_NOEXCEPT :
    m_value(value)
{ }

dynamic_t::dynamic_t(dynamic_t::double_t value) KORA_NOEXCEPT :
    m_value(value)
{ }

dynamic_t::dynamic_t(const dynamic_t::string_t& value) :
    m_value(value)
{ }

dynamic_t::dynamic_t(dynamic_t::string_t&& value) KORA_NOEXCEPT :
    m_value(dynamic_t::string_t())
{
    as_string() = std::move(value);
}

dynamic_t::dynamic_t(dynamic_t::array_t value) :
    m_value(detail::dynamic::incomplete_wrapper<dynamic_t::array_t>())
{
    boost::get<detail::dynamic::incomplete_wrapper<dynamic_t::array_t>>(m_value).set(
        new dynamic_t::array_t(std::move(value))
    );
}

dynamic_t::dynamic_t(dynamic_t::object_t value) :
    m_value(detail::dynamic::incomplete_wrapper<dynamic_t::object_t>())
{
    boost::get<detail::dynamic::incomplete_wrapper<dynamic_t::object_t>>(m_value).set(
        new dynamic_t::object_t(std::move(value))
    );
}

dynamic_t&
dynamic_t::operator=(const dynamic_t& other) {
    other.apply(assign_visitor(*this));
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t&& other) KORA_NOEXCEPT {
    boost::apply_visitor(dynamic_t::move_visitor(m_value), other.m_value);
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::null_t value) KORA_NOEXCEPT {
    m_value = value;
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::bool_t value) KORA_NOEXCEPT {
    m_value = value;
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::int_t value) KORA_NOEXCEPT {
    m_value = value;
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::uint_t value) KORA_NOEXCEPT {
    m_value = value;
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::double_t value) KORA_NOEXCEPT {
    m_value = value;
    return *this;
}

dynamic_t&
dynamic_t::operator=(const dynamic_t::string_t& value) {
    m_value = value;
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::string_t&& value) KORA_NOEXCEPT {
    m_value = dynamic_t::string_t();
    as_string() = std::move(value);
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::array_t value) {
    std::unique_ptr<dynamic_t::array_t> buffer(new dynamic_t::array_t(std::move(value)));
    m_value = detail::dynamic::incomplete_wrapper<dynamic_t::array_t>();
    boost::get<detail::dynamic::incomplete_wrapper<dynamic_t::array_t>>(m_value).set(buffer.release());
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t::object_t value) {
    std::unique_ptr<dynamic_t::object_t> buffer(new dynamic_t::object_t(std::move(value)));
    m_value = detail::dynamic::incomplete_wrapper<dynamic_t::object_t>();
    boost::get<detail::dynamic::incomplete_wrapper<dynamic_t::object_t>>(m_value).set(buffer.release());
    return *this;
}

dynamic_t::bool_t
dynamic_t::as_bool() const {
    return get<bool_t>();
}

dynamic_t::int_t
dynamic_t::as_int() const {
    return get<int_t>();
}

dynamic_t::uint_t
dynamic_t::as_uint() const {
    return get<uint_t>();
}

dynamic_t::double_t
dynamic_t::as_double() const {
    return get<double_t>();
}

const dynamic_t::string_t&
dynamic_t::as_string() const {
    return get<string_t>();
}

const dynamic_t::array_t&
dynamic_t::as_array() const {
    return get<detail::dynamic::incomplete_wrapper<array_t>>().get();
}

const dynamic_t::object_t&
dynamic_t::as_object() const {
    return get<detail::dynamic::incomplete_wrapper<object_t>>().get();
}

dynamic_t::string_t&
dynamic_t::as_string() {
    if (is_null()) {
        *this = string_t();
    }

    return get<string_t>();
}

dynamic_t::array_t&
dynamic_t::as_array() {
    return get<detail::dynamic::incomplete_wrapper<array_t>>().get();
}

dynamic_t::object_t&
dynamic_t::as_object() {
    return get<detail::dynamic::incomplete_wrapper<object_t>>().get();
}

bool
dynamic_t::is_null() const KORA_NOEXCEPT {
    return is<null_t>();
}

bool
dynamic_t::is_bool() const KORA_NOEXCEPT {
    return is<bool_t>();
}

bool
dynamic_t::is_int() const KORA_NOEXCEPT {
    return is<int_t>();
}

bool
dynamic_t::is_uint() const KORA_NOEXCEPT {
    return is<uint_t>();
}

bool
dynamic_t::is_double() const KORA_NOEXCEPT {
    return is<double_t>();
}

bool
dynamic_t::is_string() const KORA_NOEXCEPT {
    return is<string_t>();
}

bool
dynamic_t::is_array() const KORA_NOEXCEPT {
    return is<detail::dynamic::incomplete_wrapper<array_t>>();
}

bool
dynamic_t::is_object() const KORA_NOEXCEPT {
    return is<detail::dynamic::incomplete_wrapper<object_t>>();
}

bool
kora::operator==(const dynamic_t& left, const dynamic_t& right) KORA_NOEXCEPT {
    return left.apply(equals_visitor(right));
}

bool
kora::operator!=(const dynamic_t& left, const dynamic_t& right) KORA_NOEXCEPT {
    return !left.apply(equals_visitor(right));
}
