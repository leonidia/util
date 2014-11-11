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

#include "kora/utility/underlying_type.hpp"

#ifdef KORA_NOT_BAD

namespace {

enum signed_enum_t {
    signed_const1 = -1,
    signed_const2 = 1
};

enum class signed_enum_class_t {
    const1 = -1,
    const2 = 1
};

enum unsigned_enum_t: unsigned short {
    unsigned_const1 = 0,
    unsigned_const2 = 1
};

enum class unsigned_enum_class_t: unsigned short {
    const1 = 0,
    const2 = 1
};

}

TEST(UnderlyingType, SignedEnum) {
    typedef kora::underlying_type<signed_enum_t>::type underlying;

    static_assert(std::is_integral<underlying>::value, "The underlying type expected to be integral.");
    static_assert(std::is_signed<underlying>::value, "The underlying type expected to be signed.");

    EXPECT_EQ(-1, static_cast<underlying>(signed_const1));
    EXPECT_EQ(1, static_cast<underlying>(signed_const2));
    EXPECT_TRUE(static_cast<underlying>(signed_const1) < static_cast<underlying>(signed_const2));
}

TEST(UnderlyingType, SignedEnumClass) {
    typedef kora::underlying_type<signed_enum_class_t>::type underlying;

    static_assert(std::is_integral<underlying>::value, "The underlying type expected to be integral.");
    static_assert(std::is_signed<underlying>::value, "The underlying type expected to be signed.");

    EXPECT_EQ(-1, static_cast<underlying>(signed_enum_class_t::const1));
    EXPECT_EQ(1, static_cast<underlying>(signed_enum_class_t::const2));
    EXPECT_TRUE(static_cast<underlying>(signed_enum_class_t::const1) < static_cast<underlying>(signed_enum_class_t::const2));
}

TEST(UnderlyingType, UnsignedEnum) {
    typedef kora::underlying_type<unsigned_enum_t>::type underlying;

    static_assert(std::is_integral<underlying>::value, "The underlying type expected to be integral.");
    static_assert(std::is_unsigned<underlying>::value, "The underlying type expected to be unsigned.");

    EXPECT_EQ(0, static_cast<underlying>(unsigned_const1));
    EXPECT_EQ(1, static_cast<underlying>(unsigned_const2));
    EXPECT_TRUE(static_cast<underlying>(unsigned_const1) < static_cast<underlying>(unsigned_const2));
}

TEST(UnderlyingType, UnsignedEnumClass) {
    typedef kora::underlying_type<unsigned_enum_class_t>::type underlying;

    static_assert(std::is_integral<underlying>::value, "The underlying type expected to be integral.");
    static_assert(std::is_unsigned<underlying>::value, "The underlying type expected to be unsigned.");

    EXPECT_EQ(0, static_cast<underlying>(unsigned_enum_class_t::const1));
    EXPECT_EQ(1, static_cast<underlying>(unsigned_enum_class_t::const2));
    EXPECT_TRUE(static_cast<underlying>(unsigned_enum_class_t::const1) < static_cast<underlying>(unsigned_enum_class_t::const2));
}

TEST(ToUnderlyingType, SignedEnum) {
    typedef decltype(kora::to_underlying_type(signed_const1)) result_type;
    typedef kora::underlying_type<signed_enum_t>::type expected_type;

    static_assert(std::is_same<expected_type, result_type>::value,
                  "Wrong type of the result.");

    EXPECT_EQ(-1, kora::to_underlying_type(signed_const1));
    EXPECT_EQ(1, kora::to_underlying_type(signed_const2));
    EXPECT_TRUE(kora::to_underlying_type(signed_const1) < kora::to_underlying_type(signed_const2));
}

TEST(ToUnderlyingType, SignedEnumClass) {
    typedef decltype(kora::to_underlying_type(signed_enum_class_t::const1)) result_type;
    typedef kora::underlying_type<signed_enum_class_t>::type expected_type;

    static_assert(std::is_same<expected_type, result_type>::value,
                  "Wrong type of the result.");

    EXPECT_EQ(-1, kora::to_underlying_type(signed_enum_class_t::const1));
    EXPECT_EQ(1, kora::to_underlying_type(signed_enum_class_t::const2));
    EXPECT_TRUE(kora::to_underlying_type(signed_enum_class_t::const1) < kora::to_underlying_type(signed_enum_class_t::const2));
}

TEST(ToUnderlyingType, UnsignedEnum) {
    typedef decltype(kora::to_underlying_type(unsigned_const1)) result_type;
    typedef kora::underlying_type<unsigned_enum_t>::type expected_type;

    static_assert(std::is_same<expected_type, result_type>::value,
                  "Wrong type of the result.");

    EXPECT_EQ(0, kora::to_underlying_type(unsigned_const1));
    EXPECT_EQ(1, kora::to_underlying_type(unsigned_const2));
    EXPECT_TRUE(kora::to_underlying_type(unsigned_const1) < kora::to_underlying_type(unsigned_const2));
}

TEST(ToUnderlyingType, UnsignedEnumClass) {
    typedef decltype(kora::to_underlying_type(unsigned_enum_class_t::const1)) result_type;
    typedef kora::underlying_type<unsigned_enum_class_t>::type expected_type;

    static_assert(std::is_same<expected_type, result_type>::value,
                  "Wrong type of the result.");

    EXPECT_EQ(0, kora::to_underlying_type(unsigned_enum_class_t::const1));
    EXPECT_EQ(1, kora::to_underlying_type(unsigned_enum_class_t::const2));
    EXPECT_TRUE(kora::to_underlying_type(unsigned_enum_class_t::const1) < kora::to_underlying_type(unsigned_enum_class_t::const2));
}

#endif // KORA_NOT_BAD
