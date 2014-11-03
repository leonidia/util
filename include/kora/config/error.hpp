/*
Copyright (c) 2014 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef KORA_CONFIG_ERROR_HPP
#define KORA_CONFIG_ERROR_HPP

#include "kora/utility.hpp"

#include <exception>

namespace kora {

class KORA_API config_error_t :
    public std::exception
{
public:
    explicit
    config_error_t(std::string message);

    ~config_error_t() KORA_NOEXCEPT;

    const char *
    what() const KORA_NOEXCEPT;

private:
    std::string m_message;
};

class KORA_API config_parser_error_t :
    public config_error_t
{
public:
    config_parser_error_t(std::string message,
                          std::string parse_error,
                          size_t line_number,
                          size_t column_number);

    ~config_parser_error_t() KORA_NOEXCEPT;

    const std::string&
    parse_error() const;

    size_t
    line_number() const;

    size_t
    column_number() const;

private:
    std::string m_parse_error;
    size_t m_line_number;
    size_t m_column_number;
};

} // namespace kora

#endif
