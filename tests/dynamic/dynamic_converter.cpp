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

namespace {

    enum test_enum_t {
        const1 = 1,
        const2
    };

    template<class Exception>
    void
    test_forbidden_casts(const kora::dynamic_t& source) {
        if (!std::is_same<Exception, bool>::value) {
            EXPECT_FALSE(source.convertible_to<bool>());
            EXPECT_THROW(source.to<bool>(), std::bad_cast);
        }

        if ((!std::is_integral<Exception>::value && !std::is_enum<Exception>::value) ||
            std::is_same<Exception, bool>::value)
        {
            EXPECT_FALSE(source.convertible_to<int>());
            EXPECT_THROW(source.to<int>(), std::bad_cast);

            EXPECT_FALSE(source.convertible_to<unsigned int>());
            EXPECT_THROW(source.to<unsigned int>(), std::bad_cast);

            if (!std::is_arithmetic<Exception>::value) {
                EXPECT_FALSE(source.convertible_to<double>());
                EXPECT_THROW(source.to<double>(), std::bad_cast);
            }
        }

        if (!std::is_same<Exception, int>::value && !std::is_enum<Exception>::value) {
            EXPECT_FALSE(source.convertible_to<test_enum_t>());
            EXPECT_THROW(source.to<test_enum_t>(), std::bad_cast);
        }

        if (!std::is_same<Exception, std::string>::value) {
            EXPECT_FALSE(source.convertible_to<std::string>());
            EXPECT_THROW(source.to<std::string>(), std::bad_cast);

            EXPECT_FALSE(source.convertible_to<const char*>());
            EXPECT_THROW(source.to<const char*>(), std::bad_cast);
        }

        if (!std::is_same<Exception, std::vector<int>>::value) {
            EXPECT_FALSE(source.convertible_to<std::vector<int>>());
            EXPECT_THROW(source.to<std::vector<int>>(), std::bad_cast);

            EXPECT_FALSE(source.convertible_to<std::set<int>>());
            EXPECT_THROW(source.to<std::set<int>>(), std::bad_cast);
        }

        if (!std::is_same<Exception, std::tuple<int, std::string>>::value) {
            EXPECT_FALSE((source.convertible_to<std::tuple<int, std::string>>()));
            EXPECT_THROW((source.to<std::tuple<int, std::string>>()), std::bad_cast);
        }

        if (!std::is_same<Exception, kora::dynamic_t::object_t>::value) {
            EXPECT_FALSE(source.convertible_to<kora::dynamic_t::object_t>());
            EXPECT_THROW(source.to<kora::dynamic_t::object_t>(), std::bad_cast);
        }
    }

} // namespace

TEST(DynamicConverter, Null) {
    test_forbidden_casts<kora::dynamic_t::null_t>(kora::dynamic_t());
}

TEST(DynamicConverter, Dynamic) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = "._____.";

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t>());

    auto destination = source.to<kora::dynamic_t>();

    static_assert(
        std::is_same<kora::dynamic_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t."
    );

    ASSERT_TRUE(destination.is_object());
    EXPECT_EQ(1337, destination.as_object()["key1"]);
    EXPECT_EQ("._____.", destination.as_object()["key2"]);
}

TEST(DynamicConverter, DynamicBool) {
    kora::dynamic_t source = true;

    test_forbidden_casts<bool>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::bool_t>());

    auto destination = source.to<kora::dynamic_t::bool_t>();

    static_assert(
        std::is_same<kora::dynamic_t::bool_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t::bool_t."
    );

    EXPECT_EQ(destination, true);
}

TEST(DynamicConverter, Bool) {
    kora::dynamic_t source = true;

    test_forbidden_casts<bool>(source);

    ASSERT_TRUE(source.convertible_to<bool>());

    auto destination = source.to<bool>();

    static_assert(
        std::is_same<bool, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type bool."
    );

    EXPECT_EQ(destination, true);
}

namespace {

    template<class T>
    void
    test_conversion_to_integral(const kora::dynamic_t& source) {
        {
            ASSERT_TRUE(source.convertible_to<kora::dynamic_t::int_t>());

            auto destination = source.to<kora::dynamic_t::int_t>();

            static_assert(
                std::is_same<kora::dynamic_t::int_t, typename std::decay<decltype(destination)>::type>::value,
                "Expected result of type kora::dynamic_t::int_t."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<kora::dynamic_t::uint_t>());

            auto destination = source.to<kora::dynamic_t::uint_t>();

            static_assert(
                std::is_same<kora::dynamic_t::uint_t, typename std::decay<decltype(destination)>::type>::value,
                "Expected result of type kora::dynamic_t::uint_t."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<kora::dynamic_t::double_t>());

            auto destination = source.to<kora::dynamic_t::double_t>();

            static_assert(
                std::is_same<kora::dynamic_t::double_t, typename std::decay<decltype(destination)>::type>::value,
                "Expected result of type kora::dynamic_t::double_t."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<int>());

            auto destination = source.to<int>();

            static_assert(
                std::is_same<int, typename std::decay<decltype(destination)>::type>::value,
                "Expected result of type int."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<unsigned int>());

            auto destination = source.to<unsigned int>();

            static_assert(
                std::is_same<unsigned int, typename std::decay<decltype(destination)>::type>::value,
                "Expected result of type unsigned int."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<double>());

            auto destination = source.to<double>();

            static_assert(
                std::is_same<double, typename std::decay<decltype(destination)>::type>::value,
                "Expected result of type double."
            );

            EXPECT_FLOAT_EQ(1337.0, destination);
        }

        test_forbidden_casts<T>(source);
    }

} // namespace

TEST(DynamicConverter, SignedIntegral) {
    test_conversion_to_integral<int>(kora::dynamic_t::int_t(1337));
}

TEST(DynamicConverter, UnsignedIntegral) {
    test_conversion_to_integral<unsigned int>(kora::dynamic_t::uint_t(1337));
}

TEST(DynamicConverter, SignedIntegralOverflow) {
    kora::dynamic_t source(std::numeric_limits<int64_t>::max());

    ASSERT_FALSE(source.convertible_to<char>());
    EXPECT_THROW(source.to<char>(), kora::bad_numeric_cast_t);

    ASSERT_FALSE(source.convertible_to<uint32_t>());
    EXPECT_THROW(source.to<uint32_t>(), kora::bad_numeric_cast_t);

    ASSERT_FALSE(source.convertible_to<unsigned char>());
    EXPECT_THROW(source.to<unsigned char>(), kora::bad_numeric_cast_t);

    ASSERT_FALSE(source.convertible_to<int32_t>());
    EXPECT_THROW(source.to<int32_t>(), kora::bad_numeric_cast_t);
}

TEST(DynamicConverter, UnsignedIntegralOverflow) {
    kora::dynamic_t source = -1;

    ASSERT_FALSE(source.convertible_to<uint64_t>());
    EXPECT_THROW(source.to<uint64_t>(), kora::bad_numeric_cast_t);
}

TEST(DynamicConverter, DynamicDouble) {
    kora::dynamic_t source = 1337.5;

    test_forbidden_casts<double>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::double_t>());

    auto destination = source.to<kora::dynamic_t::double_t>();

    static_assert(
        std::is_same<kora::dynamic_t::double_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t::double_t."
    );

    EXPECT_EQ(1337.5, destination);
}

TEST(DynamicConverter, Double) {
    kora::dynamic_t source = 1337.5;

    test_forbidden_casts<double>(source);

    ASSERT_TRUE(source.convertible_to<double>());

    auto destination = source.to<double>();

    static_assert(
        std::is_same<double, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type double."
    );

    EXPECT_EQ(1337.5, destination);
}

TEST(DynamicConverter, FloatOverflow) {
    kora::dynamic_t source = std::numeric_limits<double>::max();

    ASSERT_FALSE(source.convertible_to<float>());
    EXPECT_THROW(source.to<float>(), kora::bad_numeric_cast_t);
}

TEST(DynamicConverter, Enum) {
    kora::dynamic_t source = const2;

    test_forbidden_casts<test_enum_t>(source);

    ASSERT_TRUE(source.convertible_to<test_enum_t>());

    auto destination = source.to<test_enum_t>();

    static_assert(
        std::is_same<test_enum_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type test_enum_t."
    );

    EXPECT_EQ(const2, destination);
}

TEST(DynamicConverter, DynamicString) {
    kora::dynamic_t source = "=^_^=";

    test_forbidden_casts<std::string>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::string_t>());

    auto destination = source.to<kora::dynamic_t::string_t>();

    static_assert(
        std::is_same<kora::dynamic_t::string_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t::string_t."
    );

    EXPECT_EQ("=^_^=", destination);
}

TEST(DynamicConverter, StdString) {
    kora::dynamic_t source = "=^_^=";

    test_forbidden_casts<std::string>(source);

    ASSERT_TRUE(source.convertible_to<std::string>());

    auto destination = source.to<std::string>();

    static_assert(
        std::is_same<std::string, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::string."
    );

    EXPECT_EQ("=^_^=", destination);
}

TEST(DynamicConverter, CString) {
    kora::dynamic_t source = "=^_^=";

    test_forbidden_casts<std::string>(source);

    ASSERT_TRUE(source.convertible_to<const char*>());

    auto destination = source.to<const char*>();

    static_assert(
        std::is_same<const char*, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type const char*."
    );

    EXPECT_STREQ("=^_^=", destination);
}

TEST(DynamicConverter, DynamicArray) {
    kora::dynamic_t source = std::vector<int>(3, 4);

    test_forbidden_casts<std::vector<int>>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::array_t>());

    auto destination = source.to<kora::dynamic_t::array_t>();

    static_assert(
        std::is_same<kora::dynamic_t::array_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t::array_t."
    );

    EXPECT_EQ(kora::dynamic_t::array_t(3, 4), destination);
}

TEST(DynamicConverter, VectorInt) {
    kora::dynamic_t source = std::vector<int>(3, 4);

    test_forbidden_casts<std::vector<int>>(source);

    ASSERT_TRUE(source.convertible_to<std::vector<int>>());

    auto destination = source.to<std::vector<int>>();

    static_assert(
        std::is_same<std::vector<int>, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::vector<int>."
    );

    EXPECT_EQ(std::vector<int>(3, 4), destination);
}

TEST(DynamicConverter, SetInt) {
    kora::dynamic_t source = std::vector<int>(3, 4);

    test_forbidden_casts<std::vector<int>>(source);

    ASSERT_TRUE(source.convertible_to<std::set<int>>());

    auto destination = source.to<std::set<int>>();

    static_assert(
        std::is_same<std::set<int>, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::set<int>."
    );

    std::set<int> pattern;
    pattern.insert(4);

    EXPECT_EQ(pattern, destination);
}

TEST(DynamicConverter, Tuple) {
    kora::dynamic_t source = std::tuple<int, std::string>(3, "@_@");

    test_forbidden_casts<std::tuple<int, std::string>>(source);

    ASSERT_TRUE((source.convertible_to<std::tuple<int, std::string>>()));

    auto destination = source.to<std::tuple<int, std::string>>();

    static_assert(
        std::is_same<std::tuple<int, std::string>, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::tuple<int, std::string>."
    );

    EXPECT_EQ((std::tuple<int, std::string>(3, "@_@")), destination);
}

TEST(DynamicConverter, Pair) {
    kora::dynamic_t source = std::tuple<int, std::string>(3, "@_@");

    test_forbidden_casts<std::tuple<int, std::string>>(source);

    ASSERT_TRUE((source.convertible_to<std::pair<int, std::string>>()));

    auto destination = source.to<std::pair<int, std::string>>();

    static_assert(
        std::is_same<std::pair<int, std::string>, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::pair<int, std::string>."
    );

    EXPECT_EQ((std::pair<int, std::string>(3, "@_@")), destination);
}

TEST(DynamicConverter, DynamicObject) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = "._____.";

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::object_t>());

    auto destination = source.to<kora::dynamic_t::object_t>();

    static_assert(
        std::is_same<kora::dynamic_t::object_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t::object_t."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ("._____.", destination["key2"]);
}

TEST(DynamicConverter, MapStringDynamic) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = "._____.";

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE((source.convertible_to<std::map<std::string, kora::dynamic_t>>()));

    auto destination = source.to<std::map<std::string, kora::dynamic_t>>();

    static_assert(
        std::is_same<std::map<std::string, kora::dynamic_t>,
                     typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::map<std::string, kora::dynamic_t>."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ("._____.", destination["key2"]);
}

TEST(DynamicConverter, MapStringInt) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = -5;

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE((source.convertible_to<std::map<std::string, int>>()));

    auto destination = source.to<std::map<std::string, int>>();

    static_assert(
        std::is_same<std::map<std::string, int>,
                     typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::map<std::string, int>."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ(-5, destination["key2"]);
}

TEST(DynamicConverter, UnorderedMapStringInt) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = -5;

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE((source.convertible_to<std::unordered_map<std::string, int>>()));

    auto destination = source.to<std::unordered_map<std::string, int>>();

    static_assert(
        std::is_same<std::unordered_map<std::string, int>,
                     typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::unordered_map<std::string, int>."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ(-5, destination["key2"]);
}
