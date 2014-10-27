/*
Copyright (c) 2014 Andrey Goryachev <andrey.goryachev@gmail.com>
Copyright (c) 2014 Ruslan Nigmatullin <euroelessar@yandex.ru>
Copyright (c) 2011-2014 Other contributors as noted in the AUTHORS file.

This file is part of Leonidia.

Leonidia is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Leonidia is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include "leonidia/dynamic.hpp"

#include <limits>

TEST(DynamicConstructor, UnsignedNumbers) {
    {
        leonidia::dynamic_t constructed = (unsigned char)13;
        EXPECT_TRUE(constructed.is_uint());
        EXPECT_EQ(13, constructed.as_uint());

        leonidia::dynamic_t assigned;
        assigned = (unsigned char)13;
        EXPECT_TRUE(assigned.is_uint());
        EXPECT_EQ(13, assigned.as_uint());
    }
    {
        leonidia::dynamic_t constructed = (unsigned short)13;
        EXPECT_TRUE(constructed.is_uint());
        EXPECT_EQ(13, constructed.as_uint());

        leonidia::dynamic_t assigned;
        assigned = (unsigned short)13;
        EXPECT_TRUE(assigned.is_uint());
        EXPECT_EQ(13, assigned.as_uint());
    }
    {
        leonidia::dynamic_t constructed = (unsigned int)13;
        EXPECT_TRUE(constructed.is_uint());
        EXPECT_EQ(13, constructed.as_uint());

        leonidia::dynamic_t assigned;
        assigned = (unsigned int)13;
        EXPECT_TRUE(assigned.is_uint());
        EXPECT_EQ(13, assigned.as_uint());
    }
}

TEST(DynamicConstructor, SignedNumbers) {
    {
        leonidia::dynamic_t constructed = (char)-13;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(-13, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = (char)-13;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(-13, assigned.as_int());
    }
    {
        leonidia::dynamic_t constructed = (short)-13;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(-13, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = (short)-13;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(-13, assigned.as_int());
    }
    {
        leonidia::dynamic_t constructed = (int)-13;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(-13, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = (int)-13;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(-13, assigned.as_int());
    }
}

TEST(DynamicConstructor, Enum) {
    {
        enum enum1_t {
            const1 = 1,
            const2,
            const3
        };

        leonidia::dynamic_t constructed = const1;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(1, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = const2;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(2, assigned.as_int());
    }
}

TEST(DynamicConstructor, Float) {
    {
        leonidia::dynamic_t constructed = (float)2.0;
        EXPECT_TRUE(constructed.is_double());
        EXPECT_DOUBLE_EQ(2.0, constructed.as_double());

        leonidia::dynamic_t assigned;
        assigned = (float)(-2.0);
        EXPECT_TRUE(assigned.is_double());
        EXPECT_DOUBLE_EQ(-2.0, assigned.as_double());
    }
    {
        leonidia::dynamic_t constructed = (long double)2.0;
        EXPECT_TRUE(constructed.is_double());
        EXPECT_DOUBLE_EQ(2.0, constructed.as_double());

        leonidia::dynamic_t assigned;
        assigned = (long double)(-2.0);
        EXPECT_TRUE(assigned.is_double());
        EXPECT_DOUBLE_EQ(-2.0, assigned.as_double());
    }
}

TEST(DynamicConstructor, FloatOverflow) {
    if (std::numeric_limits<long double>::max() > std::numeric_limits<double>::max()) {
        EXPECT_THROW(leonidia::dynamic_t dynamic = std::numeric_limits<long double>::max(),
                     leonidia::bad_numeric_cast_t);

        leonidia::dynamic_t assigned;
        EXPECT_THROW(assigned = std::numeric_limits<long double>::max(),
                     leonidia::bad_numeric_cast_t);
    } else {
        std::cout << "NOTICE: FloatOverflow test is not performed because long double is equal to double type."
                  << std::endl;
        SUCCEED();
    }
}

TEST(DynamicConstructor, StringLiteral) {
    leonidia::dynamic_t constructed = "xdd";
    EXPECT_TRUE(constructed.is_string());
    EXPECT_EQ("xdd", constructed.as_string());

    leonidia::dynamic_t assigned;
    assigned = "xdd";
    EXPECT_TRUE(assigned.is_string());
    EXPECT_EQ("xdd", assigned.as_string());
}

TEST(DynamicConstructor, Vector) {
    leonidia::dynamic_t constructed = std::vector<bool>(3, true);
    EXPECT_TRUE(constructed.is_array());
    EXPECT_EQ(leonidia::dynamic_t::array_t(3, leonidia::dynamic_t::bool_t(true)), constructed.as_array());

    leonidia::dynamic_t assigned;
    assigned = std::vector<bool>(3, true);
    EXPECT_TRUE(assigned.is_array());
    EXPECT_EQ(leonidia::dynamic_t::array_t(3, leonidia::dynamic_t::bool_t(true)), assigned.as_array());
}

TEST(DynamicConstructor, Tuple) {
    leonidia::dynamic_t::array_t pattern;
    pattern.push_back(leonidia::dynamic_t::null);
    pattern.push_back(leonidia::dynamic_t::bool_t(true));
    pattern.push_back(leonidia::dynamic_t::int_t(10));
    pattern.push_back(leonidia::dynamic_t::string_t("xd"));

    leonidia::dynamic_t constructed = std::make_tuple(leonidia::dynamic_t::null, true, 10, "xd");
    EXPECT_TRUE(constructed.is_array());
    EXPECT_EQ(pattern, constructed.as_array());

    leonidia::dynamic_t assigned;
    assigned = std::make_tuple(leonidia::dynamic_t::null, true, 10, "xd");
    EXPECT_TRUE(assigned.is_array());
    EXPECT_EQ(pattern, assigned.as_array());
}

TEST(DynamicConstructor, MapStringDynamic) {
    std::map<std::string, leonidia::dynamic_t> dynamic_map;
    dynamic_map["key1"] = true;
    dynamic_map["key2"] = 10;

    leonidia::dynamic_t constructed = dynamic_map;
    EXPECT_TRUE(constructed.is_object());
    EXPECT_EQ(dynamic_map, constructed.as_object());

    leonidia::dynamic_t assigned;
    assigned = dynamic_map;
    EXPECT_TRUE(assigned.is_object());
    EXPECT_EQ(dynamic_map, assigned.as_object());
}

TEST(DynamicConstructor, Map) {
    std::map<std::string, bool> map;
    map["key1"] = true;
    map["key2"] = false;

    leonidia::dynamic_t constructed = map;
    EXPECT_TRUE(constructed.is_object());
    EXPECT_EQ(2, constructed.as_object().size());
    EXPECT_EQ(true, constructed.as_object()["key1"].as_bool());
    EXPECT_EQ(false, constructed.as_object()["key2"].as_bool());

    leonidia::dynamic_t assigned;
    assigned = map;
    EXPECT_TRUE(assigned.is_object());
    EXPECT_EQ(2, assigned.as_object().size());
    EXPECT_EQ(true, assigned.as_object()["key1"].as_bool());
    EXPECT_EQ(false, assigned.as_object()["key2"].as_bool());
}

TEST(DynamicConstructor, UnorderedMap) {
    std::unordered_map<std::string, bool> map;
    map["key1"] = true;
    map["key2"] = false;

    leonidia::dynamic_t constructed = map;
    EXPECT_TRUE(constructed.is_object());
    EXPECT_EQ(2, constructed.as_object().size());
    EXPECT_EQ(true, constructed.as_object()["key1"].as_bool());
    EXPECT_EQ(false, constructed.as_object()["key2"].as_bool());

    leonidia::dynamic_t assigned;
    assigned = map;
    EXPECT_TRUE(assigned.is_object());
    EXPECT_EQ(2, assigned.as_object().size());
    EXPECT_EQ(true, assigned.as_object()["key1"].as_bool());
    EXPECT_EQ(false, assigned.as_object()["key2"].as_bool());
}
