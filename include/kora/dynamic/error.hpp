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

#ifndef KORA_DYNAMIC_ERROR_HPP
#define KORA_DYNAMIC_ERROR_HPP

#include "kora/utility.hpp"

#include <stdexcept>
#include <string>
#include <typeinfo>

namespace kora {

class KORA_API json_parsing_error_t :
    public std::invalid_argument
{
public:
    json_parsing_error_t(size_t offset, std::string message);

    ~json_parsing_error_t() KORA_NOEXCEPT;

    size_t
    offset() const KORA_NOEXCEPT;

    const char*
    message() const KORA_NOEXCEPT;

private:
    size_t m_offset;
    std::string m_message;
};

// Base type for all exceptions being thrown by dynamic_converter.
class KORA_API bad_cast_t :
    public std::bad_cast
{
public:
    virtual
    ~bad_cast_t() KORA_NOEXCEPT;
};

// These errors mean that the dynamic object being converted contains value of a wrong type.
class KORA_API expected_null_t :
    public bad_cast_t
{
public:
    ~expected_null_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

class KORA_API expected_bool_t :
    public bad_cast_t
{
public:
    ~expected_bool_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// The object should contain value of type dynamic_t::int_t.
class KORA_API expected_int_t :
    public bad_cast_t
{
public:
    ~expected_int_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// The object should contain value of type dynamic_t::uint_t.
class KORA_API expected_uint_t :
    public bad_cast_t
{
public:
    ~expected_uint_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// The object should contain any integer value (dynamic_t::int_t or dynamic_t::uint_t).
class KORA_API expected_integer_t :
    public bad_cast_t
{
public:
    ~expected_integer_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// The object should contain value of type dynamic_t::double_t.
class KORA_API expected_double_t :
    public bad_cast_t
{
public:
    ~expected_double_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// The object should contain any numeric value (floating point or integer).
class KORA_API expected_number_t :
    public bad_cast_t
{
public:
    ~expected_number_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

class KORA_API expected_string_t :
    public bad_cast_t
{
public:
    ~expected_string_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

class KORA_API expected_array_t :
    public bad_cast_t
{
public:
    ~expected_array_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

class KORA_API expected_object_t :
    public bad_cast_t
{
public:
    ~expected_object_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// Indicates that the object should contain an array of some fixed size.
class KORA_API expected_tuple_t :
    public bad_cast_t
{
public:
    explicit expected_tuple_t(size_t expected_size) KORA_NOEXCEPT;

    ~expected_tuple_t() KORA_NOEXCEPT;

    size_t
    expected_size() const KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;

private:
    size_t m_expected_size;
};

// Unable to convert the value to a numeric type.
class KORA_API bad_numeric_cast_t :
    public bad_cast_t
{
public:
    ~bad_numeric_cast_t() KORA_NOEXCEPT;

    virtual
    const char*
    what() const KORA_NOEXCEPT;
};

// Just to provide the information about the target type.
template<class TargetType>
class KORA_API numeric_overflow_t :
    public bad_numeric_cast_t
{
public:
    ~numeric_overflow_t() KORA_NOEXCEPT { }
};

} // namespace kora

#endif
