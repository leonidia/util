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

#include "kora/dynamic.hpp"

TEST(DynamicConstructor, UnsignedChar) {
    kora::dynamic_t constructed = (unsigned char)13;
    EXPECT_TRUE(constructed.is_uint());
    EXPECT_EQ(13, constructed.as_uint());

    kora::dynamic_t assigned;
    assigned = (unsigned char)13;
    EXPECT_TRUE(assigned.is_uint());
    EXPECT_EQ(13, assigned.as_uint());
}

TEST(DynamicConstructor, UnsignedShort) {
    kora::dynamic_t constructed = (unsigned short)13;
    EXPECT_TRUE(constructed.is_uint());
    EXPECT_EQ(13, constructed.as_uint());

    kora::dynamic_t assigned;
    assigned = (unsigned short)13;
    EXPECT_TRUE(assigned.is_uint());
    EXPECT_EQ(13, assigned.as_uint());
}

TEST(DynamicConstructor, UnsignedInt) {
    kora::dynamic_t constructed = (unsigned int)13;
    EXPECT_TRUE(constructed.is_uint());
    EXPECT_EQ(13, constructed.as_uint());

    kora::dynamic_t assigned;
    assigned = (unsigned int)13;
    EXPECT_TRUE(assigned.is_uint());
    EXPECT_EQ(13, assigned.as_uint());
}

TEST(DynamicConstructor, SignedChar) {
    kora::dynamic_t constructed = (char)-13;
    EXPECT_TRUE(constructed.is_int());
    EXPECT_EQ(-13, constructed.as_int());

    kora::dynamic_t assigned;
    assigned = (char)-13;
    EXPECT_TRUE(assigned.is_int());
    EXPECT_EQ(-13, assigned.as_int());
}

TEST(DynamicConstructor, SignedShort) {
    kora::dynamic_t constructed = (short)-13;
    EXPECT_TRUE(constructed.is_int());
    EXPECT_EQ(-13, constructed.as_int());

    kora::dynamic_t assigned;
    assigned = (short)-13;
    EXPECT_TRUE(assigned.is_int());
    EXPECT_EQ(-13, assigned.as_int());
}

TEST(DynamicConstructor, SignedInt) {
    kora::dynamic_t constructed = (int)-13;
    EXPECT_TRUE(constructed.is_int());
    EXPECT_EQ(-13, constructed.as_int());

    kora::dynamic_t assigned;
    assigned = (int)-13;
    EXPECT_TRUE(assigned.is_int());
    EXPECT_EQ(-13, assigned.as_int());
}

TEST(DynamicConstructor, Float) {
    kora::dynamic_t constructed = (float)2.0;
    EXPECT_TRUE(constructed.is_double());
    EXPECT_DOUBLE_EQ(2.0, constructed.as_double());

    kora::dynamic_t assigned;
    assigned = (float)(-2.0);
    EXPECT_TRUE(assigned.is_double());
    EXPECT_DOUBLE_EQ(-2.0, assigned.as_double());
}

TEST(DynamicConstructor, StringLiteral) {
    kora::dynamic_t constructed = "xdd";
    EXPECT_TRUE(constructed.is_string());
    EXPECT_EQ("xdd", constructed.as_string());

    kora::dynamic_t assigned;
    assigned = "xdd";
    EXPECT_TRUE(assigned.is_string());
    EXPECT_EQ("xdd", assigned.as_string());
}

TEST(DynamicConstructor, Vector) {
    kora::dynamic_t constructed = std::vector<bool>(3, true);
    EXPECT_TRUE(constructed.is_array());
    EXPECT_EQ(kora::dynamic_t::array_t(3, kora::dynamic_t::bool_t(true)), constructed.as_array());

    kora::dynamic_t assigned;
    assigned = std::vector<bool>(3, true);
    EXPECT_TRUE(assigned.is_array());
    EXPECT_EQ(kora::dynamic_t::array_t(3, kora::dynamic_t::bool_t(true)), assigned.as_array());
}

TEST(DynamicConstructor, Tuple) {
    kora::dynamic_t::array_t pattern;
    pattern.push_back(kora::dynamic_t::null);
    pattern.push_back(kora::dynamic_t::bool_t(true));
    pattern.push_back(kora::dynamic_t::int_t(10));
    pattern.push_back(kora::dynamic_t::string_t("xd"));

    kora::dynamic_t constructed = std::make_tuple(kora::dynamic_t::null, true, 10, "xd");
    EXPECT_TRUE(constructed.is_array());
    EXPECT_EQ(pattern, constructed.as_array());

    kora::dynamic_t assigned;
    assigned = std::make_tuple(kora::dynamic_t::null, true, 10, "xd");
    EXPECT_TRUE(assigned.is_array());
    EXPECT_EQ(pattern, assigned.as_array());
}

TEST(DynamicConstructor, MapStringDynamic) {
    std::map<std::string, kora::dynamic_t> dynamic_map;
    dynamic_map["key1"] = true;
    dynamic_map["key2"] = 10;

    kora::dynamic_t constructed = dynamic_map;
    EXPECT_TRUE(constructed.is_object());
    EXPECT_EQ(dynamic_map, constructed.as_object());

    kora::dynamic_t assigned;
    assigned = dynamic_map;
    EXPECT_TRUE(assigned.is_object());
    EXPECT_EQ(dynamic_map, assigned.as_object());
}

TEST(DynamicConstructor, Map) {
    std::map<std::string, bool> map;
    map["key1"] = true;
    map["key2"] = false;

    kora::dynamic_t constructed = map;
    EXPECT_TRUE(constructed.is_object());
    EXPECT_EQ(2, constructed.as_object().size());
    EXPECT_EQ(constructed.as_object()["key1"].as_bool(), true);
    EXPECT_EQ(constructed.as_object()["key2"].as_bool(), false);

    kora::dynamic_t assigned;
    assigned = map;
    EXPECT_TRUE(assigned.is_object());
    EXPECT_EQ(2, assigned.as_object().size());
    EXPECT_EQ(assigned.as_object()["key1"].as_bool(), true);
    EXPECT_EQ(assigned.as_object()["key2"].as_bool(), false);
}

TEST(DynamicConstructor, UnorderedMap) {
    std::unordered_map<std::string, bool> map;
    map["key1"] = true;
    map["key2"] = false;

    kora::dynamic_t constructed = map;
    EXPECT_TRUE(constructed.is_object());
    EXPECT_EQ(2, constructed.as_object().size());
    EXPECT_EQ(constructed.as_object()["key1"].as_bool(), true);
    EXPECT_EQ(constructed.as_object()["key2"].as_bool(), false);

    kora::dynamic_t assigned;
    assigned = map;
    EXPECT_TRUE(assigned.is_object());
    EXPECT_EQ(2, assigned.as_object().size());
    EXPECT_EQ(assigned.as_object()["key1"].as_bool(), true);
    EXPECT_EQ(assigned.as_object()["key2"].as_bool(), false);
}

namespace {

    struct test_struct_t {
        int a;
        std::string b;
    };

} // namespace

namespace kora { namespace dynamic {

    template<>
    struct constructor<test_struct_t> {
        static const bool enable = true;

        static inline
        void
        convert(const test_struct_t& from, dynamic_t& to) {
            to = std::tuple<int, std::string>(from.a, from.b);
        }
    };

}} // namespace kora::dynamic

TEST(DynamicConstructor, CustomConstructor) {
    kora::dynamic_t constructed = test_struct_t {1337, ">_>"};
    EXPECT_TRUE(constructed.is_array());
    EXPECT_EQ(2, constructed.as_array().size());
    EXPECT_EQ(1337, constructed.as_array()[0].as_int());
    EXPECT_EQ(">_>", constructed.as_array()[1].as_string());

    kora::dynamic_t assigned;
    assigned = test_struct_t {1337, ">_>"};
    EXPECT_TRUE(assigned.is_array());
    EXPECT_EQ(2, assigned.as_array().size());
    EXPECT_EQ(1337, assigned.as_array()[0].as_int());
    EXPECT_EQ(">_>", assigned.as_array()[1].as_string());
}
