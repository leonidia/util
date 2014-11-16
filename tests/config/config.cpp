/*
Copyright (c) 2014 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#include "kora/config/config.hpp"

TEST(Config, Constructor1) {
    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;
    underlying_object.as_object()["key2"] = "=^._.^=";

    kora::config_t config("koren", underlying_object);

    EXPECT_EQ("koren", config.path());
    EXPECT_EQ(5, config.underlying_object().as_object()["key1"]);
    EXPECT_EQ("=^._.^=", config.underlying_object().as_object()["key2"]);
}

TEST(Config, MoveConstructor) {
    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;
    underlying_object.as_object()["key2"] = "=^._.^=";

    kora::config_t source("koren", underlying_object);

    kora::config_t destination(std::move(source));

    EXPECT_EQ("koren", destination.path());
    EXPECT_EQ(5, destination.underlying_object().as_object()["key1"]);
    EXPECT_EQ("=^._.^=", destination.underlying_object().as_object()["key2"]);
}

TEST(Config, Size) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).size(), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).size(), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).size(), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).size(), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).size(), kora::config_cast_error_t);
    EXPECT_EQ(7, kora::config_t("", "=^._.^=").size());
    EXPECT_EQ(3, kora::config_t("", kora::dynamic_t::array_t(3, 4)).size());
    EXPECT_EQ(0, kora::config_t("", kora::dynamic_t::object_t()).size());
}

TEST(Config, Has) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).has("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).has("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).has("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).has("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).has("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").has("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::array_t(3, 4)).has("key"), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;

    kora::config_t config("", underlying_object);

    EXPECT_TRUE(config.has("key1"));
    EXPECT_FALSE(config.has("key2"));
}

TEST(Config, AtKey) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).at("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).at("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).at("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).at("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).at("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").at("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::array_t(3, 4)).at("key"), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;

    kora::config_t config("", underlying_object);

    EXPECT_EQ(5, config.at("key1").underlying_object());
    EXPECT_EQ(".key1", config.at("key1").path());

    EXPECT_THROW(config.at("key2"), kora::config_access_error_t);
}

TEST(Config, AtKeyOperator) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null)["key"], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true))["key"], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2))["key"], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3))["key"], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4))["key"], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=")["key"], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::array_t(3, 4))["key"], kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;

    kora::config_t config("", underlying_object);

    EXPECT_EQ(5, config["key1"].underlying_object());
    EXPECT_EQ(".key1", config["key1"].path());

    EXPECT_THROW(config["key2"], kora::config_access_error_t);
}

TEST(Config, AtKeyWithDefault) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).at("key", 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).at("key", 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).at("key", 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).at("key", 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).at("key", 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").at("key", 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::array_t(3, 4)).at("key", 0), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;

    kora::config_t config("", underlying_object);

    auto key_value = config.at("key1", 0);

    static_assert(std::is_same<int, decltype(key_value)>::value, "Expected result of type int.");

    EXPECT_EQ(5, key_value);
    EXPECT_EQ(0, config.at("key2", 0));
    EXPECT_THROW(config.at("key1", std::string("default")), kora::config_cast_error_t);
}

TEST(Config, AtKeyConverting) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).at<int>("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).at<int>("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).at<int>("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).at<int>("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).at<int>("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").at<int>("key"), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::array_t(3, 4)).at<int>("key"), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::object_t();

    underlying_object.as_object()["key1"] = 5;

    kora::config_t config("", underlying_object);

    auto key_value = config.at<int>("key1");

    static_assert(std::is_same<int, decltype(key_value)>::value, "Expected result of type int.");

    EXPECT_EQ(5, key_value);
    EXPECT_THROW(config.at<std::string>("key2"), kora::config_access_error_t);
    EXPECT_THROW(config.at<std::string>("key1"), kora::config_cast_error_t);
}

TEST(Config, AtIndex) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).at(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).at(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).at(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).at(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).at(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").at(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::object_t()).at(0), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::array_t(3, 4);
    kora::config_t config("", underlying_object);

    EXPECT_EQ(4, config.at(0).underlying_object());
    EXPECT_EQ("[0]", config.at(0).path());
    EXPECT_EQ(4, config.at(2).underlying_object());
    EXPECT_THROW(config.at(3).underlying_object(), kora::config_access_error_t);
}

TEST(Config, AtIndexOperator) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null)[0], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true))[0], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2))[0], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3))[0], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4))[0], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=")[0], kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::object_t())[0], kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::array_t(3, 4);
    kora::config_t config("", underlying_object);

    EXPECT_EQ(4, config[0].underlying_object());
    EXPECT_EQ("[0]", config[0].path());
    EXPECT_EQ(4, config[2].underlying_object());
    EXPECT_THROW(config[3].underlying_object(), kora::config_access_error_t);
}

TEST(Config, AtIndexWithDefault) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).at(0, 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).at(0, 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).at(0, 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).at(0, 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).at(0, 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").at(0, 0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::object_t()).at(0, 0), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::array_t(3, 4);
    kora::config_t config("", underlying_object);

    auto index_value = config.at(0, 0);

    static_assert(std::is_same<int, decltype(index_value)>::value, "Expected result of type int.");

    EXPECT_EQ(4, index_value);
    EXPECT_EQ(4, config.at(2, 0));
    EXPECT_EQ(0, config.at(3, 0));
    EXPECT_THROW(config.at(0, std::string("default")), kora::config_cast_error_t);
}

TEST(Config, AtIndexConverting) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).at<int>(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).at<int>(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(2)).at<int>(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::uint_t(3)).at<int>(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).at<int>(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", "=^._.^=").at<int>(0), kora::config_cast_error_t);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::object_t()).at<int>(0), kora::config_cast_error_t);


    kora::dynamic_t underlying_object = kora::dynamic_t::array_t(3, 4);
    kora::config_t config("", underlying_object);

    auto index_value = config.at<int>(0);

    static_assert(std::is_same<int, decltype(index_value)>::value, "Expected result of type int.");

    EXPECT_EQ(4, index_value);
    EXPECT_EQ(4, config.at<int>(2));
    EXPECT_THROW(config.at<int>(3), kora::config_access_error_t);
    EXPECT_THROW(config.at<std::string>(0), kora::config_cast_error_t);
}

TEST(Config, To) {
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::null).to<int>(), kora::config_cast_error_t);
    EXPECT_EQ(kora::config_t("", kora::dynamic_t::bool_t(true)).to<bool>(), true);
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::bool_t(true)).to<int>(), kora::config_cast_error_t);
    EXPECT_EQ(2, kora::config_t("", kora::dynamic_t::int_t(2)).to<int>());
    EXPECT_EQ(2, kora::config_t("", kora::dynamic_t::int_t(2)).to<unsigned int>());
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::int_t(-2)).to<unsigned int>(), kora::config_cast_error_t);
    EXPECT_EQ(3, kora::config_t("", kora::dynamic_t::uint_t(3)).to<int>());
    EXPECT_EQ(3, kora::config_t("", kora::dynamic_t::uint_t(3)).to<unsigned int>());
    EXPECT_THROW(kora::config_t("", kora::dynamic_t::double_t(4)).to<int>(), kora::config_cast_error_t);
    EXPECT_EQ(4, kora::config_t("", kora::dynamic_t::double_t(4)).to<float>());
    EXPECT_EQ("=^._.^=", kora::config_t("", "=^._.^=").to<std::string>());
    EXPECT_STREQ("=^._.^=", kora::config_t("", "=^._.^=").to<const char*>());
    EXPECT_EQ(std::vector<int>(3, 4), kora::config_t("", kora::dynamic_t::array_t(3, 4)).to<std::vector<int>>());
    EXPECT_EQ((std::map<std::string, int>()),
              (kora::config_t("", kora::dynamic_t::object_t()).to<std::map<std::string, int>>()));

    kora::dynamic_t underlying_object = kora::dynamic_t::array_t(3, 4);
    underlying_object.as_array()[1] = "=^._.^=";

    kora::config_t config("", underlying_object);

    try {
        config.to<std::vector<int>>();
        FAIL();
    } catch (const kora::config_cast_error_t& error) {
        EXPECT_EQ("[1]", error.path());
    }
}

TEST(Config, EqualityConfigDynamic) {
    kora::dynamic_t underlying = kora::dynamic_t::object_t();

    underlying.as_object()["key0"] = kora::dynamic_t::null;
    underlying.as_object()["key1"] = 5;
    underlying.as_object()["key2"] = true;
    underlying.as_object()["key3"] = "~:{";

    kora::config_t config("", underlying);

    EXPECT_TRUE(config == underlying);
    EXPECT_FALSE(config == 5);
    EXPECT_FALSE(config == kora::dynamic_t::null);

    EXPECT_TRUE(config.at("key0") == kora::dynamic_t::null);
    EXPECT_FALSE(config.at("key0") == true);
    EXPECT_FALSE(config.at("key0") == 5);

    EXPECT_TRUE(config.at("key1") == 5);
    EXPECT_FALSE(config.at("key1") == 25);
    EXPECT_FALSE(config.at("key1") == true);
    EXPECT_FALSE(config.at("key1") == kora::dynamic_t::null);

    EXPECT_TRUE(config.at("key2") == true);
    EXPECT_FALSE(config.at("key2") == false);
    EXPECT_FALSE(config.at("key2") == 1);
    EXPECT_FALSE(config.at("key2") == kora::dynamic_t::null);

    EXPECT_TRUE(config.at("key3") == "~:{");
    EXPECT_FALSE(config.at("key3") == "~:<");
    EXPECT_FALSE(config.at("key3") == 1);
    EXPECT_FALSE(config.at("key3") == kora::dynamic_t::null);
}

TEST(Config, EqualityDynamicConfig) {
    kora::dynamic_t underlying = kora::dynamic_t::object_t();

    underlying.as_object()["key0"] = kora::dynamic_t::null;
    underlying.as_object()["key1"] = 5;
    underlying.as_object()["key2"] = true;
    underlying.as_object()["key3"] = "~:{";

    kora::config_t config("", underlying);

    EXPECT_TRUE(underlying == config);
    EXPECT_FALSE(5 == config);
    EXPECT_FALSE(kora::dynamic_t::null == config);

    EXPECT_TRUE(kora::dynamic_t::null == config.at("key0"));
    EXPECT_FALSE(true == config.at("key0"));
    EXPECT_FALSE(5 == config.at("key0"));

    EXPECT_TRUE(5 == config.at("key1"));
    EXPECT_FALSE(25 == config.at("key1"));
    EXPECT_FALSE(true == config.at("key1"));
    EXPECT_FALSE(kora::dynamic_t::null == config.at("key1"));

    EXPECT_TRUE(true == config.at("key2"));
    EXPECT_FALSE(false == config.at("key2"));
    EXPECT_FALSE(1 == config.at("key2"));
    EXPECT_FALSE(kora::dynamic_t::null == config.at("key2"));

    EXPECT_TRUE("~:{" == config.at("key3"));
    EXPECT_FALSE("~:<" == config.at("key3"));
    EXPECT_FALSE(1 == config.at("key3"));
    EXPECT_FALSE(kora::dynamic_t::null == config.at("key3"));
}

TEST(Config, InequalityConfigDynamic) {
    kora::dynamic_t underlying = kora::dynamic_t::object_t();

    underlying.as_object()["key0"] = kora::dynamic_t::null;
    underlying.as_object()["key1"] = 5;
    underlying.as_object()["key2"] = true;
    underlying.as_object()["key3"] = "~:{";

    kora::config_t config("", underlying);

    EXPECT_FALSE(config != underlying);
    EXPECT_TRUE(config != 5);
    EXPECT_TRUE(config != kora::dynamic_t::null);

    EXPECT_FALSE(config.at("key0") != kora::dynamic_t::null);
    EXPECT_TRUE(config.at("key0") != true);
    EXPECT_TRUE(config.at("key0") != 5);

    EXPECT_FALSE(config.at("key1") != 5);
    EXPECT_TRUE(config.at("key1") != 25);
    EXPECT_TRUE(config.at("key1") != true);
    EXPECT_TRUE(config.at("key1") != kora::dynamic_t::null);

    EXPECT_FALSE(config.at("key2") != true);
    EXPECT_TRUE(config.at("key2") != false);
    EXPECT_TRUE(config.at("key2") != 1);
    EXPECT_TRUE(config.at("key2") != kora::dynamic_t::null);

    EXPECT_FALSE(config.at("key3") != "~:{");
    EXPECT_TRUE(config.at("key3") != "~:<");
    EXPECT_TRUE(config.at("key3") != 1);
    EXPECT_TRUE(config.at("key3") != kora::dynamic_t::null);
}

TEST(Config, InequalityDynamicConfig) {
    kora::dynamic_t underlying = kora::dynamic_t::object_t();

    underlying.as_object()["key0"] = kora::dynamic_t::null;
    underlying.as_object()["key1"] = 5;
    underlying.as_object()["key2"] = true;
    underlying.as_object()["key3"] = "~:{";

    kora::config_t config("", underlying);

    EXPECT_FALSE(underlying != config);
    EXPECT_TRUE(5 != config);
    EXPECT_TRUE(kora::dynamic_t::null != config);

    EXPECT_FALSE(kora::dynamic_t::null != config.at("key0"));
    EXPECT_TRUE(true != config.at("key0"));
    EXPECT_TRUE(5 != config.at("key0"));

    EXPECT_FALSE(5 != config.at("key1"));
    EXPECT_TRUE(25 != config.at("key1"));
    EXPECT_TRUE(true != config.at("key1"));
    EXPECT_TRUE(kora::dynamic_t::null != config.at("key1"));

    EXPECT_FALSE(true != config.at("key2"));
    EXPECT_TRUE(false != config.at("key2"));
    EXPECT_TRUE(1 != config.at("key2"));
    EXPECT_TRUE(kora::dynamic_t::null != config.at("key2"));

    EXPECT_FALSE("~:{" != config.at("key3"));
    EXPECT_TRUE("~:<" != config.at("key3"));
    EXPECT_TRUE(1 != config.at("key3"));
    EXPECT_TRUE(kora::dynamic_t::null != config.at("key3"));
}

TEST(Config, Output) {
    kora::dynamic_t underlying = kora::dynamic_t::object_t();

    underlying.as_object()["key0"] = kora::dynamic_t::null;
    underlying.as_object()["key1"] = 5;
    underlying.as_object()["key2"] = true;
    underlying.as_object()["key3"] = "~:{";

    kora::config_t config("", underlying);

    std::ostringstream stream;

    {
        stream << config;

        std::istringstream json_stream(stream.str());
        auto parsed = kora::dynamic::read_json(json_stream);
        EXPECT_EQ(underlying, parsed);
    }

    stream.str("");
    stream << config.at("key0");
    EXPECT_EQ("null", stream.str());

    stream.str("");
    stream << config.at("key1");
    EXPECT_EQ("5", stream.str());

    stream.str("");
    stream << config.at("key2");
    EXPECT_EQ("1", stream.str());

    stream.str("");
    stream << config.at("key3");
    EXPECT_EQ("~:{", stream.str());
}
