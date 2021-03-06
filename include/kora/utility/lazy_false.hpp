/*
Copyright (c) 2013 Andrey Goryachev <andrey.goryachev@gmail.com>
Copyright (c) 2011-2013 Other contributors as noted in the AUTHORS file.

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

#ifndef KORA_UTILITY_LAZY_FALSE_HPP
#define KORA_UTILITY_LAZY_FALSE_HPP

/*! \file lazy_false.hpp
 *
 * Provides helper to delay static_assert evaluation.
 */

namespace kora {

/*! Delay static_assert evaluation to the template instantiation moment.
 *
 * Usage: <tt>template<class T> struct my_struct { static_assert(lazy_false<T>::value, "Fatality."); };</tt>
 */
template<class T>
struct lazy_false {
    static const bool value = sizeof(T) == 0;
};

} // namespace kora

#endif
