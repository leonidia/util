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

#ifndef KORA_DYNAMIC_OBJECT_HPP
#define KORA_DYNAMIC_OBJECT_HPP

#include <map>

namespace kora {

/*! Type to store unordered associative arrays in dynamic_t (object in terms of JSON).
 *
 * Keys are strings, values are dynamic_t objects.
 * It's guaranteed to have API backward compatible with std::map<std::string, dynamic_t>
 * except that the order of the keys is not defined and user-provided allocators aren't supported.
 */
class dynamic_t::object_t :
    public std::map<std::string, dynamic_t>
{
    typedef std::map<std::string, dynamic_t> base_type;

public:
    object_t() = default;

    template<class InputIt>
    object_t(InputIt first, InputIt last) :
        base_type(first, last)
    { }

    object_t(const object_t& other) :
        base_type(other)
    { }

    object_t(object_t&& other) KORA_NOEXCEPT :
        base_type(std::move(other))
    { }

    object_t(std::initializer_list<value_type> list) :
        base_type(list)
    { }

    object_t(const base_type& other) :
        base_type(other)
    { }

    object_t(base_type&& other) KORA_NOEXCEPT :
        base_type(std::move(other))
    { }

    object_t&
    operator=(const object_t& other) {
        base_type::operator=(other);
        return *this;
    }

    object_t&
    operator=(object_t&& other) KORA_NOEXCEPT {
        base_type::operator=(std::move(other));
        return *this;
    }

    using base_type::at;

    /*! Get value by key.
     *
     * \param[in] key The key to search in the object.
     * \param[in] default_ The value to return in case if the object doesn't contain the key.
     * \returns Value stored by the key or \p default_ if the object doesn't contain the key.
     */
    KORA_API
    dynamic_t&
    at(const std::string& key, dynamic_t& default_) KORA_NOEXCEPT;

    /*! Get value by key.
     *
     * \param[in] key The key to search in the object.
     * \param[in] default_ The value to return in case if the object doesn't contain the key.
     * \returns Value stored by the key or \p default_ if the object doesn't contain the key.
     */
    KORA_API
    const dynamic_t&
    at(const std::string& key, const dynamic_t& default_) const KORA_NOEXCEPT;

    using base_type::operator[];

    /*! Get value by key.
     *
     * \param[in] key The key to search in the object.
     * \returns Value stored by the key.
     * \throws std::out_of_range When the object doesn't contain the key.
     */
    KORA_API
    const dynamic_t&
    operator[](const std::string& key) const;
};

} // namespace kora

#endif
