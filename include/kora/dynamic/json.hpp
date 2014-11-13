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

#ifndef KORA_DYNAMIC_JSON_HPP
#define KORA_DYNAMIC_JSON_HPP

#include "kora/dynamic/dynamic.hpp"

#include <istream>
#include <ostream>

namespace kora {

/*!\relatesalso dynamic_t
 *
 * Serializes dynamic object into unformatted JSON.
 *
 * This function may serialize not only objects and arrays, but any dynamic_t object.
 * It's not like standard JSON, and you should perform additional checks on yourown
 * if you want to receive a JSON object or array.
 *
 * \param output Stream to write the resulting JSON to.
 * \param value The dynamic object to serialize.
 * \throws std::bad_alloc
 * \throws Any exception thrown by \p output.
 */
KORA_API
void
write_json(std::ostream& output, const dynamic_t& value);

/*!\relatesalso dynamic_t
 *
 * Serializes dynamic object into human-readable JSON.
 *
 * It works exactly as to_json(const dynamic_t&, std::ostream&) except of the formating.
 *
 * \param output Stream to write the resulting JSON to.
 * \param value The dynamic object to serialize.
 * \param indent Number of spaces in one indentation level.
 * \throws std::bad_alloc
 * \throws Any exception thrown by \p output.
 *
 * \sa to_json(const dynamic_t&, std::ostream&)
 */
KORA_API
void
write_pretty_json(std::ostream& output, const dynamic_t& value, size_t indent = 4);

namespace dynamic {

/*!\relatesalso kora::dynamic_t
 *
 * Creates dynamic object from JSON.
 *
 * This function doesn't require the input stream to contain only one JSON object.
 * It reads one JSON object with surrounding spaces and leaves other data untouched.
 *
 * \param input Stream containing the JSON.
 * \returns Constructed dynamic object.
 * \throws json_parsing_error_t
 * \throws std::bad_alloc
 * \throws Any exception thrown by \p input.
 */
KORA_API
dynamic_t
from_json(std::istream &input);

} // namespace dynamic

} // namespace kora

#endif
