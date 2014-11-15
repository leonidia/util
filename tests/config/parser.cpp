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

#include "kora/config/parser.hpp"

#include <fstream>

TEST(ConfigParser, DefaultConstructor) {
    kora::config_parser_t parser;
    EXPECT_TRUE(parser.root().underlying_object().is_null());
    EXPECT_EQ("<root>", parser.root().path());
}

TEST(ConfigParser, CopyConstructor1) {
    kora::config_parser_t parser1;

    kora::config_parser_t parser2(parser1);
    EXPECT_TRUE(parser2.root().underlying_object().is_null());
    EXPECT_EQ("<root>", parser2.root().path());
}

TEST(ConfigParser, CopyConstructor2) {
    kora::config_parser_t parser1;

    std::string json = "{\"key\": 43}";

    std::istringstream stream(json);
    parser1.parse(stream);

    EXPECT_EQ(43, parser1.root().at<int>("key"));

    kora::config_parser_t parser2(parser1);
    EXPECT_EQ(parser1.root().underlying_object(), parser2.root().underlying_object());
    EXPECT_EQ("<root>", parser2.root().path());
}

TEST(ConfigParser, CopyAssignment1) {
    kora::config_parser_t parser1;

    kora::config_parser_t parser2;
    parser2 = parser1;

    EXPECT_TRUE(parser2.root().underlying_object().is_null());
    EXPECT_EQ("<root>", parser2.root().path());
}

TEST(ConfigParser, CopyAssignment2) {
    kora::config_parser_t parser1;

    std::string json = "{\"key\": 43}";

    std::istringstream stream(json);
    parser1.parse(stream);

    EXPECT_EQ(43, parser1.root().at<int>("key"));

    kora::config_parser_t parser2;
    parser2 = parser1;
    
    EXPECT_EQ(parser1.root().underlying_object(), parser2.root().underlying_object());
    EXPECT_EQ("<root>", parser2.root().path());
}

namespace {

    void
    test_numbers_parsing(std::function<void(std::string, kora::config_parser_t&)> parse) {
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

        kora::config_parser_t parser;
        parse(json, parser);

        kora::config_t config = parser.root();

        EXPECT_EQ("<root>", config.path());

        ASSERT_EQ(std::numeric_limits<short>::min(), config.at<short>("short_min"));
        ASSERT_EQ(std::numeric_limits<short>::max(), config.at<short>("short_max"));
        ASSERT_EQ(std::numeric_limits<unsigned short>::min(), config.at<unsigned short>("ushort_min"));
        ASSERT_EQ(std::numeric_limits<unsigned short>::max(), config.at<unsigned short>("ushort_max"));
        ASSERT_EQ(std::numeric_limits<int64_t>::min(), config.at<int64_t>("int64_min"));
        ASSERT_EQ(std::numeric_limits<int64_t>::max(), config.at<int64_t>("int64_max"));
        ASSERT_EQ(std::numeric_limits<uint64_t>::min(), config.at<uint64_t>("uint64_min"));
        ASSERT_EQ(std::numeric_limits<uint64_t>::max(), config.at<uint64_t>("uint64_max"));
    }

    void
    test_basic_parsing(std::function<void(std::string, kora::config_parser_t&)> parse) {
        std::string json =
                "{\n"
                "    \"int_key\": 1337,\n"
                "    \"string_key\": \"@_@\",\n"
                "    \"array_key\": [1, true, \"%)))\"],\n"
                "    \"object_key\": {\n"
                "        \"key\": \"value\"\n"
                "    }\n"
                "}\n"
        ;

        kora::config_parser_t parser;
        parse(json, parser);

        kora::config_t config = parser.root();

        EXPECT_EQ("<root>", config.path());

        ASSERT_EQ(1337, config.at<int>("int_key"));
        ASSERT_EQ("@_@", config.at<std::string>("string_key"));
        ASSERT_EQ(1, config.at("array_key").at<int>(0));
        ASSERT_EQ(true, config.at("array_key").at<bool>(1));
        ASSERT_EQ("%)))", config.at("array_key").at<std::string>(2));
        ASSERT_EQ("value", config.at("object_key").at<std::string>("key"));
    }

    void
    test_parsing_error_1(std::function<void(std::string, kora::config_parser_t&)> parse) {
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

        kora::config_parser_t parser;

        try {
            parse(json, parser);
            GTEST_FAIL();
        } catch (kora::config_parser_error_t &error) {
            GTEST_ASSERT_EQ(9, error.line_number());
            GTEST_ASSERT_EQ(39, error.column_number());
        }
    }

    void
    test_parsing_error_2(std::function<void(std::string, kora::config_parser_t&)> parse) {
        std::string json = "s{\n";

        kora::config_parser_t parser;

        try {
            parse(json, parser);
            GTEST_FAIL();
        } catch (kora::config_parser_error_t &error) {
            GTEST_ASSERT_EQ(1, error.line_number());
            GTEST_ASSERT_EQ(1, error.column_number());
        }
    }

    void
    test_parsing_error_3(std::function<void(std::string, kora::config_parser_t&)> parse) {
        std::string json = "{s\n";

        kora::config_parser_t parser;

        try {
            parse(json, parser);
            GTEST_FAIL();
        } catch (kora::config_parser_error_t &error) {
            GTEST_ASSERT_EQ(1, error.line_number());
            GTEST_ASSERT_EQ(2, error.column_number());
        }
    }

    void
    test_that_anything_after_config_isnt_allowed(std::function<void(std::string, kora::config_parser_t&)> parse) {
        std::string json =
                "{\n"
                "    \"key1\": -1,\n"
                "    \"key2\": true\n"
                "}garbage\n"
        ;

        kora::config_parser_t parser;

        try {
            parse(json, parser);
            GTEST_FAIL();
        } catch (kora::config_parser_error_t &error) {
            ASSERT_EQ(4, error.line_number());
            ASSERT_EQ(2, error.column_number());
        }
    }

    void
    test_that_config_must_be_an_object(std::function<void(std::string, kora::config_parser_t&)> parse) {
        std::string json =
                "  [\n"
                "      -1,\n"
                "      true\n"
                "  ]\n"
        ;

        kora::config_parser_t parser;

        try {
            parse(json, parser);
            GTEST_FAIL();
        } catch (kora::config_parser_error_t &error) {
            ASSERT_EQ(1, error.line_number());
            ASSERT_EQ(3, error.column_number());
        }
    }

    void
    test_parser(std::function<void(std::string, kora::config_parser_t&)> parse) {
        test_basic_parsing(parse);
        test_numbers_parsing(parse);
        test_parsing_error_1(parse);
        test_parsing_error_2(parse);
        test_parsing_error_3(parse);
        test_that_anything_after_config_isnt_allowed(parse);
        test_that_config_must_be_an_object(parse);
    }

    const std::string tmpfilename = "config_parser_test_tmp_file_dont_touch_this_ijhiu47837y8hc";

    void
    method_parse_wrapper(std::string json, kora::config_parser_t &parser) {
        std::istringstream stream(json);
        parser.parse(stream);
    }

    void
    method_open_wrapper(std::string json, kora::config_parser_t &parser) {
        std::ofstream tmpfile(tmpfilename);
        tmpfile.write(json.data(), json.size());
        tmpfile.close();
        parser.open(tmpfilename);
    }

    void
    stream_constructor_wrapper(std::string json, kora::config_parser_t &parser) {
        std::istringstream stream(json);
        parser = kora::config_parser_t(stream);
    }

    void
    file_constructor_wrapper(std::string json, kora::config_parser_t &parser) {
        std::ofstream tmpfile(tmpfilename);
        tmpfile.write(json.data(), json.size());
        tmpfile.close();
        parser = kora::config_parser_t(tmpfilename);
    }

} // namespace

TEST(ConfigParser, StreamConstructor) {
    test_parser(&stream_constructor_wrapper);
}

TEST(ConfigParser, FileConstructor) {
    test_parser(&file_constructor_wrapper);
}

TEST(ConfigParser, Parse) {
    test_parser(&method_parse_wrapper);
}

TEST(ConfigParser, Open) {
    test_parser(&method_open_wrapper);
}
