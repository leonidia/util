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

TEST(DynamicConverter, Dynamic) {
    kora::dynamic_t source = kora::dynamic_t::empty_object;

    source.as_object()["key1"] = 1337;
    source.as_object()["key2"] = "._____.";

    auto destination = source.to<kora::dynamic_t>();

    static_assert(
        std::is_same<kora::dynamic_t, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type kora::dynamic_t."
    );

    ASSERT_TRUE(destination.is_object());
    EXPECT_EQ(destination.as_object()["key1"], 1337);
    EXPECT_EQ(destination.as_object()["key2"], "._____.");
}

TEST(DynamicConverter, Bool) {
    kora::dynamic_t source = true;

    auto destination = source.to<bool>();

    static_assert(
        std::is_same<bool, typename std::decay<decltype(destination)>::type>::value,
        "Expected result of type bool."
    );

    EXPECT_EQ(destination, true);
}
