/*
    Copyright (c) 2014 Artem Sokolov <derikon@yandex.ru>
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

#ifndef KORA_CONFIG_CONVERTERS_HPP
#define KORA_CONFIG_CONVERTERS_HPP

#include <chrono>
#include <cctype>

namespace kora { namespace dynamic {

//! \brief Converts dynamic_t to std::chrono::duration<Rep, Period>.
template<class Rep, class Period>
struct converter<std::chrono::duration<Rep, Period>> {
    typedef std::chrono::duration<Rep, Period> result_type;

    //! Doesn't call any controller's traverse methods.\n
    //! Fails with \p config_time_parse_error_t error if either not convertible to uint_t or
    //! or convertible to std::string but string format does not match ^\d+(ns|us|ms|s|m|h)$.\n
    //! \returns <tt>std::chrono::duration<Rep, Period></tt>
    template<class Controller>
    static inline
    result_type
    convert(const dynamic_t& from, Controller& controller) {
        if (from.convertible_to<dynamic_t::uint_t>()) {
            return result_type(from.to<dynamic_t::uint_t>());
        } else if (from.is_string()) {
            const auto &string = from.as_string();
            size_t pos = 0;
            Rep rep = Rep();

            try {
                rep = stoll(string, &pos);
            } catch (const std::invalid_argument &ex) {
                controller.fail(config_time_parse_error_t(
                            std::string("invalid_argument: ") + ex.what()), from);
            } catch (const std::out_of_range &ex) {
                controller.fail(config_time_parse_error_t(
                            std::string("out_of_range: ") + ex.what()), from);
            }

            auto suffix = string.substr(pos);

            if ((suffix) == "ns") {
                return std::chrono::duration_cast<result_type>(std::chrono::nanoseconds(rep));
            }

            if ((suffix) == "us") {
                return std::chrono::duration_cast<result_type>(std::chrono::microseconds(rep));
            }

            if ((suffix) == "ms") {
                return std::chrono::duration_cast<result_type>(std::chrono::milliseconds(rep));
            }

            if ((suffix) == "s") {
                return std::chrono::duration_cast<result_type>(std::chrono::seconds(rep));
            }

            if ((suffix) == "m") {
                return std::chrono::duration_cast<result_type>(std::chrono::minutes(rep));
            }

            if ((suffix) == "h") {
                return std::chrono::duration_cast<result_type>(std::chrono::hours(rep));
            }

            controller.fail(config_time_parse_error_t("invalid suffix"), from);
        } else {
            controller.fail(config_time_parse_error_t("invalid type"), from);
        }
    }

    //! \returns bool
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT {
        if (from.convertible_to<dynamic_t::uint_t>()) {
            return true;
        }

        if (from.is_string()) {
            const auto &string = from.as_string();
            size_t pos = 0;
            for (; pos != string.size(); ++pos) {
                if (!std::isdigit(string[pos])) {
                    break;
                }
            }

            if (pos == 0) {
                return false;
            }

            const auto &suffix = string.substr(pos);

            if (suffix == "ns" || suffix == "us" || suffix == "ms" || suffix == "s"
                    || suffix == "m" || suffix == "h") {
                return true;
            }
        }

        return false;
    }
};

}} // namespace kora::dynamic

#endif

