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

#ifndef KORA_UTILITY_SFINAE_HPP
#define KORA_UTILITY_SFINAE_HPP

namespace kora {

// Use it instead of std::enable_if to enable a specialization if your dependent type T has
// some nested type.
// Usage: template<class T> struct some_struct<T, typename requires_type<T::nested_type>::type> { }
template<class T, class Result = void>
struct requires_type {
    typedef Result type;
};

// Use it instead of std::enable_if to enable a specialization if your dependent type T has
// some method.
// Usage:
// template<class T>
// struct some_struct<T, typename requires_method<void(T::*)(int, double), T::method_name>::type> { }
template<class Method, Method, class Result = void>
struct requires_method {
    typedef Result type;
};

} // namespace kora

#endif
