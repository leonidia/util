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

TEST(Dynamic, AssociatedItems) {
    kora::dynamic_t::bool_t bool_variable;
    (void)bool_variable;
    kora::dynamic_t::int_t int_variable;
    (void)int_variable;
    kora::dynamic_t::uint_t uint_variable;
    (void)uint_variable;
    kora::dynamic_t::double_t double_variable;
    (void)double_variable;
    kora::dynamic_t::string_t string_variable;
    (void)string_variable;
    kora::dynamic_t::array_t array_variable;
    (void)array_variable;
    kora::dynamic_t::null_t null_variable;
    (void)null_variable;
    kora::dynamic_t::object_t object_variable;
    (void)object_variable;

    const kora::dynamic_t &null_reference = kora::dynamic_t::null;
    (void)null_reference;
    const kora::dynamic_t &empty_string_reference = kora::dynamic_t::empty_string;
    (void)empty_string_reference;
    const kora::dynamic_t &empty_array_reference = kora::dynamic_t::empty_array;
    (void)empty_array_reference;
    const kora::dynamic_t &empty_object_reference = kora::dynamic_t::empty_object;
    (void)empty_object_reference;
}

TEST(Dynamic, DefaultConstructor) {
    kora::dynamic_t default_dynamic;
    EXPECT_TRUE(default_dynamic.is_null());
}

TEST(Dynamic, NullConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::null_t();
    EXPECT_TRUE(dynamic.is_null());
}

TEST(Dynamic, BoolConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::bool_t(true);
    EXPECT_TRUE(dynamic.is_bool());
    EXPECT_EQ(true, dynamic.as_bool());
}

TEST(Dynamic, IntConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::int_t(5);
    EXPECT_TRUE(dynamic.is_int());
    EXPECT_EQ(5, dynamic.as_int());
}

TEST(Dynamic, UintConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::uint_t(5);
    EXPECT_TRUE(dynamic.is_uint());
    EXPECT_EQ(5, dynamic.as_uint());
}

TEST(Dynamic, DoubleConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::double_t(5.0);
    EXPECT_TRUE(dynamic.is_double());
    EXPECT_EQ(5.0, dynamic.as_double());
}

TEST(Dynamic, StringConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::string_t("xdd");
    EXPECT_TRUE(dynamic.is_string());
    EXPECT_EQ("xdd", dynamic.as_string());
}

TEST(Dynamic, ArrayConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::array_t(3, 4);
    EXPECT_TRUE(dynamic.is_array());
    EXPECT_EQ(kora::dynamic_t::array_t(3, 4), dynamic.as_array());
}

TEST(Dynamic, ObjectConstructor) {
    kora::dynamic_t dynamic = kora::dynamic_t::object_t();
    dynamic.as_object()["key"] = 43;

    EXPECT_TRUE(dynamic.is_object());
    EXPECT_EQ(1, dynamic.as_object().count("key"));
    EXPECT_EQ(dynamic.as_object()["key"], 43);
}

TEST(Dynamic, CopyConstructor) {
    {
        kora::dynamic_t dyn1;
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn1.is_null());
        EXPECT_TRUE(dyn2.is_null());
    }
    {
        kora::dynamic_t dyn1(true);
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn1.is_bool());
        EXPECT_TRUE(dyn1.as_bool());
        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::int_t(20));
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
        EXPECT_TRUE(dyn1.is_int());
        EXPECT_EQ(dyn1.as_int(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::uint_t(20));
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
        EXPECT_TRUE(dyn1.is_uint());
        EXPECT_EQ(dyn1.as_uint(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::double_t(20));
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
        EXPECT_TRUE(dyn1.is_double());
        EXPECT_EQ(dyn1.as_double(), 20.0);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::string_t("xd"));
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
        EXPECT_TRUE(dyn1.is_string());
        EXPECT_EQ(dyn1.as_string(), "xd");
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::array_t(3, 4));
        kora::dynamic_t dyn2(dyn1);

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
        EXPECT_TRUE(dyn1.is_array());
        EXPECT_EQ(dyn1.as_array().size(), 3);
        EXPECT_EQ(dyn1.as_array()[0], 4);
    }
    {
        kora::dynamic_t dyn1 = kora::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        kora::dynamic_t dyn2(dyn1);
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
        kora::dynamic_t dyn1;
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_null());
    }
    {
        kora::dynamic_t dyn1(true);
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::int_t(20));
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::uint_t(20));
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::double_t(20));
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::string_t("xd"));
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::array_t(3, 4));
        kora::dynamic_t dyn2(std::move(dyn1));

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
    }
    {
        kora::dynamic_t dyn1 = kora::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        kora::dynamic_t dyn2(std::move(dyn1));
        EXPECT_TRUE(dyn2.is_object());
        EXPECT_EQ(dyn2.as_object().size(), 1);
        EXPECT_EQ(dyn2.as_object()["key"], 42);
    }
}

TEST(Dynamic, NullAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::null_t();
    EXPECT_TRUE(dynamic.is_null());
}

TEST(Dynamic, BoolAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::bool_t(true);
    EXPECT_TRUE(dynamic.is_bool());
    EXPECT_EQ(true, dynamic.as_bool());
}

TEST(Dynamic, IntAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::int_t(5);
    EXPECT_TRUE(dynamic.is_int());
    EXPECT_EQ(5, dynamic.as_int());
}

TEST(Dynamic, UintAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::uint_t(5);
    EXPECT_TRUE(dynamic.is_uint());
    EXPECT_EQ(5, dynamic.as_uint());
}

TEST(Dynamic, DoubleAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::double_t(5.0);
    EXPECT_TRUE(dynamic.is_double());
    EXPECT_EQ(5.0, dynamic.as_double());
}

TEST(Dynamic, StringAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::string_t("xdd");
    EXPECT_TRUE(dynamic.is_string());
    EXPECT_EQ("xdd", dynamic.as_string());
}

TEST(Dynamic, ArrayAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::array_t(3, 4);
    EXPECT_TRUE(dynamic.is_array());
    EXPECT_EQ(kora::dynamic_t::array_t(3, 4), dynamic.as_array());
}

TEST(Dynamic, ObjectAssignment) {
    kora::dynamic_t dynamic;
    dynamic = kora::dynamic_t::object_t();
    dynamic.as_object()["key"] = 43;

    EXPECT_TRUE(dynamic.is_object());
    EXPECT_EQ(1, dynamic.as_object().count("key"));
    EXPECT_EQ(dynamic.as_object()["key"], 43);
}

TEST(Dynamic, CopyAssignment) {
    {
        kora::dynamic_t dyn1;
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn1.is_null());
        EXPECT_TRUE(dyn2.is_null());
    }
    {
        kora::dynamic_t dyn1(true);
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn1.is_bool());
        EXPECT_TRUE(dyn1.as_bool());
        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::int_t(20));
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
        EXPECT_TRUE(dyn1.is_int());
        EXPECT_EQ(dyn1.as_int(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::uint_t(20));
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
        EXPECT_TRUE(dyn1.is_uint());
        EXPECT_EQ(dyn1.as_uint(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::double_t(20));
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
        EXPECT_TRUE(dyn1.is_double());
        EXPECT_EQ(dyn1.as_double(), 20.0);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::string_t("xd"));
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
        EXPECT_TRUE(dyn1.is_string());
        EXPECT_EQ(dyn1.as_string(), "xd");
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::array_t(3, 4));
        kora::dynamic_t dyn2;
        dyn2 = dyn1;

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
        EXPECT_TRUE(dyn1.is_array());
        EXPECT_EQ(dyn1.as_array().size(), 3);
        EXPECT_EQ(dyn1.as_array()[0], 4);
    }
    {
        kora::dynamic_t dyn1 = kora::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        kora::dynamic_t dyn2;
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
        kora::dynamic_t dyn1;
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_null());
    }
    {
        kora::dynamic_t dyn1(true);
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_bool());
        EXPECT_TRUE(dyn2.as_bool());
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::int_t(20));
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_int());
        EXPECT_EQ(dyn2.as_int(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::uint_t(20));
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_uint());
        EXPECT_EQ(dyn2.as_uint(), 20);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::double_t(20));
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_double());
        EXPECT_EQ(dyn2.as_double(), 20.0);
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::string_t("xd"));
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_string());
        EXPECT_EQ(dyn2.as_string(), "xd");
    }
    {
        kora::dynamic_t dyn1(kora::dynamic_t::array_t(3, 4));
        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_array());
        EXPECT_EQ(dyn2.as_array().size(), 3);
        EXPECT_EQ(dyn2.as_array()[0], 4);
    }
    {
        kora::dynamic_t dyn1 = kora::dynamic_t::object_t();
        dyn1.as_object()["key"] = 42;

        kora::dynamic_t dyn2;
        dyn2 = std::move(dyn1);

        EXPECT_TRUE(dyn2.is_object());
        EXPECT_EQ(dyn2.as_object().size(), 1);
        EXPECT_EQ(dyn2.as_object()["key"], 42);
    }
}

namespace {
    void
    test_copy_equality(const kora::dynamic_t& original) {
        kora::dynamic_t copy(original);
        EXPECT_TRUE(original == copy);
    }
} // namespace

TEST(Dynamic, EqualityAfterCopy) {
    {
        SCOPED_TRACE("Null");
        test_copy_equality(kora::dynamic_t());
    }
    {
        SCOPED_TRACE("Bool");
        test_copy_equality(false);
    }
    {
        SCOPED_TRACE("Int");
        test_copy_equality(kora::dynamic_t::int_t(20));
    }
    {
        SCOPED_TRACE("Uint");
        test_copy_equality(kora::dynamic_t::uint_t(20));
    }
    {
        SCOPED_TRACE("Double");
        test_copy_equality(kora::dynamic_t::double_t(20));
    }
    {
        SCOPED_TRACE("String");
        test_copy_equality(kora::dynamic_t::string_t("xd"));
    }
    {
        SCOPED_TRACE("Array");
        test_copy_equality(kora::dynamic_t::array_t(3, 4));
    }
    {
        SCOPED_TRACE("Object");
        kora::dynamic_t dynamic = kora::dynamic_t::object_t();
        dynamic.as_object()["key"] = 42;

        test_copy_equality(dynamic);
    }
}

namespace {

    void
    basic_equality_checks(const kora::dynamic_t& dynamic) {
        EXPECT_FALSE(dynamic == false);
        EXPECT_FALSE(dynamic == kora::dynamic_t::int_t(20));
        EXPECT_FALSE(dynamic == kora::dynamic_t::uint_t(20));
        EXPECT_FALSE(dynamic == kora::dynamic_t::double_t(20.7));
        EXPECT_FALSE(dynamic == kora::dynamic_t::string_t("._______."));
        EXPECT_FALSE(dynamic == kora::dynamic_t::array_t());
        EXPECT_FALSE(dynamic == kora::dynamic_t::object_t());
    }

    void
    basic_equality_checks_nonnull(const kora::dynamic_t& dynamic) {
        EXPECT_FALSE(dynamic == kora::dynamic_t::null);
        basic_equality_checks(dynamic);
    }

} // namespace

TEST(Dynamic, EqualityNull) {
    kora::dynamic_t dynamic;

    EXPECT_TRUE(dynamic == kora::dynamic_t::null);
    basic_equality_checks(dynamic);
}

TEST(Dynamic, EqualityBool) {
    kora::dynamic_t dynamic(true);

    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == true);
}

TEST(Dynamic, EqualityInt) {
    kora::dynamic_t dynamic(kora::dynamic_t::int_t(21));

    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == kora::dynamic_t::int_t(21));
    EXPECT_TRUE(dynamic == kora::dynamic_t::uint_t(21));
    EXPECT_TRUE(dynamic == kora::dynamic_t::double_t(21));
}

TEST(Dynamic, EqualityUint) {
    kora::dynamic_t dynamic(kora::dynamic_t::uint_t(21));

    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == kora::dynamic_t::int_t(21));
    EXPECT_TRUE(dynamic == kora::dynamic_t::uint_t(21));
    EXPECT_TRUE(dynamic == kora::dynamic_t::double_t(21));
}

TEST(Dynamic, EqualityDouble) {
    kora::dynamic_t dynamic(kora::dynamic_t::double_t(21));

    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == kora::dynamic_t::int_t(21));
    EXPECT_TRUE(dynamic == kora::dynamic_t::uint_t(21));
    EXPECT_TRUE(dynamic == kora::dynamic_t::double_t(21));
}

TEST(Dynamic, EqualityString) {
    kora::dynamic_t dynamic(kora::dynamic_t::string_t("xd"));

    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == kora::dynamic_t::string_t("xd"));
}

TEST(Dynamic, EqualityArray) {
    kora::dynamic_t dynamic(kora::dynamic_t::array_t(3, 4));

    basic_equality_checks_nonnull(dynamic);
    EXPECT_TRUE(dynamic == kora::dynamic_t::array_t(3, 4));
}

TEST(Dynamic, EqualityObject) {
    kora::dynamic_t dynamic = kora::dynamic_t::object_t();
    dynamic.as_object()["key"] = 42;

    basic_equality_checks_nonnull(dynamic);
}

namespace {

    void
    basic_inequality_checks(const kora::dynamic_t& dynamic) {
        EXPECT_TRUE(dynamic != false);
        EXPECT_TRUE(dynamic != kora::dynamic_t::int_t(20));
        EXPECT_TRUE(dynamic != kora::dynamic_t::uint_t(20));
        EXPECT_TRUE(dynamic != kora::dynamic_t::double_t(20.7));
        EXPECT_TRUE(dynamic != kora::dynamic_t::string_t("._______."));
        EXPECT_TRUE(dynamic != kora::dynamic_t::array_t());
        EXPECT_TRUE(dynamic != kora::dynamic_t::object_t());
    }

    void
    basic_inequality_checks_nonnull(const kora::dynamic_t& dynamic) {
        EXPECT_TRUE(dynamic != kora::dynamic_t::null);
        basic_inequality_checks(dynamic);
    }

} // namespace

TEST(Dynamic, InequalityNull) {
    kora::dynamic_t dynamic;

    EXPECT_FALSE(dynamic != kora::dynamic_t::null);
    basic_inequality_checks(dynamic);
}

TEST(Dynamic, InequalityBool) {
    kora::dynamic_t dynamic(true);

    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != true);
}

TEST(Dynamic, InequalityInt) {
    kora::dynamic_t dynamic(kora::dynamic_t::int_t(21));

    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != kora::dynamic_t::int_t(21));
    EXPECT_FALSE(dynamic != kora::dynamic_t::uint_t(21));
    EXPECT_FALSE(dynamic != kora::dynamic_t::double_t(21));
}

TEST(Dynamic, InequalityUint) {
    kora::dynamic_t dynamic(kora::dynamic_t::uint_t(21));

    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != kora::dynamic_t::int_t(21));
    EXPECT_FALSE(dynamic != kora::dynamic_t::uint_t(21));
    EXPECT_FALSE(dynamic != kora::dynamic_t::double_t(21));
}

TEST(Dynamic, InequalityDouble) {
    kora::dynamic_t dynamic(kora::dynamic_t::double_t(21));

    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != kora::dynamic_t::int_t(21));
    EXPECT_FALSE(dynamic != kora::dynamic_t::uint_t(21));
    EXPECT_FALSE(dynamic != kora::dynamic_t::double_t(21));
}

TEST(Dynamic, InequalityString) {
    kora::dynamic_t dynamic(kora::dynamic_t::string_t("xd"));

    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != kora::dynamic_t::string_t("xd"));
}

TEST(Dynamic, InequalityArray) {
    kora::dynamic_t dynamic(kora::dynamic_t::array_t(3, 4));

    basic_inequality_checks_nonnull(dynamic);
    EXPECT_FALSE(dynamic != kora::dynamic_t::array_t(3, 4));
}

TEST(Dynamic, InequalityObject) {
    kora::dynamic_t dynamic = kora::dynamic_t::object_t();
    dynamic.as_object()["key"] = 42;

    basic_inequality_checks_nonnull(dynamic);
}

namespace {

    template<class Expected>
    struct type_tester_t:
        public boost::static_visitor<>
    {
        void
        operator()(Expected&) const {
            SUCCEED();
        }

        template<class T>
        void
        operator()(T&&) const {
            FAIL();
        }
    };

    template<class Expected>
    void
    test_apply_types(kora::dynamic_t &dynamic) {
        // At the moment, const and non-const apply's are different methods, so I test both.
        // And there is some template magic, so it's better to test apply with all types
        // of a visitor reference.

        // Rvalue visitor.
        dynamic.apply(type_tester_t<Expected>());
        const_cast<const kora::dynamic_t&>(dynamic).apply(type_tester_t<const Expected>());

        // Non-const visitor.
        type_tester_t<Expected> tester;
        dynamic.apply(tester);

        type_tester_t<const Expected> tester_const;
        const_cast<const kora::dynamic_t&>(dynamic).apply(tester_const);

        // Const visitor.
        dynamic.apply(const_cast<const type_tester_t<Expected>&>(tester));
        const_cast<const kora::dynamic_t&>(dynamic).apply(
            const_cast<const type_tester_t<const Expected>&>(tester_const)
        );
    }

    struct values_tester_t:
        public boost::static_visitor<>
    {
        void
        operator()(const kora::dynamic_t::null_t&) const {
            // Null is always null. Nothing to test.
            SUCCEED();
        }

        void
        operator()(const kora::dynamic_t::bool_t& v) const {
            EXPECT_EQ(true, v);
        }

        void
        operator()(const kora::dynamic_t::int_t& v) const {
            EXPECT_EQ(20, v);
        }

        void
        operator()(const kora::dynamic_t::uint_t& v) const {
            EXPECT_EQ(20, v);
        }

        void
        operator()(const kora::dynamic_t::double_t& v) const {
            EXPECT_EQ(20, v);
        }

        void
        operator()(const kora::dynamic_t::string_t& v) const {
            EXPECT_EQ("xd", v);
        }

        void
        operator()(const kora::dynamic_t::array_t& v) const {
            EXPECT_EQ(kora::dynamic_t::array_t(3, 4), v);
        }

        void
        operator()(const kora::dynamic_t::object_t& v) const {
            EXPECT_EQ(1, v.size());
            EXPECT_EQ(v["key"], 20);
        }
    };

    void
    test_apply_values(kora::dynamic_t &dynamic) {
        // At the moment, const and non-const apply's are different methods, so I test both.
        // And there is some template magic, so it's better to test apply with all types
        // of a visitor reference.

        // Rvalue visitor.
        dynamic.apply(values_tester_t());
        const_cast<const kora::dynamic_t&>(dynamic).apply(values_tester_t());

        // Non-const visitor.
        values_tester_t tester;
        dynamic.apply(tester);
        const_cast<const kora::dynamic_t&>(dynamic).apply(tester);

        // Const visitor.
        dynamic.apply(const_cast<const values_tester_t&>(tester));
        const_cast<const kora::dynamic_t&>(dynamic).apply(
            const_cast<const values_tester_t&>(tester)
        );
    }

    struct mutate_tester_t:
        public boost::static_visitor<>
    {
        void
        operator()(kora::dynamic_t::null_t& v) const {
            v = kora::dynamic_t::null_t();
        }

        void
        operator()(kora::dynamic_t::bool_t& v) const {
            v = false;
        }

        void
        operator()(kora::dynamic_t::int_t& v) const {
            v = 18;
        }

        void
        operator()(kora::dynamic_t::uint_t& v) const {
            v = 18;
        }

        void
        operator()(kora::dynamic_t::double_t& v) const {
            v = 18;
        }

        void
        operator()(kora::dynamic_t::string_t& v) const {
            v = "-_-";
        }

        void
        operator()(kora::dynamic_t::array_t& v) const {
            v = kora::dynamic_t::array_t(2, 8);
        }

        void
        operator()(kora::dynamic_t::object_t& v) const {
            v["key2"] = 18;
        }
    };

    void
    test_apply_mutate(kora::dynamic_t &dynamic) {
        dynamic.apply(mutate_tester_t());

        mutate_tester_t tester;
        dynamic.apply(tester);
        dynamic.apply(const_cast<const mutate_tester_t&>(tester));
    }

    struct nonconst_visitor_t:
        public boost::static_visitor<>
    {
        template<class T>
        void
        operator()(T&&) {
            SUCCEED();
        }
    };

} // namespace

TEST(Dynamic, ApplyToNull) {
    kora::dynamic_t dynamic;
    test_apply_types<kora::dynamic_t::null_t>(dynamic);
    test_apply_values(dynamic);
    test_apply_mutate(dynamic);
}

TEST(Dynamic, ApplyToBool) {
    kora::dynamic_t dynamic(true);
    test_apply_types<kora::dynamic_t::bool_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic, false);
}

TEST(Dynamic, ApplyToInt) {
    kora::dynamic_t dynamic(kora::dynamic_t::int_t(20));
    test_apply_types<kora::dynamic_t::int_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic, kora::dynamic_t::int_t(18));
}

TEST(Dynamic, ApplyToUint) {
    kora::dynamic_t dynamic(kora::dynamic_t::uint_t(20));
    test_apply_types<kora::dynamic_t::uint_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic, kora::dynamic_t::uint_t(18));
}

TEST(Dynamic, ApplyToDouble) {
    kora::dynamic_t dynamic(kora::dynamic_t::double_t(20));
    test_apply_types<kora::dynamic_t::double_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic, kora::dynamic_t::double_t(18));
}

TEST(Dynamic, ApplyToString) {
    kora::dynamic_t dynamic("xd");
    test_apply_types<kora::dynamic_t::string_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic, "-_-");
}

TEST(Dynamic, ApplyToArray) {
    kora::dynamic_t dynamic(kora::dynamic_t::array_t(3, 4));
    test_apply_types<kora::dynamic_t::array_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic, kora::dynamic_t::array_t(2, 8));
}

TEST(Dynamic, ApplyToObject) {
    kora::dynamic_t dynamic = kora::dynamic_t::object_t();
    dynamic.as_object()["key"] = 20;
    test_apply_types<kora::dynamic_t::object_t>(dynamic);
    test_apply_values(dynamic);

    test_apply_mutate(dynamic);
    EXPECT_EQ(dynamic.as_object()["key2"], 18);
}

TEST(Dynamic, ApplyNonConstVisitor) {
    kora::dynamic_t dynamic;
    nonconst_visitor_t visitor;

    dynamic.apply(visitor);
}
