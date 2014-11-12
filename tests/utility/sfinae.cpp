/*
Copyright (c) 2014 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#include <gtest/gtest.h>

#include "kora/utility/sfinae.hpp"

#include <type_traits>

namespace {

struct type_with_nested_type {
    typedef int nested_type;
};

template<class T, class = void>
struct test_requires_type : std::integral_constant<bool, false> { };

template<class T>
struct test_requires_type<T, typename kora::requires_type<typename T::nested_type>::type> :
    std::integral_constant<bool, true>
{ };

} // namespace

TEST(RequiresType, SimpleTest) {
    static_assert(!test_requires_type<int>::value, "Default specialization expected to be selected.");
    static_assert(test_requires_type<type_with_nested_type>::value,
                  "Specialization with requires_type expected to be selected.");

    SUCCEED();
}

namespace {

struct type_with_int_member {
    int x;
};

struct type_with_method {
    void
    x(int) { }
};

template<class T, class = void>
struct test_requires_member : std::integral_constant<int, 1> { };

template<class T>
struct test_requires_member<T, typename kora::requires_member<void(T::*)(int), &T::x>::type> :
    std::integral_constant<int, 2>
{ };

template<class T>
struct test_requires_member<T, typename kora::requires_member<int T::*, &T::x>::type> :
    std::integral_constant<int, 3>
{ };

} // namespace

TEST(RequiresMember, SimpleTest) {
    static_assert(test_requires_member<int>::value == 1,
                  "Default specialization expected to be selected.");
    static_assert(test_requires_member<type_with_method>::value == 2,
                  "Specialization requiring method expected to be selected.");
    static_assert(test_requires_member<type_with_int_member>::value == 3,
                  "Specialization requiring int member expected to be selected.");

    SUCCEED();
}
