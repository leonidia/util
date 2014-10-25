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

TEST(DynamicObject, Constructors) {
    leonidia::dynamic_t::object_t obj1;
    EXPECT_TRUE(obj1.empty());

    obj1["key"] = 5;

    leonidia::dynamic_t::object_t obj2(obj1);
    EXPECT_EQ(1, obj2.count("key"));
    EXPECT_EQ(obj2.at("key"), 5);

    leonidia::dynamic_t::object_t obj3(std::move(obj2));
    EXPECT_EQ(1, obj3.count("key"));
    EXPECT_EQ(obj3.at("key"), 5);

    std::map<std::string, leonidia::dynamic_t> map;
    map["key2"] = 6;

    leonidia::dynamic_t::object_t obj4(map);
    EXPECT_EQ(1, obj4.count("key2"));
    EXPECT_EQ(obj4.at("key2"), 6);

    leonidia::dynamic_t::object_t obj5(std::move(map));
    EXPECT_EQ(1, obj5.count("key2"));
    EXPECT_EQ(obj5.at("key2"), 6);

    leonidia::dynamic_t::object_t obj6(obj5.begin(), obj5.end());
    EXPECT_EQ(1, obj6.size());
    EXPECT_EQ(1, obj6.count("key2"));
    EXPECT_EQ(obj6.at("key2"), 6);
}

TEST(DynamicObject, Assignment) {
    leonidia::dynamic_t::object_t obj1;
    EXPECT_TRUE(obj1.empty());

    obj1["key"] = 5;

    leonidia::dynamic_t::object_t obj2;
    EXPECT_TRUE(obj2.empty());

    obj2 = obj1;
    EXPECT_EQ(1, obj2.count("key"));
    EXPECT_EQ(obj2.at("key"), 5);

    leonidia::dynamic_t::object_t obj3;
    EXPECT_TRUE(obj3.empty());

    obj3 = obj1;
    EXPECT_EQ(1, obj3.count("key"));
    EXPECT_EQ(obj3.at("key"), 5);
}

TEST(DynamicObject, Indexing) {
    leonidia::dynamic_t::object_t obj1;
    obj1["key"] = 5;

    leonidia::dynamic_t dynamic;

    obj1.at("key2", dynamic) = 42.0;

    EXPECT_TRUE(dynamic.is_double());
    EXPECT_EQ(dynamic, 42.0);

    EXPECT_EQ(obj1.at("key", dynamic), 5);

    const leonidia::dynamic_t::object_t& const_obj = obj1;
    const leonidia::dynamic_t& const_dynamic = dynamic;

    EXPECT_EQ(obj1.at("key", const_dynamic), 5);
    EXPECT_EQ(obj1.at("key2", const_dynamic), 42.0);
    EXPECT_EQ(const_obj.at("key", const_dynamic), 5);
    EXPECT_EQ(const_obj.at("key2", const_dynamic), 42.0);

    EXPECT_EQ(const_obj["key"], 5);
    ASSERT_THROW(const_obj["key2"], std::out_of_range);
}

TEST(Dynamic, AssociatedItems) {
    leonidia::dynamic_t::bool_t bool_variable;
    (void)bool_variable;
    leonidia::dynamic_t::int_t int_variable;
    (void)int_variable;
    leonidia::dynamic_t::uint_t uint_variable;
    (void)uint_variable;
    leonidia::dynamic_t::double_t double_variable;
    (void)double_variable;
    leonidia::dynamic_t::string_t string_variable;
    (void)string_variable;
    leonidia::dynamic_t::array_t array_variable;
    (void)array_variable;
    leonidia::dynamic_t::null_t null_variable;
    (void)null_variable;
    leonidia::dynamic_t::object_t object_variable;
    (void)object_variable;
    leonidia::dynamic_t::value_t variant_variable;
    (void)variant_variable;

    const leonidia::dynamic_t &null_reference = leonidia::dynamic_t::null;
    (void)null_reference;
    const leonidia::dynamic_t &empty_string_reference = leonidia::dynamic_t::empty_string;
    (void)empty_string_reference;
    const leonidia::dynamic_t &empty_array_reference = leonidia::dynamic_t::empty_array;
    (void)empty_array_reference;
    const leonidia::dynamic_t &empty_object_reference = leonidia::dynamic_t::empty_object;
    (void)empty_object_reference;
}

TEST(Dynamic, DefaultConstructor) {
    leonidia::dynamic_t dyn1;
    EXPECT_TRUE(dyn1.is_null());

    dyn1 = 20;
    EXPECT_TRUE(dyn1.is_int());

    leonidia::dynamic_t dyn2(dyn1);
    EXPECT_TRUE(dyn2.is_int());
    EXPECT_EQ(dyn2.as_int(), 20);

    leonidia::dynamic_t dyn3(std::move(dyn2));
    EXPECT_TRUE(dyn3.is_int());
    EXPECT_EQ(dyn3.as_int(), 20);
}

TEST(Dynamic, CopyConstructor) {
    {
        leonidia::dynamic_t dyn1;
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn1.is_null());
        EXPECT_TRUE(dyn2.is_null());
    }
    {
        leonidia::dynamic_t dyn1(true);
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn1.is_bool());
        EXPECT_TRUE(dyn1.as_bool());
        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::int_t(20));
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
        EXPECT_TRUE(dyn1.is_int());
        EXPECT_EQ(dyn1.as_int(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::uint_t(20));
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
        EXPECT_TRUE(dyn1.is_uint());
        EXPECT_EQ(dyn1.as_uint(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::double_t(20));
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
        EXPECT_TRUE(dyn1.is_double());
        EXPECT_EQ(dyn1.as_double(), 20.0);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::string_t("xd"));
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
        EXPECT_TRUE(dyn1.is_string());
        EXPECT_EQ(dyn1.as_string(), "xd");
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::array_t(3, 4));
        leonidia::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
        EXPECT_TRUE(dyn1.is_array());
        EXPECT_EQ(dyn1.as_array().size(), 3);
        EXPECT_EQ(dyn1.as_array()[0], 4);
    }
    {
        leonidia::dynamic_t dyn1 = leonidia::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        leonidia::dynamic_t dyn2(dyn1);
        EXPECT_TRUE(dyn2.is_object());
        EXPECT_EQ(dyn2.as_object().size(), 1);
        EXPECT_EQ(dyn2.as_object()["key"], 42);
        EXPECT_TRUE(dyn1.is_object());
        EXPECT_EQ(dyn1.as_object().size(), 1);
        EXPECT_EQ(dyn1.as_object()["key"], 42);
    }
}

TEST(Dynamic, MoveConstructor) {
    {
        leonidia::dynamic_t dyn1;
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_null());
    }
    {
        leonidia::dynamic_t dyn1(true);
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::int_t(20));
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::uint_t(20));
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::double_t(20));
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::string_t("xd"));
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::array_t(3, 4));
        leonidia::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
    }
    {
        leonidia::dynamic_t dyn1 = leonidia::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        leonidia::dynamic_t dyn2(std::move(dyn1));
        EXPECT_TRUE(dyn2.is_object());
        EXPECT_EQ(dyn2.as_object().size(), 1);
        EXPECT_EQ(dyn2.as_object()["key"], 42);
    }
}

TEST(Dynamic, CopyAssignment) {
    {
        leonidia::dynamic_t dyn1;
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn1.is_null());
        EXPECT_TRUE(dyn2.is_null());
    }
    {
        leonidia::dynamic_t dyn1(true);
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn1.is_bool());
        EXPECT_TRUE(dyn1.as_bool());
        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::int_t(20));
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
        EXPECT_TRUE(dyn1.is_int());
        EXPECT_EQ(dyn1.as_int(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::uint_t(20));
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
        EXPECT_TRUE(dyn1.is_uint());
        EXPECT_EQ(dyn1.as_uint(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::double_t(20));
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
        EXPECT_TRUE(dyn1.is_double());
        EXPECT_EQ(dyn1.as_double(), 20.0);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::string_t("xd"));
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
        EXPECT_TRUE(dyn1.is_string());
        EXPECT_EQ(dyn1.as_string(), "xd");
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::array_t(3, 4));
        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
        EXPECT_TRUE(dyn1.is_array());
        EXPECT_EQ(dyn1.as_array().size(), 3);
        EXPECT_EQ(dyn1.as_array()[0], 4);
    }
    {
        leonidia::dynamic_t dyn1 = leonidia::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        leonidia::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_object());
        EXPECT_EQ(dyn2.as_object().size(), 1);
        EXPECT_EQ(dyn2.as_object()["key"], 42);
        EXPECT_TRUE(dyn1.is_object());
        EXPECT_EQ(dyn1.as_object().size(), 1);
        EXPECT_EQ(dyn1.as_object()["key"], 42);
    }
}

TEST(Dynamic, MoveAssignment) {
    {
        leonidia::dynamic_t dyn1;
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_null());
    }
    {
        leonidia::dynamic_t dyn1(true);
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::int_t(20));
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::uint_t(20));
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::double_t(20));
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::string_t("xd"));
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
    }
    {
        leonidia::dynamic_t dyn1(leonidia::dynamic_t::array_t(3, 4));
        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
    }
    {
        leonidia::dynamic_t dyn1 = leonidia::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        leonidia::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_object());
        EXPECT_EQ(dyn2.as_object().size(), 1);
        EXPECT_EQ(dyn2.as_object()["key"], 42);
    }
}

namespace {
    void
    test_copy_equality(const leonidia::dynamic_t& original) {
        leonidia::dynamic_t copy(original);
        EXPECT_TRUE(original == copy);
    }
}

TEST(Dynamic, EqualityAfterCopy) {
    {
        SCOPED_TRACE("Null");
        leonidia::dynamic_t dynamic;
        EXPECT_TRUE(dynamic.is_null());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("Bool");
        leonidia::dynamic_t dynamic(false);
        EXPECT_TRUE(dynamic.is_bool());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("Int");
        leonidia::dynamic_t dynamic(leonidia::dynamic_t::int_t(20));
        EXPECT_TRUE(dynamic.is_int());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("Uint");
        leonidia::dynamic_t dynamic(leonidia::dynamic_t::uint_t(20));
        EXPECT_TRUE(dynamic.is_uint());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("Double");
        leonidia::dynamic_t dynamic(leonidia::dynamic_t::double_t(20));
        EXPECT_TRUE(dynamic.is_double());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("String");
        leonidia::dynamic_t dynamic(leonidia::dynamic_t::string_t("xd"));
        EXPECT_TRUE(dynamic.is_string());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("Array");
        leonidia::dynamic_t dynamic(leonidia::dynamic_t::array_t(3, 4));
        EXPECT_TRUE(dynamic.is_array());
        test_copy_equality(dynamic);
    }
    {
        SCOPED_TRACE("Object");
        leonidia::dynamic_t dynamic = leonidia::dynamic_t::object_t();
        dynamic.as_object()["key"] = 42;

        EXPECT_TRUE(dynamic.is_object());
        test_copy_equality(dynamic);
    }
}

namespace {

    void
    basic_equality_checks(const leonidia::dynamic_t& dynamic) {
        EXPECT_FALSE(dynamic == false);
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::int_t(20));
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::uint_t(20));
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::double_t(20));
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::string_t("._______."));
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::array_t());
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::object_t());
    }

    void
    basic_equality_checks_nonnull(const leonidia::dynamic_t& dynamic) {
        EXPECT_FALSE(dynamic == leonidia::dynamic_t::null);
        basic_equality_checks(dynamic);
    }

}

TEST(Dynamic, EqualityNull) {
    leonidia::dynamic_t dynamic;

    EXPECT_TRUE(dynamic.is_null());
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::null);
    basic_equality_checks(dynamic);
}

TEST(Dynamic, EqualityBool) {
    leonidia::dynamic_t dynamic(true);

    EXPECT_TRUE(dynamic.is_bool());
    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == true);
}

TEST(Dynamic, EqualityInt) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::int_t(21));

    EXPECT_TRUE(dynamic.is_int());
    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::int_t(21));
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::uint_t(21));
    EXPECT_FALSE(dynamic == leonidia::dynamic_t::double_t(21));
}

TEST(Dynamic, EqualityUint) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::uint_t(21));

    EXPECT_TRUE(dynamic.is_uint());
    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::int_t(21));
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::uint_t(21));
    EXPECT_FALSE(dynamic == leonidia::dynamic_t::double_t(21));
}

TEST(Dynamic, EqualityDouble) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::double_t(21));

    EXPECT_TRUE(dynamic.is_double());
    basic_equality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic == leonidia::dynamic_t::int_t(21));
    EXPECT_FALSE(dynamic == leonidia::dynamic_t::uint_t(21));
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::double_t(21));
}

TEST(Dynamic, EqualityString) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::string_t("xd"));

    EXPECT_TRUE(dynamic.is_string());
    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::string_t("xd"));
}

TEST(Dynamic, EqualityArray) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::array_t(3, 4));

    EXPECT_TRUE(dynamic.is_array());
    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == leonidia::dynamic_t::array_t(3, 4));
}

TEST(Dynamic, EqualityObject) {
    leonidia::dynamic_t dynamic = leonidia::dynamic_t::object_t();
    dynamic.as_object()["key"] = 42;

    EXPECT_TRUE(dynamic.is_object());
    basic_equality_checks_nonnull(dynamic);
}

namespace {

    void
    basic_inequality_checks(const leonidia::dynamic_t& dynamic) {
        EXPECT_TRUE(dynamic != false);
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::int_t(20));
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::uint_t(20));
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::double_t(20));
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::string_t("._______."));
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::array_t());
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::object_t());
    }

    void
    basic_inequality_checks_nonnull(const leonidia::dynamic_t& dynamic) {
        EXPECT_TRUE(dynamic != leonidia::dynamic_t::null);
        basic_inequality_checks(dynamic);
    }

}

TEST(Dynamic, InequalityNull) {
    leonidia::dynamic_t dynamic;

    EXPECT_TRUE(dynamic.is_null());
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::null);
    basic_inequality_checks(dynamic);
}

TEST(Dynamic, InequalityBool) {
    leonidia::dynamic_t dynamic(true);

    EXPECT_TRUE(dynamic.is_bool());
    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != true);
}

TEST(Dynamic, InequalityInt) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::int_t(21));

    EXPECT_TRUE(dynamic.is_int());
    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::int_t(21));
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::uint_t(21));
    EXPECT_TRUE(dynamic != leonidia::dynamic_t::double_t(21));
}

TEST(Dynamic, InequalityUint) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::uint_t(21));

    EXPECT_TRUE(dynamic.is_uint());
    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::int_t(21));
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::uint_t(21));
    EXPECT_TRUE(dynamic != leonidia::dynamic_t::double_t(21));
}

TEST(Dynamic, InequalityDouble) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::double_t(21));

    EXPECT_TRUE(dynamic.is_double());
    basic_inequality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic != leonidia::dynamic_t::int_t(21));
    EXPECT_TRUE(dynamic != leonidia::dynamic_t::uint_t(21));
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::double_t(21));
}

TEST(Dynamic, InequalityString) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::string_t("xd"));

    EXPECT_TRUE(dynamic.is_string());
    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::string_t("xd"));
}

TEST(Dynamic, InequalityArray) {
    leonidia::dynamic_t dynamic(leonidia::dynamic_t::array_t(3, 4));

    EXPECT_TRUE(dynamic.is_array());
    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != leonidia::dynamic_t::array_t(3, 4));
}

TEST(Dynamic, InequalityObject) {
    leonidia::dynamic_t dynamic = leonidia::dynamic_t::object_t();
    dynamic.as_object()["key"] = 42;

    EXPECT_TRUE(dynamic.is_object());
    basic_inequality_checks_nonnull(dynamic);
}
