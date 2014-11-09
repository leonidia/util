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

#include "kora/dynamic/error.hpp"

using namespace kora;

json_parsing_error_t::json_parsing_error_t(size_t offset, std::string message) :
    std::invalid_argument("parsing error - " + message),
    m_offset(offset),
    m_message(message)
{ }

json_parsing_error_t::~json_parsing_error_t() KORA_NOEXCEPT { }

size_t
json_parsing_error_t::offset() const KORA_NOEXCEPT {
    return m_offset;
}

const char*
json_parsing_error_t::message() const KORA_NOEXCEPT {
    return m_message.data();
}

bad_cast_t::~bad_cast_t() KORA_NOEXCEPT { }

expected_null_t::~expected_null_t() KORA_NOEXCEPT { }

const char*
expected_null_t::what() const KORA_NOEXCEPT {
    return "the value expected to be null";
}

expected_bool_t::~expected_bool_t() KORA_NOEXCEPT { }

const char*
expected_bool_t::what() const KORA_NOEXCEPT {
    return "the value expected to be boolean";
}

expected_int_t::~expected_int_t() KORA_NOEXCEPT { }

const char*
expected_int_t::what() const KORA_NOEXCEPT {
    return "the value expected to be of type dynamic_t::int_t";
}

expected_uint_t::~expected_uint_t() KORA_NOEXCEPT { }

const char*
expected_uint_t::what() const KORA_NOEXCEPT {
    return "the value expected to be of type dynamic_t::uint_t";
}

expected_integer_t::~expected_integer_t() KORA_NOEXCEPT { }

const char*
expected_integer_t::what() const KORA_NOEXCEPT {
    return "the value expected to be an integer";
}

expected_double_t::~expected_double_t() KORA_NOEXCEPT { }

const char*
expected_double_t::what() const KORA_NOEXCEPT {
    return "the value expected to be of type dynamic_t::double_t";
}

expected_number_t::~expected_number_t() KORA_NOEXCEPT { }

const char*
expected_number_t::what() const KORA_NOEXCEPT {
    return "the value expected to be a number";
}

expected_string_t::~expected_string_t() KORA_NOEXCEPT { }

const char*
expected_string_t::what() const KORA_NOEXCEPT {
    return "the value expected to be a string";
}

expected_array_t::~expected_array_t() KORA_NOEXCEPT { }

const char*
expected_array_t::what() const KORA_NOEXCEPT {
    return "the value expected to be an array";
}

expected_object_t::~expected_object_t() KORA_NOEXCEPT { }

const char*
expected_object_t::what() const KORA_NOEXCEPT {
    return "the value expected to be an object";
}

expected_tuple_t::expected_tuple_t(size_t expected_size) KORA_NOEXCEPT :
    m_expected_size(expected_size)
{ }

expected_tuple_t::~expected_tuple_t() KORA_NOEXCEPT { }

size_t
expected_tuple_t::expected_size() const KORA_NOEXCEPT {
    return m_expected_size;
}

const char*
expected_tuple_t::what() const KORA_NOEXCEPT {
    return "the object expected to be a tuple";
}

bad_numeric_cast_t::~bad_numeric_cast_t() KORA_NOEXCEPT { }

const char*
bad_numeric_cast_t::what() const KORA_NOEXCEPT {
    return "the value can't be represented in the target type";
}
