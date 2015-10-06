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

    template<class Exception>
    void
    test_forbidden_casts(const kora::dynamic_t& source) {
        if (!std::is_same<Exception, bool>::value) {
            EXPECT_FALSE(source.convertible_to<bool>());
            EXPECT_THROW(source.to<bool>(), std::bad_cast);
        }

        if (!std::is_integral<Exception>::value || std::is_same<Exception, bool>::value) {
            EXPECT_FALSE(source.convertible_to<int>());
            EXPECT_THROW(source.to<int>(), std::bad_cast);

            EXPECT_FALSE(source.convertible_to<unsigned int>());
            EXPECT_THROW(source.to<unsigned int>(), std::bad_cast);

            if (!std::is_arithmetic<Exception>::value) {
                EXPECT_FALSE(source.convertible_to<double>());
                EXPECT_THROW(source.to<double>(), std::bad_cast);
            }
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

namespace {

    kora::dynamic_t
    get_array_of_maps() {
        return std::vector<std::map<std::string, int>>(2);
    }

    kora::dynamic_t
    get_pair_with_first_map() {
        return std::tuple<std::map<std::string, int>, int>(std::map<std::string, int>(), 13);
    }

    kora::dynamic_t
    get_pair_with_second_map() {
        return std::tuple<int, std::map<std::string, int>>(13, std::map<std::string, int>());
    }

    kora::dynamic_t
    get_map_of_arrays() {
        std::map<std::string, std::vector<int>> result;
        result["key"] = std::vector<int>(3, 4);
        return result;
    }

    class conroller_recursive_call_success_t { };

    struct test_array_visiting_t {
        void
        start_array(const kora::dynamic_t&) {
            throw conroller_recursive_call_success_t();
        }

        void
        finish_array() { }

        void
        item(size_t) { }

        void
        start_object(const kora::dynamic_t&) { }

        void
        finish_object() { }

        void
        item(const std::string &) { }

        template<class Exception>
        KORA_NORETURN
        void
        fail(const Exception&, const kora::dynamic_t&) const {
            // FAIL() cannot be used here because it tries to return void (why?)
            // and this whould be bad noreturn function.
            ADD_FAILURE();
            std::terminate();
        }
    };

    struct test_map_visiting_t {
        void
        start_array(const kora::dynamic_t&) { }

        void
        finish_array() { }

        void
        item(size_t) { }

        void
        start_object(const kora::dynamic_t&) {
            throw conroller_recursive_call_success_t();
        }

        void
        finish_object() { }

        void
        item(const std::string &) { }

        template<class Exception>
        KORA_NORETURN
        void
        fail(const Exception&, const kora::dynamic_t&) const {
            // FAIL() cannot be used here because it tries to return void (why?)
            // and this whould be bad noreturn function.
            ADD_FAILURE();
            std::terminate();
        }
    };

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
        std::is_same<kora::dynamic_t, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<kora::dynamic_t::bool_t, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<bool, std::decay<decltype(destination)>::type>::value,
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
                std::is_same<kora::dynamic_t::int_t, std::decay<decltype(destination)>::type>::value,
                "Expected result of type kora::dynamic_t::int_t."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<kora::dynamic_t::uint_t>());

            auto destination = source.to<kora::dynamic_t::uint_t>();

            static_assert(
                std::is_same<kora::dynamic_t::uint_t, std::decay<decltype(destination)>::type>::value,
                "Expected result of type kora::dynamic_t::uint_t."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<kora::dynamic_t::double_t>());

            auto destination = source.to<kora::dynamic_t::double_t>();

            static_assert(
                std::is_same<kora::dynamic_t::double_t, std::decay<decltype(destination)>::type>::value,
                "Expected result of type kora::dynamic_t::double_t."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<int>());

            auto destination = source.to<int>();

            static_assert(
                std::is_same<int, std::decay<decltype(destination)>::type>::value,
                "Expected result of type int."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<unsigned int>());

            auto destination = source.to<unsigned int>();

            static_assert(
                std::is_same<unsigned int, std::decay<decltype(destination)>::type>::value,
                "Expected result of type unsigned int."
            );

            EXPECT_EQ(1337, destination);
        }

        {
            ASSERT_TRUE(source.convertible_to<double>());

            auto destination = source.to<double>();

            static_assert(
                std::is_same<double, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<kora::dynamic_t::double_t, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<double, std::decay<decltype(destination)>::type>::value,
        "Expected result of type double."
    );

    EXPECT_EQ(1337.5, destination);
}

TEST(DynamicConverter, FloatOverflow) {
    kora::dynamic_t source = std::numeric_limits<double>::max();

    ASSERT_FALSE(source.convertible_to<float>());
    EXPECT_THROW(source.to<float>(), kora::bad_numeric_cast_t);
}

TEST(DynamicConverter, DynamicString) {
    kora::dynamic_t source = "=^_^=";

    test_forbidden_casts<std::string>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::string_t>());

    auto destination = source.to<kora::dynamic_t::string_t>();

    static_assert(
        std::is_same<kora::dynamic_t::string_t, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<std::string, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<const char*, std::decay<decltype(destination)>::type>::value,
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
        std::is_same<kora::dynamic_t::array_t, std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t::array_t."
    );

    EXPECT_EQ(kora::dynamic_t::array_t(3, 4), destination);
}

TEST(DynamicConverter, VectorT) {
    kora::dynamic_t source = std::vector<int>(3, 4);

    test_forbidden_casts<std::vector<int>>(source);

    ASSERT_TRUE(source.convertible_to<std::vector<int>>());

    auto destination = source.to<std::vector<int>>();

    static_assert(
        std::is_same<std::vector<int>, std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::vector<int>."
    );

    EXPECT_EQ(std::vector<int>(3, 4), destination);
}

TEST(DynamicConverter, VectorTCallsController) {
    typedef std::vector<std::map<std::string, int>> destination_type;

    EXPECT_THROW(get_array_of_maps().to<destination_type>(test_map_visiting_t()),
                 conroller_recursive_call_success_t);
}

TEST(DynamicConverter, SetT) {
    kora::dynamic_t source = std::vector<int>(3, 4);

    test_forbidden_casts<std::vector<int>>(source);

    ASSERT_TRUE(source.convertible_to<std::set<int>>());

    auto destination = source.to<std::set<int>>();

    static_assert(
        std::is_same<std::set<int>, std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::set<int>."
    );

    std::set<int> pattern;
    pattern.insert(4);

    EXPECT_EQ(pattern, destination);
}

TEST(DynamicConverter, SetTCallsController) {
    typedef std::set<std::map<std::string, int>> destination_type;

    EXPECT_THROW(get_array_of_maps().to<destination_type>(test_map_visiting_t()),
                 conroller_recursive_call_success_t);
}

TEST(DynamicConverter, Tuple) {
    kora::dynamic_t source = std::tuple<int, std::string>(3, "@_@");

    test_forbidden_casts<std::tuple<int, std::string>>(source);

    ASSERT_TRUE((source.convertible_to<std::tuple<int, std::string>>()));

    auto destination = source.to<std::tuple<int, std::string>>();

    static_assert(
        std::is_same<std::tuple<int, std::string>, std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::tuple<int, std::string>."
    );

    EXPECT_EQ((std::tuple<int, std::string>(3, "@_@")), destination);
}

TEST(DynamicConverter, TupleCallsController) {
    {
        typedef std::tuple<std::map<std::string, int>, int> destination_type;

        EXPECT_THROW(get_pair_with_first_map().to<destination_type>(test_map_visiting_t()),
                     conroller_recursive_call_success_t);
    }

    {
        typedef std::tuple<int, std::map<std::string, int>> destination_type;

        EXPECT_THROW(get_pair_with_second_map().to<destination_type>(test_map_visiting_t()),
                     conroller_recursive_call_success_t);
    }
}

TEST(DynamicConverter, Pair) {
    kora::dynamic_t source = std::tuple<int, std::string>(3, "@_@");

    test_forbidden_casts<std::tuple<int, std::string>>(source);

    ASSERT_TRUE((source.convertible_to<std::pair<int, std::string>>()));

    auto destination = source.to<std::pair<int, std::string>>();

    static_assert(
        std::is_same<std::pair<int, std::string>, std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::pair<int, std::string>."
    );

    EXPECT_EQ((std::pair<int, std::string>(3, "@_@")), destination);
}

TEST(DynamicConverter, PairCallsController) {
    {
        typedef std::pair<std::map<std::string, int>, int> destination_type;

        EXPECT_THROW(get_pair_with_first_map().to<destination_type>(test_map_visiting_t()),
                     conroller_recursive_call_success_t);
    }

    {
        typedef std::pair<int, std::map<std::string, int>> destination_type;

        EXPECT_THROW(get_pair_with_second_map().to<destination_type>(test_map_visiting_t()),
                     conroller_recursive_call_success_t);
    }
}

TEST(DynamicConverter, DynamicObject) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = "._____.";

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE(source.convertible_to<kora::dynamic_t::object_t>());

    auto destination = source.to<kora::dynamic_t::object_t>();

    static_assert(
        std::is_same<kora::dynamic_t::object_t, std::decay<decltype(destination)>::type>::value,
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
                     std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::map<std::string, kora::dynamic_t>."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ("._____.", destination["key2"]);
}

TEST(DynamicConverter, MapStringT) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = -5;

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE((source.convertible_to<std::map<std::string, int>>()));

    auto destination = source.to<std::map<std::string, int>>();

    static_assert(
        std::is_same<std::map<std::string, int>,
                     std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::map<std::string, int>."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ(-5, destination["key2"]);
}

TEST(DynamicConverter, MapStringTCallsController) {
    typedef std::map<std::string, std::vector<int>> destination_type;

    EXPECT_THROW(get_map_of_arrays().to<destination_type>(test_array_visiting_t()),
                 conroller_recursive_call_success_t);
}

TEST(DynamicConverter, UnorderedMapStringT) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = -5;

    test_forbidden_casts<kora::dynamic_t::object_t>(source);

    ASSERT_TRUE((source.convertible_to<std::unordered_map<std::string, int>>()));

    auto destination = source.to<std::unordered_map<std::string, int>>();

    static_assert(
        std::is_same<std::unordered_map<std::string, int>,
                     std::decay<decltype(destination)>::type>::value,
        "Expected result of type std::unordered_map<std::string, int>."
    );

    EXPECT_EQ(1337, destination["key1"]);
    EXPECT_EQ(-5, destination["key2"]);
}

TEST(DynamicConverter, UnorderedMapStringTCallsController) {
    typedef std::unordered_map<std::string, std::vector<int>> destination_type;

    EXPECT_THROW(get_map_of_arrays().to<destination_type>(test_array_visiting_t()),
                 conroller_recursive_call_success_t);
}

namespace {

    struct test_tag_t { };

    class test_conversion_exception_t :
        public kora::bad_cast_t
    {
    public:
        ~test_conversion_exception_t() KORA_NOEXCEPT { }
    };

} // namespace

namespace kora { namespace dynamic {

    template<>
    struct converter<test_tag_t> {
        typedef int result_type;

        template<class Controller>
        static inline
        result_type
        convert(const dynamic_t& from, Controller& controller) {
            if (from.is_int()) {
                return from.as_int();
            } else {
                controller.fail(test_conversion_exception_t(), from);
            }
        }

        static inline
        bool
        convertible(const dynamic_t& from) {
            return from.is_int();
        }
    };

}} // namespace kora::dynamic

TEST(DynamicConverter, CustomConverter) {
    auto conversion_result = kora::dynamic_t(kora::dynamic_t::int_t(-5)).to<test_tag_t>();

    static_assert(std::is_same<int, decltype(conversion_result)>::value,
                  "Type of conversion result expected to be int");

    EXPECT_EQ(-5, conversion_result);

    EXPECT_THROW(kora::dynamic_t(false).to<test_tag_t>(), test_conversion_exception_t);
}

namespace {

struct test_object_controller_t {
    test_object_controller_t() :
        start_was_called(false),
        finish_was_called(false)
    { }

    void
    start_array(const kora::dynamic_t&) {
        FAIL();
    }

    void
    finish_array() {
        FAIL();
    }

    void
    item(size_t) {
        FAIL();
    }

    void
    start_object(const kora::dynamic_t& obj) {
        EXPECT_TRUE(obj.is_object());
        EXPECT_TRUE(object_keys.empty());

        start_was_called = true;
    }

    void
    finish_object() {
        finish_was_called = true;
    }

    void
    item(const std::string &key) {
        EXPECT_GT(5, object_keys.size());
        EXPECT_EQ(0, object_keys.count(key));

        object_keys.insert(key);
    }

    template<class Exception>
    KORA_NORETURN
    void
    fail(const Exception&, const kora::dynamic_t&) const {
        // FAIL() cannot be used here because it tries to return void (why?)
        // and this whould be bad noreturn function.
        ADD_FAILURE();
        std::terminate();
    }

    std::set<std::string> object_keys;
    bool start_was_called;
    bool finish_was_called;
};

struct test_array_controller_t {
    test_array_controller_t() :
        start_was_called(false),
        finish_was_called(false)
    { }

    void
    start_array(const kora::dynamic_t& obj) {
        EXPECT_TRUE(obj.is_array());
        EXPECT_TRUE(array_indeces.empty());

        start_was_called = true;
    }

    void
    finish_array() {
        finish_was_called = true;
    }

    void
    item(size_t index) {
        EXPECT_GT(5, array_indeces.size());
        EXPECT_EQ(0, array_indeces.count(index));

        array_indeces.insert(index);
    }

    void
    start_object(const kora::dynamic_t&) {
        FAIL();
    }

    void
    finish_object() {
        FAIL();
    }

    void
    item(const std::string&) {
        FAIL();
    }

    template<class Exception>
    KORA_NORETURN
    void
    fail(const Exception&, const kora::dynamic_t&) const {
        // FAIL() cannot be used here because it tries to return void (why?)
        // and this whould be bad noreturn function.
        ADD_FAILURE();
        std::terminate();
    }

    std::set<size_t> array_indeces;
    bool start_was_called;
    bool finish_was_called;
};

struct test_exception_controller_t {
    void
    start_array(const kora::dynamic_t&) {
        FAIL();
    }

    void
    finish_array() {
        FAIL();
    }

    void
    item(size_t) {
        FAIL();
    }

    void
    start_object(const kora::dynamic_t& obj) {
        EXPECT_TRUE(obj.is_object());
        EXPECT_TRUE(object_keys.empty());
    }

    void
    finish_object() {
        FAIL();
    }

    void
    item(const std::string &key) {
        EXPECT_GT(5, object_keys.size());
        EXPECT_EQ(0, object_keys.count(key));

        object_keys.insert(key);
        current_key = key;
    }

    template<class Exception>
    KORA_NORETURN
    void
    fail(const Exception& error, const kora::dynamic_t& obj) const {
        EXPECT_TRUE(obj.is_bool());

        EXPECT_EQ("error_key", current_key);

        throw error;
    }

    std::set<std::string> object_keys;
    std::string current_key;
};

} // namespace

TEST(DynamicConverter, ObjectController) {
    kora::dynamic_t::object_t object;

    object["key1"] = 5;
    object["key2"] = 4;
    object["key3"] = -5;
    object["key4"] = 5.0;
    object["key5"] = 3;

    test_object_controller_t object_controller;
    kora::dynamic_t(object).to<std::map<std::string, double>>(object_controller);

    EXPECT_TRUE(object_controller.start_was_called);
    EXPECT_TRUE(object_controller.finish_was_called);
    EXPECT_EQ(5, object_controller.object_keys.size());
    EXPECT_EQ(1, object_controller.object_keys.count("key1"));
    EXPECT_EQ(1, object_controller.object_keys.count("key2"));
    EXPECT_EQ(1, object_controller.object_keys.count("key3"));
    EXPECT_EQ(1, object_controller.object_keys.count("key4"));
    EXPECT_EQ(1, object_controller.object_keys.count("key5"));
}

TEST(DynamicConverter, ArrayController) {
    kora::dynamic_t::array_t array(5, "value");

    test_array_controller_t array_controller;
    kora::dynamic_t(array).to<std::vector<std::string>>(array_controller);

    EXPECT_TRUE(array_controller.start_was_called);
    EXPECT_TRUE(array_controller.finish_was_called);
    EXPECT_EQ(5, array_controller.array_indeces.size());
    EXPECT_EQ(1, array_controller.array_indeces.count(0));
    EXPECT_EQ(1, array_controller.array_indeces.count(1));
    EXPECT_EQ(1, array_controller.array_indeces.count(2));
    EXPECT_EQ(1, array_controller.array_indeces.count(3));
    EXPECT_EQ(1, array_controller.array_indeces.count(4));
}

TEST(DynamicConverter, ErrorController) {
    kora::dynamic_t::object_t ill_formed_object;

    ill_formed_object["key1"] = 5;
    ill_formed_object["key2"] = 4;
    ill_formed_object["error_key"] = true;
    ill_formed_object["key4"] = 5.0;
    ill_formed_object["key5"] = 3;

    test_exception_controller_t error_controller;
    EXPECT_THROW((kora::dynamic_t(ill_formed_object).to<std::map<std::string, double>>(error_controller)),
                 kora::bad_cast_t);
}

TEST(DynamicConverter, DynamicObjectIsNotConvertibleToMapIfAnyNonFirstItemIsNotConvertible) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = "string value";
    source.as_object()["key2"] = "string value";

    source.as_object().begin()->second = 10;

    ASSERT_FALSE((source.convertible_to<std::map<std::string, int>>()));
}

TEST(DynamicConverter, DynamicObjectIsNotConvertibleToUnorderedMapIfAnyNonFirstItemIsNotConvertible) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = "string value";
    source.as_object()["key2"] = "string value";

    source.as_object().begin()->second = 10;

    ASSERT_FALSE((source.convertible_to<std::unordered_map<std::string, int>>()));
}
