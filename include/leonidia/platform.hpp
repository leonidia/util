/*
Copyright (c) 2011-2013 Andrey Sibiryov <me@kobology.ru>
Copyright (c) 2011-2013 Other contributors as noted in the AUTHORS file.

This file is part of Cocaine.

Cocaine is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Cocaine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LEONIDIA_PLATFORM_HPP
#define LEONIDIA_PLATFORM_HPP

#if defined(__GNUC__)
#if __GNUC__ == 4 && __GNUC_MINOR__ >= 4
#define LEONIDIA_HAVE_GCC44
#endif

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 6
#define LEONIDIA_HAVE_GCC46
#endif

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 7
#define LEONIDIA_HAVE_GCC47
#endif
#endif

#if defined(LEONIDIA_HAVE_GCC46) || defined(__clang__)
#define LEONIDIA_NOT_BAD
#endif

#endif

