/*
Copyright (c) 2014 Artem Sokolov <derikon@yandex.ru>
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

#ifndef KORA_UTILITY_MAKE_UNIQUE_HPP
#define KORA_UTILITY_MAKE_UNIQUE_HPP

#include <memory>
#include <type_traits>

namespace kora {

namespace detail {

template<typename T>
struct is_array_of_unknown_bound : std::false_type {};

template<typename T>
struct is_array_of_unknown_bound<T[]> : std::true_type {};

template<typename T, std::size_t N>
struct is_array_of_unknown_bound<T[N]> : std::false_type {};

template<typename T>
struct is_array_of_known_bound : std::false_type {};

template<typename T>
struct is_array_of_known_bound<T[]> : std::false_type {};

template<typename T, std::size_t N>
struct is_array_of_known_bound<T[N]> : std::true_type {};

} // namespace detail

//! Constructs a non-array type T. The arguments are passed to the constructor of T.
//! The function does not participate in the overload resolution if T is an array type.
//! \param args list of arguments with which an instance of T will be constructed
//! \returns std::unique_ptr<T>(new T(std::forward<Args>(args)...))
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

//! Constructs an array of unknown bound T.
//! The function does not participate in the overload resolution unless T is an array of unknown bound.
//! \param size the size of the array to construct
//! returns std::unique_ptr<T>(new typename std::remove_extent<T>::type[size]())
template<typename T>
typename std::enable_if<detail::is_array_of_unknown_bound<T>::value, std::unique_ptr<T>>::type
make_unique(std::size_t size) {
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[size]());
}

//! Construction of arrays of known bound is disallowed.
//! The function does not participate in the overload resolution unless T is an array of known bound.
template<typename T, typename... Args>
typename std::enable_if<detail::is_array_of_known_bound<T>::value>::type
make_unique(Args&&...) = delete;

} // namespace kora

#endif
