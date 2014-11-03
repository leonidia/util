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

class config_parser_t
{
public:
    KORA_API config_parser_t();
    KORA_API ~config_parser_t();

    KORA_API void open(const std::string &path);
    KORA_API void parse(std::istream &stream);

    KORA_API config_t root() const;

private:
    dynamic_t m_root;
};

} // namespace kora

#endif
