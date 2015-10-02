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

#include "kora/dynamic/dynamic.hpp"

using namespace kora;

dynamic_t&
dynamic_t::object_t::at(const std::string& key, dynamic_t& default_) {
    auto it = find(key);

    if (it == end()) {
        return default_;
    } else {
        return it->second;
    }
}

const dynamic_t&
dynamic_t::object_t::at(const std::string& key, const dynamic_t& default_) const {
    auto it = find(key);

    if (it == end()) {
        return default_;
    } else {
        return it->second;
    }
}

dynamic_t
dynamic_t::object_t::at(const std::string& key, dynamic_t&& default_) const {
    const auto it = find(key);

    if (it == end()) {
        return default_;
    } else {
        return it->second;
    }
}

const dynamic_t&
dynamic_t::object_t::operator[](const std::string& key) const {
    return at(key);
}
