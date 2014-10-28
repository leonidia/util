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

class KORA_API bad_numeric_cast_t :
    public std::bad_cast
{
public:
    bad_numeric_cast_t() KORA_NOEXCEPT;

    ~bad_numeric_cast_t() KORA_NOEXCEPT;
};

} // namespace kora


#endif
