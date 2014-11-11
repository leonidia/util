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
#include <string>

namespace kora {

class config_parser_t {
public:
    //! \post <tt>this->root().underlying_object().is_null() == true</tt>
    KORA_API
    config_parser_t() KORA_NOEXCEPT;

    KORA_API
    explicit
    config_parser_t(const std::string &path);

    KORA_API
    explicit
    config_parser_t(std::istream &stream);

    KORA_API
    ~config_parser_t() KORA_NOEXCEPT;

    /*! Create config from a JSON object stored in a file.
     *
     * \param path Path to the file with the JSON object.
     * \returns \p root() after new configuration is parsed and stored in the parser.
     * \throws config_parser_error_t If the file contains an invalid JSON.
     * \throws config_parser_error_t If the file contains something after the JSON object.
     * \throws config_cast_error_t If the file contains a valid JSON,
     * but not an object (actually it may be only an array).
     * \throws std::runtime_error If the method failed to open the file.
     * \throws std::bad_alloc
     *
     * \sa parse(std::istream&)
     */
    KORA_API
    config_t
    open(const std::string &path);

    /*! Create config from a JSON object.
     *
     * \param stream The source of the JSON object.
     * \returns \p root() after new configuration is parsed and stored in the parser.
     * \throws config_parser_error_t If the stream contains an invalid JSON.
     * \throws config_parser_error_t If the stream contains something after the JSON object.
     * \throws config_cast_error_t If the stream contains a valid JSON,
     * but not an object (actually it may be only an array).
     * \throws std::bad_alloc
     * \throws Any exception thrown by \p dynamic_t::from_json(stream).
     */
    KORA_API
    config_t
    parse(std::istream &stream);

    /*! Get config object.
     *
     * \returns config_t with path "<root>" and recently loaded dynamic object.
     */
    KORA_API
    config_t
    root() const;

private:
    dynamic_t m_root;
};

} // namespace kora

#endif
