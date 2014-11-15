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

#ifndef KORA_CONFIG_PARSER_HPP
#define KORA_CONFIG_PARSER_HPP

#include "kora/config/config.hpp"

#include "kora/dynamic.hpp"
#include "kora/utility.hpp"

#include <istream>
#include <memory>
#include <string>

namespace kora {

/*! Creates config_t from JSON objects.
 * \warning It owns the underlying dynamic_t object of all produced config_t objects and should be alive
 * while any produced config_t is in use.
 */
class config_parser_t {
public:
    //! \post <tt>this->root().underlying_object().is_null() == true</tt>
    KORA_API
    config_parser_t() KORA_NOEXCEPT;

    /*!
     * \throws std::bad_alloc.
     */
    KORA_API
    config_parser_t(const config_parser_t &other);

    /*! Create config from a JSON object stored in a file.
     *
     * Actually does the same as the default constructor and then calls method open(const std::string&) with \p path.
     * \param path Path to the file with the JSON object.
     * \throws Anything thrown by \p open(path).
     *
     * \sa open(const std::string&)
     */
    KORA_API
    explicit
    config_parser_t(const std::string &path);

    /*! Create config from a JSON object.
     *
     * Actually does the same as the default constructor and then calls method parse(std::istream&) with \p stream.
     * \param stream The source of the JSON object.
     * \throws Anything thrown by \p parse(stream).
     *
     * \sa parse(std::istream&)
     */
    KORA_API
    explicit
    config_parser_t(std::istream &stream);

    KORA_API
    ~config_parser_t() KORA_NOEXCEPT;

    /*!
     * \throws std::bad_alloc.
     */
    KORA_API
    config_parser_t&
    operator=(const config_parser_t &other);

    /*! Parse config from a JSON object stored in a file.
     *
     * \warning Invalidates all config_t objects produced earlier.
     *
     * \param path Path to the file with the JSON object.
     * \returns \p root() after new configuration is parsed and stored in the parser.
     * \throws config_parser_error_t If the file contains anything but a valid JSON object.
     * \throws std::runtime_error If the method failed to open the file.
     * \throws std::bad_alloc
     *
     * \sa parse(std::istream&)
     */
    KORA_API
    config_t
    open(const std::string &path);

    /*! Parse config from a JSON object.
     *
     * \warning Invalidates all config_t objects produced earlier.
     *
     * \param stream The source of the JSON object.
     * \returns \p root() after new configuration is parsed and stored in the parser.
     * \throws config_parser_error_t If the stream contains anything but a valid JSON object.
     * \throws std::bad_alloc
     * \throws Any exception thrown by \p kora::dynamic::read_json(stream).
     */
    KORA_API
    config_t
    parse(std::istream &stream);

    /*! Get config object.
     *
     * \returns config_t with path "<root>" and recently loaded dynamic object.
     * \throws Any exception thrown by constructor of config_t.
     */
    KORA_API
    config_t
    root() const;

private:
    class implementation_t;

    std::unique_ptr<implementation_t> m_impl;
};

} // namespace kora

#endif
