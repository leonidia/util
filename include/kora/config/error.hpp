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
#include <string>

namespace kora {

//! Base class of all custom exceptions thrown by config_t and config_parser_t.
class KORA_API config_error_t :
    public std::exception
{
public:
    //! \post <tt>this->what() == message</tt>
    //! \param message Error message, obviously.
    explicit
    config_error_t(std::string message);

    ~config_error_t() KORA_NOEXCEPT;

    const char *
    what() const KORA_NOEXCEPT;

private:
    std::string m_message;
};

//! Base class of all custom exceptions thrown by config_t.
class KORA_API config_value_error_t :
    public config_error_t
{
public:
    //! \post this->what() provides some nice human readable error message
    //! pointing to the related place in the config.
    //! \param path Path to a place in the config which the error is related to.
    //! \param message Description of the error.
    config_value_error_t(std::string path, std::string message);

    ~config_value_error_t() KORA_NOEXCEPT;

    //! \returns Path to a place in the config which the error is related to.
    const std::string&
    path() const KORA_NOEXCEPT;

    //! \returns Description of the error.
    const std::string&
    message() const KORA_NOEXCEPT;

private:
    std::string m_path;
    std::string m_message;
};

//! Thrown by config_t when the user tried to access a value which doesn't exist in the config.
class KORA_API config_access_error_t :
    public config_value_error_t
{
public:
    //! \sa config_value_error_t::config_value_error_t(std::string, std::string)
    config_access_error_t(std::string path, std::string message);

    ~config_access_error_t() KORA_NOEXCEPT;
};

//! Indicates converison error. When the user tried to do something incompatible with actual config_t's value.
class KORA_API config_cast_error_t :
    public config_value_error_t
{
public:
    //! \sa config_value_error_t::config_value_error_t(std::string, std::string)
    config_cast_error_t(std::string path, std::string message);

    ~config_cast_error_t() KORA_NOEXCEPT;
};

//! Thrown by config_parser_t trying to parse incorrect data.
class KORA_API config_parser_error_t :
    public config_error_t
{
public:
    /*!
     * \post <tt>this->what() == message</tt>
     * \param message Human readable message describing the error and place where it has occured.
     * \param parse_error Text description of the error.
     * \param line_number Line where the error has occured.
     * \param column_number Column in which the error has occured.
     */
    config_parser_error_t(std::string message,
                          std::string parse_error,
                          size_t line_number,
                          size_t column_number);

    ~config_parser_error_t() KORA_NOEXCEPT;

    //! \returns Text description of the error.
    const std::string&
    parse_error() const KORA_NOEXCEPT;

    //! \returns Line where the error has occured.
    size_t
    line_number() const KORA_NOEXCEPT;

    //! \returns Column in which the error has occured.
    size_t
    column_number() const KORA_NOEXCEPT;

private:
    std::string m_parse_error;
    size_t m_line_number;
    size_t m_column_number;
};

} // namespace kora

#endif
