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

#include <sstream>

namespace {
    bool
    compare_double(double left, double right) {
        const double eps = 0.1;
        return (left - right) <= eps && (left - right) >= -eps;
    }
}

TEST(Dynamic, JsonObjectParsing) {
    std::string object_json =
    "{\n"
    "    \"null_key\": null,\n"
    "    \"true_key\": true,\n"
    "    \"false_key\": false,\n"
    "    \"int_key\": 5,\n"
    "    \"double_key\": 25.3,\n"
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

    leonidia::dynamic_t parsed = leonidia::dynamic_t::from_json(input);

    ASSERT_TRUE(parsed.is_object());

    EXPECT_EQ(parsed.as_object().count("null_key"), 1);
    EXPECT_EQ(parsed.as_object()["null_key"], leonidia::dynamic_t::null);

    EXPECT_EQ(parsed.as_object().count("true_key"), 1);
    EXPECT_EQ(parsed.as_object()["true_key"], true);

    EXPECT_EQ(parsed.as_object().count("false_key"), 1);
    EXPECT_EQ(parsed.as_object()["false_key"], false);

    EXPECT_EQ(parsed.as_object().count("int_key"), 1);
    EXPECT_EQ(parsed.as_object()["int_key"], 5);

    EXPECT_EQ(parsed.as_object().count("double_key"), 1);
    EXPECT_TRUE(compare_double(parsed.as_object()["double_key"].as_double(), 25.3));

    EXPECT_EQ(parsed.as_object().count("string_key"), 1);
    EXPECT_EQ(parsed.as_object()["string_key"], "xdd");

    EXPECT_EQ(parsed.as_object().count("array_key"), 1);
    EXPECT_TRUE(parsed.as_object()["array_key"].is_array());
    EXPECT_EQ(parsed.as_object()["array_key"].as_array().size(), 6);
    EXPECT_EQ(parsed.as_object()["array_key"].as_array()[0], true);
    EXPECT_EQ(parsed.as_object()["array_key"].as_array()[1], -5);
    EXPECT_TRUE(compare_double(parsed.as_object()["array_key"].as_array()[2].as_double(), -5.0));
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
