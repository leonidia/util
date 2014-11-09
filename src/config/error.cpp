/*
Copyright (c) 2014 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#include "kora/config/error.hpp"

using namespace kora;

config_error_t::config_error_t(std::string message) :
    m_message(std::move(message))
{ }

config_error_t::~config_error_t() KORA_NOEXCEPT { }

const char*
config_error_t::what() const KORA_NOEXCEPT {
    return m_message.c_str();
}

config_value_error_t::config_value_error_t(std::string path, std::string message) :
    config_error_t("error in item " + path + ": " + message),
    m_path(std::move(path)),
    m_message(std::move(message))
{ }

config_value_error_t::~config_value_error_t() KORA_NOEXCEPT { }

const std::string&
config_value_error_t::path() const {
    return m_path;
}

const std::string&
config_value_error_t::message() const {
    return m_message;
}

config_access_error_t::config_access_error_t(std::string path, std::string message) :
    config_value_error_t(path, message)
{ }

config_access_error_t::~config_access_error_t() KORA_NOEXCEPT { }

config_cast_error_t::config_cast_error_t(std::string path, std::string message) :
    config_value_error_t(path, message)
{ }

config_cast_error_t::~config_cast_error_t() KORA_NOEXCEPT { }

config_parser_error_t::config_parser_error_t(std::string message,
                                             std::string parse_error,
                                             size_t line_number,
                                             size_t column_number) :
    config_error_t(std::move(message)),
    m_parse_error(std::move(parse_error)),
    m_line_number(line_number),
    m_column_number(column_number)
{ }

config_parser_error_t::~config_parser_error_t() KORA_NOEXCEPT { }

const std::string&
config_parser_error_t::parse_error() const {
    return m_parse_error;
}

size_t
config_parser_error_t::line_number() const {
    return m_line_number;
}

size_t
config_parser_error_t::column_number() const {
    return m_column_number;
}
