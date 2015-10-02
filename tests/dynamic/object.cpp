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

TEST(DynamicObject, Constructors) {
    kora::dynamic_t::object_t obj1;
    EXPECT_TRUE(obj1.empty());

    obj1["key"] = 5;

    kora::dynamic_t::object_t obj2(obj1);
    EXPECT_EQ(1, obj2.count("key"));
    EXPECT_EQ(5, obj2.at("key"));

    kora::dynamic_t::object_t obj3(std::move(obj2));
    EXPECT_EQ(1, obj3.count("key"));
    EXPECT_EQ(5, obj3.at("key"));

    std::map<std::string, kora::dynamic_t> map;
    map["key2"] = 6;

    kora::dynamic_t::object_t obj4(map);
    EXPECT_EQ(1, obj4.count("key2"));
    EXPECT_EQ(6, obj4.at("key2"));

    kora::dynamic_t::object_t obj5(std::move(map));
    EXPECT_EQ(1, obj5.count("key2"));
    EXPECT_EQ(6, obj5.at("key2"));

    kora::dynamic_t::object_t obj6(obj5.begin(), obj5.end());
    EXPECT_EQ(1, obj6.size());
    EXPECT_EQ(1, obj6.count("key2"));
    EXPECT_EQ(6, obj6.at("key2"));
}

TEST(DynamicObject, Assignment) {
    kora::dynamic_t::object_t obj1;
    EXPECT_TRUE(obj1.empty());

    obj1["key"] = 5;

    kora::dynamic_t::object_t obj2;
    EXPECT_TRUE(obj2.empty());

    obj2 = obj1;
    EXPECT_EQ(1, obj2.count("key"));
    EXPECT_EQ(5, obj2.at("key"));

    kora::dynamic_t::object_t obj3;
    EXPECT_TRUE(obj3.empty());

    obj3 = obj1;
    EXPECT_EQ(1, obj3.count("key"));
    EXPECT_EQ(5, obj3.at("key"));
}

TEST(DynamicObject, Indexing) {
    kora::dynamic_t::object_t obj1;
    obj1["key"] = 5;

    kora::dynamic_t dynamic;

    obj1.at("key2", dynamic) = 42.0;

    EXPECT_TRUE(dynamic.is_double());
    EXPECT_EQ(42.0, dynamic);

    EXPECT_EQ(5, obj1.at("key", dynamic));

    const kora::dynamic_t::object_t& const_obj = obj1;
    const kora::dynamic_t& const_dynamic = dynamic;

    EXPECT_EQ(5, obj1.at("key", const_dynamic));
    EXPECT_EQ(42.0, obj1.at("key2", const_dynamic));
    EXPECT_EQ(5, const_obj.at("key", const_dynamic));
    EXPECT_EQ(42.0, const_obj.at("key2", const_dynamic));

    EXPECT_EQ(5, const_obj["key"]);
    ASSERT_THROW(const_obj["key2"], std::out_of_range);
}

TEST(DynamicObject, AtRvalue) {
    kora::dynamic_t::object_t obj1;
    obj1["key"] = 5;

    const auto& obj2 = obj1.at("unknown", kora::dynamic_t::object_t());

    EXPECT_EQ(kora::dynamic_t::object_t(), obj2);
}
