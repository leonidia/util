/*
Copyright (c) 2013 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#ifndef KORA_UTILITY_VISIBILITY_HPP
#define KORA_UTILITY_VISIBILITY_HPP

/*! \file visibility.hpp
 *
 * Provides macros to control symbols visibility.
 */

/*!
 * \def KORA_API
 *
 * Declares function or entire class as visible.
 *
 * Example: <tt>class KORA_API my_class { ... };</tt>
 *
 * Example: <tt>KORA_API void my_function();</tt>
 */

/*!
 * \def KORA_PUSH_VISIBLE
 *
 * Makes all succeeding declarations visible.
 * It's useful when you want to make big part of code visible.
 * For example it may be entire header file with declaration of your exception classes.
 * Or you may want to make visible symbols from third-party header file which include (to make exceptions visible).
 *
 * Example: <tt>\n
 * KORA_PUSH_VISIBLE\n
 * #include <boost/lexical_cast.hpp>\n
 * KORA_POP_VISIBILITY\n
 * </tt>
 *
 * \sa KORA_POP_VISIBILITY
 */

/*!
 * \def KORA_POP_VISIBILITY
 *
 * Restores previous visibility after \p KORA_PUSH_VISIBLE.
 *
 * \sa KORA_PUSH_VISIBLE
 */


#ifdef __GNUC__
    #define KORA_API __attribute__ ((__visibility__ ("default")))
    #define KORA_PUSH_VISIBLE _Pragma("GCC visibility push(default)")
    #define KORA_POP_VISIBILITY _Pragma("GCC visibility pop")
#else
    #define KORA_API
    #define KORA_PUSH_VISIBLE
    #define KORA_POP_VISIBILITY
#endif

#endif
