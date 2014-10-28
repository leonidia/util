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

#include "kora/dynamic.hpp"

#include <boost/lexical_cast.hpp>

#include <sstream>

namespace {

    void
    check_parsed_object(const kora::dynamic_t& parsed) {
        ASSERT_TRUE(parsed.is_object());

        EXPECT_EQ(parsed.as_object().count("null_key"), 1);
        EXPECT_EQ(parsed.as_object()["null_key"], kora::dynamic_t::null);

        EXPECT_EQ(parsed.as_object().count("true_key"), 1);
        EXPECT_EQ(parsed.as_object()["true_key"], true);

        EXPECT_EQ(parsed.as_object().count("false_key"), 1);
        EXPECT_EQ(parsed.as_object()["false_key"], false);

        EXPECT_EQ(parsed.as_object().count("int_key"), 1);
        EXPECT_EQ(parsed.as_object()["int_key"], 5);

        EXPECT_EQ(parsed.as_object().count("double_key"), 1);
        EXPECT_DOUBLE_EQ(parsed.as_object()["double_key"].to<double>(), 25.2);

        EXPECT_EQ(parsed.as_object().count("string_key"), 1);
        EXPECT_EQ(parsed.as_object()["string_key"], "xdd");

        EXPECT_EQ(parsed.as_object().count("array_key"), 1);
        EXPECT_TRUE(parsed.as_object()["array_key"].is_array());
        EXPECT_EQ(parsed.as_object()["array_key"].as_array().size(), 6);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[0], true);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[1], -5);
        EXPECT_DOUBLE_EQ(parsed.as_object()["array_key"].as_array()[2].to<double>(), -5.0);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[3], "-_-");
        EXPECT_TRUE(parsed.as_object()["array_key"].as_array()[4].is_array());
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[4].as_array().size(), 2);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[4].as_array()[0], 1);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[4].as_array()[1], 2);
        EXPECT_TRUE(parsed.as_object()["array_key"].as_array()[5].is_object());
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[5].as_object().size(), 1);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[5].as_object().count("key"), 1);
        EXPECT_EQ(parsed.as_object()["array_key"].as_array()[5].as_object()["key"], 12);

        EXPECT_EQ(parsed.as_object().count("object_key"), 1);
        EXPECT_TRUE(parsed.as_object()["object_key"].is_object());
        EXPECT_EQ(parsed.as_object()["object_key"].as_object().size(), 1);
        EXPECT_EQ(parsed.as_object()["object_key"].as_object().count("key"), 1);
        EXPECT_EQ(parsed.as_object()["object_key"].as_object()["key"], 13);
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
        EXPECT_EQ(parsed.as_array().size(), 6);
        EXPECT_EQ(parsed.as_array()[0], true);
        EXPECT_EQ(parsed.as_array()[1], -5);
        EXPECT_DOUBLE_EQ(parsed.as_array()[2].to<double>(), -5.0);
        EXPECT_EQ(parsed.as_array()[3], "-_-");
        EXPECT_TRUE(parsed.as_array()[4].is_array());
        EXPECT_EQ(parsed.as_array()[4].as_array().size(), 2);
        EXPECT_EQ(parsed.as_array()[4].as_array()[0], 1);
        EXPECT_EQ(parsed.as_array()[4].as_array()[1], 2);
        EXPECT_TRUE(parsed.as_array()[5].is_object());
        EXPECT_EQ(parsed.as_array()[5].as_object().size(), 1);
        EXPECT_EQ(parsed.as_array()[5].as_object().count("key"), 1);
        EXPECT_EQ(parsed.as_array()[5].as_object()["key"], 12);
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

TEST(Dynamic, JsonObjectParsing) {
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

    check_parsed_object(kora::dynamic_t::from_json(input));

}

TEST(Dynamic, JsonArrayParsing) {
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

    check_parsed_array(kora::dynamic_t::from_json(input));
}

namespace {
    void
    check_parsing_error(const std::string& data) {
        std::istringstream input(data);
        EXPECT_THROW(kora::dynamic_t::from_json(input), kora::json_parsing_error_t);
    }
}

TEST(Dynamic, JsonParsingErrors) {
    {
        SCOPED_TRACE("Null");
        check_parsing_error("null");
    }
    {
        SCOPED_TRACE("Bool");
        check_parsing_error("true");
        check_parsing_error("false");
    }
    {
        SCOPED_TRACE("Numbers");
        check_parsing_error("5");
        check_parsing_error("-5");
        check_parsing_error("-5.2");
    }
    {
        SCOPED_TRACE("String");
        check_parsing_error("\"dcdcd\"");
    }
    {
        SCOPED_TRACE("Garbage");
        check_parsing_error("fdkjfj!@#j");
    }
}

TEST(Dynamic, ObjectToJson) {
    std::ostringstream output;
    construct_object().to_json(output);

    std::istringstream input(output.str());
    check_parsed_object(kora::dynamic_t::from_json(input));
}

TEST(Dynamic, ArrayToJson) {
    std::ostringstream output;
    construct_array().to_json(output);

    std::istringstream input(output.str());
    check_parsed_array(kora::dynamic_t::from_json(input));
}

TEST(Dynamic, ValuesToStream) {
    std::ostringstream output;

    output << kora::dynamic_t();
    EXPECT_EQ("null", output.str());

    output.str("");
    output << kora::dynamic_t(true);
    EXPECT_EQ("true", output.str());

    output.str("");
    output << kora::dynamic_t(false);
    EXPECT_EQ("false", output.str());

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
    EXPECT_EQ("\"xd\"", output.str());

    output.str("");
    output << kora::dynamic_t(kora::dynamic_t::array_t());
    EXPECT_EQ("[]", output.str());

    output.str("");
    output << kora::dynamic_t(kora::dynamic_t::object_t());
    EXPECT_EQ("{}", output.str());
}

TEST(Dynamic, ObjectToStream) {
    std::ostringstream output;
    output << construct_object();

    std::istringstream input(output.str());
    check_parsed_object(kora::dynamic_t::from_json(input));
}

TEST(Dynamic, ArrayToStream) {
    std::ostringstream output;
    output << construct_array();

    std::istringstream input(output.str());
    check_parsed_array(kora::dynamic_t::from_json(input));
}

TEST(Dynamic, BoostLexicalCast) {
    std::string serialized = boost::lexical_cast<std::string>(construct_object());

    std::istringstream input(serialized);
    check_parsed_object(kora::dynamic_t::from_json(input));
}
