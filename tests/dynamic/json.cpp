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

#include <boost/lexical_cast.hpp>

#include <sstream>

namespace {

    void
    check_parsed_object(const kora::dynamic_t& parsed) {
        ASSERT_TRUE(parsed.is_object());

        EXPECT_EQ(1, parsed.as_object().count("null_key"));
        EXPECT_EQ(kora::dynamic_t::null, parsed.as_object()["null_key"]);

        EXPECT_EQ(1, parsed.as_object().count("true_key"));
        EXPECT_EQ(parsed.as_object()["true_key"], true);

        EXPECT_EQ(1, parsed.as_object().count("false_key"));
        EXPECT_EQ(parsed.as_object()["false_key"], false);

        EXPECT_EQ(1, parsed.as_object().count("int_key"));
        EXPECT_EQ(5, parsed.as_object()["int_key"]);

        EXPECT_EQ(1, parsed.as_object().count("double_key"));
        EXPECT_DOUBLE_EQ(25.2, parsed.as_object()["double_key"].to<double>());

        EXPECT_EQ(1, parsed.as_object().count("string_key"));
        EXPECT_EQ("xdd", parsed.as_object()["string_key"]);

        EXPECT_EQ(1, parsed.as_object().count("array_key"));
        EXPECT_TRUE(parsed.as_object()["array_key"].is_array());
        EXPECT_EQ(6, parsed.as_object()["array_key"].as_array().size());
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[0], true);
        EXPECT_EQ(-5, parsed.as_object()["array_key"].as_array()[1]);
        EXPECT_DOUBLE_EQ(-5.0, parsed.as_object()["array_key"].as_array()[2].to<double>());
        EXPECT_EQ("-_-", parsed.as_object()["array_key"].as_array()[3]);
        EXPECT_TRUE(parsed.as_object()["array_key"].as_array()[4].is_array());
        EXPECT_EQ(2, parsed.as_object()["array_key"].as_array()[4].as_array().size());
        EXPECT_EQ(1, parsed.as_object()["array_key"].as_array()[4].as_array()[0]);
        EXPECT_EQ(2, parsed.as_object()["array_key"].as_array()[4].as_array()[1]);
        EXPECT_TRUE(parsed.as_object()["array_key"].as_array()[5].is_object());
        EXPECT_EQ(1, parsed.as_object()["array_key"].as_array()[5].as_object().size());
        EXPECT_EQ(1, parsed.as_object()["array_key"].as_array()[5].as_object().count("key"));
        EXPECT_EQ(12, parsed.as_object()["array_key"].as_array()[5].as_object()["key"]);

        EXPECT_EQ(1, parsed.as_object().count("object_key"));
        EXPECT_TRUE(parsed.as_object()["object_key"].is_object());
        EXPECT_EQ(1, parsed.as_object()["object_key"].as_object().size());
        EXPECT_EQ(1, parsed.as_object()["object_key"].as_object().count("key"));
        EXPECT_EQ(13, parsed.as_object()["object_key"].as_object()["key"]);
    }

    kora::dynamic_t
    construct_object() {
        kora::dynamic_t::object_t object;

        object["null_key"] = kora::dynamic_t::null;
        object["true_key"] = true;
        object["false_key"] = false;
        object["int_key"] = 5;
        object["double_key"] = 25.2;
        object["string_key"] = "xdd";

        kora::dynamic_t::array_t nested_array;
        nested_array.emplace_back(true);
        nested_array.emplace_back(-5);
        nested_array.emplace_back(-5.0);
        nested_array.emplace_back("-_-");
        nested_array.emplace_back(kora::dynamic_t::array_t({1, 2}));

        kora::dynamic_t::object_t nested_object1;
        nested_object1["key"] = 12;
        nested_array.emplace_back(nested_object1);

        object["array_key"] = nested_array;

        kora::dynamic_t::object_t nested_object2;
        nested_object2["key"] = 13;

        object["object_key"] = nested_object2;

        return object;
    }

    void
    check_parsed_array(const kora::dynamic_t& parsed) {
        ASSERT_TRUE(parsed.is_array());
        EXPECT_EQ(6, parsed.as_array().size());
        EXPECT_EQ(parsed.as_array()[0], true);
        EXPECT_EQ(-5, parsed.as_array()[1]);
        EXPECT_DOUBLE_EQ(-5.0, parsed.as_array()[2].to<double>());
        EXPECT_EQ("-_-", parsed.as_array()[3]);
        EXPECT_TRUE(parsed.as_array()[4].is_array());
        EXPECT_EQ(2, parsed.as_array()[4].as_array().size());
        EXPECT_EQ(1, parsed.as_array()[4].as_array()[0]);
        EXPECT_EQ(2, parsed.as_array()[4].as_array()[1]);
        EXPECT_TRUE(parsed.as_array()[5].is_object());
        EXPECT_EQ(1, parsed.as_array()[5].as_object().size());
        EXPECT_EQ(1, parsed.as_array()[5].as_object().count("key"));
        EXPECT_EQ(12, parsed.as_array()[5].as_object()["key"]);
    }

    kora::dynamic_t
    construct_array() {
        kora::dynamic_t::array_t array;
        array.emplace_back(true);
        array.emplace_back(-5);
        array.emplace_back(-5.0);
        array.emplace_back("-_-");
        array.emplace_back(kora::dynamic_t::array_t({1, 2}));

        kora::dynamic_t::object_t nested_object;
        nested_object["key"] = 12;
        array.emplace_back(nested_object);

        return array;
    }
}

TEST(DynamicJson, ObjectParsing) {
    std::string object_json =
    "{\n"
    "    \"null_key\": null,\n"
    "    \"true_key\": true,\n"
    "    \"false_key\": false,\n"
    "    \"int_key\": 5,\n"
    "    \"double_key\": 25.2,\n"
    "    \"string_key\": \"xdd\",\n"
    "    \"array_key\": [\n"
    "        true,\n"
    "        -5,\n"
    "        -5.0,\n"
    "        \"-_-\",\n"
    "        [1, 2],\n"
    "        { \"key\": 12}\n"
    "    ],\n"
    "    \"object_key\": {\n"
    "        \"key\": 13\n"
    "    }\n"
    "}"
    "some garbage is allowed here";

    std::istringstream input(object_json);

    check_parsed_object(kora::dynamic::from_json(input));

}

TEST(DynamicJson, ArrayParsing) {
    std::string array_json =
    "[\n"
    "    true,\n"
    "    -5,\n"
    "    -5.0,\n"
    "    \"-_-\",\n"
    "    [1, 2],\n"
    "    { \"key\": 12}\n"
    "]\n"
    "some garbage is allowed here";

    std::istringstream input(array_json);

    check_parsed_array(kora::dynamic::from_json(input));
}

namespace {
    void
    check_parsing_error(const std::string& data) {
        std::istringstream input(data);
        EXPECT_THROW(kora::dynamic::from_json(input), kora::json_parsing_error_t);
    }
}

TEST(DynamicJson, NullIsBadJson) {
    check_parsing_error("null");
}

TEST(DynamicJson, BoolIsBadJson) {
    check_parsing_error("true");
    check_parsing_error("false");
}

TEST(DynamicJson, NumberIsBadJson) {
    check_parsing_error("5");
    check_parsing_error("-5");
    check_parsing_error("-5.2");
}

TEST(DynamicJson, StringIsBadJson) {
    check_parsing_error("\"dcdcd\"");
}

TEST(DynamicJson, GarbageIsBadJson) {
    check_parsing_error("fdkjfj!@#j");
}

TEST(DynamicJson, ValuesToJson) {
    std::ostringstream output;

    kora::write_json(output, kora::dynamic_t());
    EXPECT_EQ("null", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(true));
    EXPECT_EQ("true", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(false));
    EXPECT_EQ("false", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(5));
    EXPECT_EQ("5", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(-5));
    EXPECT_EQ("-5", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(25.2));
    EXPECT_EQ("25.2", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(-25.2));
    EXPECT_EQ("-25.2", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t("xd"));
    EXPECT_EQ("\"xd\"", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(kora::dynamic_t::array_t()));
    EXPECT_EQ("[]", output.str());

    output.str("");
    kora::write_json(output, kora::dynamic_t(kora::dynamic_t::object_t()));
    EXPECT_EQ("{}", output.str());
}

TEST(DynamicJson, ObjectToJson) {
    std::ostringstream output;
    kora::write_json(output, construct_object());

    std::istringstream input(output.str());
    check_parsed_object(kora::dynamic::from_json(input));
}

TEST(DynamicJson, ArrayToJson) {
    std::ostringstream output;
    kora::write_json(output, construct_array());

    std::istringstream input(output.str());
    check_parsed_array(kora::dynamic::from_json(input));
}

TEST(DynamicJson, ValuesToPrettyJson) {
    std::ostringstream output;

    kora::write_pretty_json(output, kora::dynamic_t());
    EXPECT_EQ("null", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(true));
    EXPECT_EQ("true", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(false));
    EXPECT_EQ("false", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(5));
    EXPECT_EQ("5", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(-5));
    EXPECT_EQ("-5", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(25.2));
    EXPECT_EQ("25.2", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(-25.2));
    EXPECT_EQ("-25.2", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t("xd"));
    EXPECT_EQ("\"xd\"", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(kora::dynamic_t::array_t()));
    EXPECT_EQ("[]", output.str());

    output.str("");
    kora::write_pretty_json(output, kora::dynamic_t(kora::dynamic_t::object_t()));
    EXPECT_EQ("{}", output.str());

    output.str("");
    kora::dynamic_t::object_t object_with_key;
    object_with_key["key"] = 1337;
    kora::write_pretty_json(output, kora::dynamic_t(object_with_key), 7);
    EXPECT_EQ("{\n       \"key\": 1337\n}", output.str());
}

TEST(DynamicJson, ObjectToPrettyJson) {
    std::ostringstream output;
    kora::write_pretty_json(output, construct_object());

    std::istringstream input(output.str());
    check_parsed_object(kora::dynamic::from_json(input));
}

TEST(DynamicJson, ArrayToPrettyJson) {
    std::ostringstream output;
    kora::write_pretty_json(output, construct_array());

    std::istringstream input(output.str());
    check_parsed_array(kora::dynamic::from_json(input));
}

TEST(DynamicJson, ValuesToStream) {
    std::ostringstream output;

    output << kora::dynamic_t();
    EXPECT_EQ("null", output.str());

    output.str("");
    output << kora::dynamic_t(true);
    EXPECT_EQ("1", output.str());

    output.str("");
    output << kora::dynamic_t(false);
    EXPECT_EQ("0", output.str());

    output.str("");
    output << kora::dynamic_t(5);
    EXPECT_EQ("5", output.str());

    output.str("");
    output << kora::dynamic_t(-5);
    EXPECT_EQ("-5", output.str());

    output.str("");
    output << kora::dynamic_t(25.2);
    EXPECT_EQ("25.2", output.str());

    output.str("");
    output << kora::dynamic_t(-25.2);
    EXPECT_EQ("-25.2", output.str());

    output.str("");
    output << kora::dynamic_t("xd");
    EXPECT_EQ("xd", output.str());

    output.str("");
    output << kora::dynamic_t(kora::dynamic_t::array_t());
    EXPECT_EQ("[]", output.str());

    output.str("");
    output << kora::dynamic_t(kora::dynamic_t::object_t());
    EXPECT_EQ("{}", output.str());
}

TEST(DynamicJson, ObjectToStream) {
    std::ostringstream output;
    output << construct_object();

    std::istringstream input(output.str());
    check_parsed_object(kora::dynamic::from_json(input));
}

TEST(DynamicJson, ArrayToStream) {
    std::ostringstream output;
    output << construct_array();

    std::istringstream input(output.str());
    check_parsed_array(kora::dynamic::from_json(input));
}

TEST(DynamicJson, BoostLexicalCast) {
    std::string serialized = boost::lexical_cast<std::string>(construct_object());

    std::istringstream input(serialized);
    check_parsed_object(kora::dynamic::from_json(input));
}
