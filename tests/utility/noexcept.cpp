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

#include "kora/utility/noexcept.hpp"

#include <exception>

namespace {

struct test_exception_t :
    std::exception
{
    ~test_exception_t() KORA_NOEXCEPT { }

    // This method must be noexcept, so if it compiles then KORA_NOEXCEPT works.
    virtual
    const char*
    what() const KORA_NOEXCEPT {
        return "test";
    }
};

}

TEST(Noexcept, SimpleTest) {
    test_exception_t a;
    (void)a;
    SUCCEED();
}
