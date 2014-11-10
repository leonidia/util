/*
Copyright (c) 2011-2013 Andrey Sibiryov <me@kobology.ru>
Copyright (c) 2011-2013 Other contributors as noted in the AUTHORS file.

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

#ifndef KORA_UTILITY_PLATFORM_HPP
#define KORA_UTILITY_PLATFORM_HPP

/*! \file platform.hpp
 *
 * Defines macros to detect current compiler.
 * These macros are used to workaround bugs and limitations of specific compilers.
 */

 /*!
  * \def KORA_HAVE_GCCXY
  *
  * These macros are defined when we have GCC of version not older than X.Y.
  */

#if defined(__GNUC__)
#if __GNUC__ == 4 && __GNUC_MINOR__ >= 4
#define KORA_HAVE_GCC44
#endif

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 6
#define KORA_HAVE_GCC46
#endif

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 7
#define KORA_HAVE_GCC47
#endif
#endif

 /*!
  * \def KORA_NOT_BAD
  *
  * This macro is defined when we are not on g++ 4.4. Because when we are, it's very bad.
  */

#if !defined(KORA_HAVE_GCC44) || defined(KORA_HAVE_GCC46)
#define KORA_NOT_BAD
#endif

#endif
