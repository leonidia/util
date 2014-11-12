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

#include "kora/utility/noreturn.hpp"

#include <stdexcept>

namespace {

KORA_NORETURN
void
throw_error() {
    throw std::runtime_error("test");
}

// This should compile without errors and warnings.
int
test_function(int x) {
    if (x == 5) {
        throw_error();
    } else {
        return 2 * x;
    }
}

}

TEST(Noreturn, SimpleTest) {
    test_function(2);
    SUCCEED();
}
