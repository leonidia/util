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

#include "kora/config.hpp"

TEST(Config, NumericTest) {
    std::string json =
            "{\n"
            "    \"short_min\": -32768,\n"
            "    \"short_max\": 32767,\n"
            "    \"ushort_min\": 0,\n"
            "    \"ushort_max\": 65535,\n"
            "    \"int64_min\": -9223372036854775808,\n"
            "    \"int64_max\": 9223372036854775807,\n"
            "    \"uint64_min\": 0,\n"
            "    \"uint64_max\": 18446744073709551615\n"
            "}\n"
    ;

    std::istringstream stream(json);

    kora::config_parser_t parser;
    parser.parse(stream);

    kora::config_t config = parser.root();

    GTEST_ASSERT_EQ(config.at<short>("short_min"), std::numeric_limits<short>::min());
    GTEST_ASSERT_EQ(config.at<short>("short_max"), std::numeric_limits<short>::max());
    GTEST_ASSERT_EQ(config.at<unsigned short>("ushort_min"), std::numeric_limits<unsigned short>::min());
    GTEST_ASSERT_EQ(config.at<unsigned short>("ushort_max"), std::numeric_limits<unsigned short>::max());
    GTEST_ASSERT_EQ(config.at<int64_t>("int64_min"), std::numeric_limits<int64_t>::min());
    GTEST_ASSERT_EQ(config.at<int64_t>("int64_max"), std::numeric_limits<int64_t>::max());
    GTEST_ASSERT_EQ(config.at<uint64_t>("uint64_min"), std::numeric_limits<uint64_t>::min());
    GTEST_ASSERT_EQ(config.at<uint64_t>("uint64_max"), std::numeric_limits<uint64_t>::max());
}


TEST(Config, ParseErrorTest) {
    std::string json =
            "{\n"
            "    \"short_min\": -32768,\n"
            "    \"short_max\": 32767,\n"
            "    \"ushort_min\": 0,\n"
            "    \"ushort_max\": 65535,\n"
            "    \"int64_min\": -9223372036854775808,\n"
            "    \"int64_max\": 9223372036854775807,\n"
            "    \"uint64_min\": 0,\n"
            "    \"uint64_max\": 18446744073709551615-\n"
            "}\n"
    ;

    std::istringstream stream(json);

    kora::config_parser_t parser;
    try {
        parser.parse(stream);
        GTEST_FAIL();
    } catch (kora::config_parser_error &error) {
        GTEST_ASSERT_EQ(error.line_number(), 9);
        GTEST_ASSERT_EQ(error.column_number(), 39);
    }
}

TEST(Config, ParseErrorTest_2) {
    std::string json = "s{\n";

    std::istringstream stream(json);

    kora::config_parser_t parser;

    try {
        parser.parse(stream);
        GTEST_FAIL();
    } catch (kora::config_parser_error &error) {
        GTEST_ASSERT_EQ(error.line_number(), 1);
        GTEST_ASSERT_EQ(error.column_number(), 1);
    }
}

TEST(Config, ParseErrorTest_3) {
    std::string json = "{s\n";

    std::istringstream stream(json);

    kora::config_parser_t parser;

    try {
        parser.parse(stream);
        GTEST_FAIL();
    } catch (kora::config_parser_error &error) {
        GTEST_ASSERT_EQ(error.line_number(), 1);
        GTEST_ASSERT_EQ(error.column_number(), 1);
    }
}
