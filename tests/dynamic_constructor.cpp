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

#include <limits>

TEST(DynamicConstructor, UnsignedNumbers) {
    {
        leonidia::dynamic_t constructed = (unsigned char)13;
        EXPECT_TRUE(constructed.is_uint());
        EXPECT_EQ(13, constructed.as_uint());

        leonidia::dynamic_t assigned;
        assigned = (unsigned char)13;
        EXPECT_TRUE(assigned.is_uint());
        EXPECT_EQ(13, assigned.as_uint());
    }
    {
        leonidia::dynamic_t constructed = (unsigned short)13;
        EXPECT_TRUE(constructed.is_uint());
        EXPECT_EQ(13, constructed.as_uint());

        leonidia::dynamic_t assigned;
        assigned = (unsigned short)13;
        EXPECT_TRUE(assigned.is_uint());
        EXPECT_EQ(13, assigned.as_uint());
    }
    {
        leonidia::dynamic_t constructed = (unsigned int)13;
        EXPECT_TRUE(constructed.is_uint());
        EXPECT_EQ(13, constructed.as_uint());

        leonidia::dynamic_t assigned;
        assigned = (unsigned int)13;
        EXPECT_TRUE(assigned.is_uint());
        EXPECT_EQ(13, assigned.as_uint());
    }
}

TEST(DynamicConstructor, SignedNumbers) {
    {
        leonidia::dynamic_t constructed = (char)-13;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(-13, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = (char)-13;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(-13, assigned.as_int());
    }
    {
        leonidia::dynamic_t constructed = (short)-13;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(-13, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = (short)-13;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(-13, assigned.as_int());
    }
    {
        leonidia::dynamic_t constructed = (int)-13;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(-13, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = (int)-13;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(-13, assigned.as_int());
    }
}

TEST(DynamicConstructor, Enum) {
    {
        enum enum1_t {
            const1 = 1,
            const2,
            const3
        };

        leonidia::dynamic_t constructed = const1;
        EXPECT_TRUE(constructed.is_int());
        EXPECT_EQ(1, constructed.as_int());

        leonidia::dynamic_t assigned;
        assigned = const2;
        EXPECT_TRUE(assigned.is_int());
        EXPECT_EQ(2, assigned.as_int());
    }
}

TEST(DynamicConstructor, Float) {
    {
        leonidia::dynamic_t constructed = (float)2.0;
        EXPECT_TRUE(constructed.is_double());
        EXPECT_DOUBLE_EQ(2.0, constructed.as_double());

        leonidia::dynamic_t assigned;
        assigned = (float)(-2.0);
        EXPECT_TRUE(assigned.is_double());
        EXPECT_DOUBLE_EQ(-2.0, assigned.as_double());
    }
    {
        leonidia::dynamic_t constructed = (long double)2.0;
        EXPECT_TRUE(constructed.is_double());
        EXPECT_DOUBLE_EQ(2.0, constructed.as_double());

        leonidia::dynamic_t assigned;
        assigned = (long double)(-2.0);
        EXPECT_TRUE(assigned.is_double());
        EXPECT_DOUBLE_EQ(-2.0, assigned.as_double());
    }
}

TEST(DynamicConstructor, FloatOverflow) {
    if (std::numeric_limits<long double>::max() > std::numeric_limits<double>::max()) {
        EXPECT_THROW(leonidia::dynamic_t dynamic = std::numeric_limits<long double>::max(),
                     boost::numeric::positive_overflow);

        leonidia::dynamic_t assigned;
        EXPECT_THROW(assigned = std::numeric_limits<long double>::max(),
                     boost::numeric::positive_overflow);
    } else {
        std::cout << "NOTICE: FloatOverflow test is not performed because long double is equal to double type."
                  << std::endl;
        SUCCEED();
    }
}

TEST(DynamicConstructor, StringLiteral) {
    leonidia::dynamic_t constructed = "xdd";
    EXPECT_TRUE(constructed.is_string());
    EXPECT_EQ("xdd", constructed.as_string());

    leonidia::dynamic_t assigned;
    assigned = "xdd";
    EXPECT_TRUE(assigned.is_string());
    EXPECT_EQ("xdd", assigned.as_string());
}
