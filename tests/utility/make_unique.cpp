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

#include <gtest/gtest.h>

#include "kora/utility/make_unique.hpp"
#include "kora/utility/sfinae.hpp"

namespace {

struct test_struct_t {
    test_struct_t() : x(0), y(0) { }
    test_struct_t(int x_) : x(x_), y(0) { }
    test_struct_t(int x_, int y_) : x(x_), y(y_) { }

    int x;
    int y;
};

template<typename T, typename = void>
struct can_create_unique_ptr : std::false_type
{ };

template<typename T>
struct can_create_unique_ptr<T,
    typename kora::requires_type<decltype(kora::make_unique<T>(0))>::type> : std::true_type
{ };

template<typename T>
struct stub {
};

enum unique_ptr_version_tag {
    single_object_version,
    array_version
};

template<typename T>
unique_ptr_version_tag
unique_ptr_version(T& t, stub<decltype(*t)> * = 0) {
    (void) t;
    return single_object_version;
}

template<typename T>
unique_ptr_version_tag
unique_ptr_version(T& t, stub<decltype(t[0])> * = 0) {
    (void) t;
    return array_version;
}

} // namespace

TEST(MakeUnique, TheAbilityToCreate) {
    EXPECT_EQ(can_create_unique_ptr<test_struct_t>::value, true);
    EXPECT_EQ(can_create_unique_ptr<test_struct_t[]>::value, true);
    EXPECT_EQ(can_create_unique_ptr<test_struct_t[5]>::value, false);
}

TEST(MakeUnique, CreatingOfUniquePtr) {
	{
		auto a = kora::make_unique<test_struct_t>();
		EXPECT_EQ(static_cast<bool>(a), true);
	}

	{
		auto a = kora::make_unique<test_struct_t[]>(5);
		EXPECT_EQ(static_cast<bool>(a), true);
	}
}

TEST(MakeUnique, CorrectUniquePtrVersion) {
    {
        auto a = kora::make_unique<test_struct_t>();
        EXPECT_EQ(unique_ptr_version(a), single_object_version);
    }

    {
        auto a = kora::make_unique<test_struct_t[]>(1);
        EXPECT_EQ(unique_ptr_version(a), array_version);
    }
}

TEST(MakeUnique, UniquePtrSingleObjectVersion) {
    {
        auto a = kora::make_unique<test_struct_t>();
        EXPECT_EQ(a->x, 0);
        EXPECT_EQ(a->y, 0);
    }

    {
        auto a = kora::make_unique<test_struct_t>(42);
        EXPECT_EQ(a->x, 42);
        EXPECT_EQ(a->y, 0);
    }

    {
        auto a = kora::make_unique<test_struct_t>(42, 34);
        EXPECT_EQ(a->x, 42);
        EXPECT_EQ(a->y, 34);
    }
}

TEST(MakeUnique, UniquePtrArrayVersion) {
    {
        auto a = kora::make_unique<test_struct_t[]>(5);
        EXPECT_EQ(a[1].x, 0);
    }
}

