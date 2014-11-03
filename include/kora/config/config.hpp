/*
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

#ifndef KORA_CONFIG_CONFIG_HPP
#define KORA_CONFIG_CONFIG_HPP

#include "kora/config/error.hpp"

#include "kora/dynamic.hpp"
#include "kora/utility.hpp"

KORA_PUSH_VISIBLE
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
KORA_POP_VISIBILITY

#include <type_traits>
#include <vector>

namespace kora {

namespace detail
{

struct config_conversion_controller_t {
    KORA_API
    config_conversion_controller_t(const std::string &path);

    KORA_API
    void
    start_array(const dynamic_t&);

    KORA_API
    void
    finish_array();

    KORA_API
    void
    item(size_t index);

    KORA_API
    void
    start_object(const dynamic_t&);

    KORA_API
    void
    finish_object();

    KORA_API
    void
    item(const std::string &key);

    template<class Exception>
    KORA_NORETURN
    void
    fail(const Exception& e, const dynamic_t&) const {
        throw_config_error(e.what());
    }

    template<class TargetType>
    KORA_NORETURN
    void
    fail(const numeric_overflow_t<TargetType>&, const dynamic_t&) const {
        if (std::is_floating_point<TargetType>::value) {
            throw_float_overflow_error(
                boost::lexical_cast<std::string>(std::numeric_limits<TargetType>::min()).c_str(),
                boost::lexical_cast<std::string>(std::numeric_limits<TargetType>::max()).c_str()
            );
        } else {
            throw_integer_overflow_error(
                boost::lexical_cast<std::string>(std::numeric_limits<TargetType>::min()).c_str(),
                boost::lexical_cast<std::string>(std::numeric_limits<TargetType>::max()).c_str()
            );
        }
    }

    KORA_API
    KORA_NORETURN
    void
    fail(const expected_tuple_t& e, const dynamic_t&) const;

private:
    KORA_API
    KORA_NORETURN
    void
    throw_integer_overflow_error(const char *min, const char *max) const;

    KORA_API
    KORA_NORETURN
    void
    throw_float_overflow_error(const char *min, const char *max) const;

    KORA_API
    KORA_NORETURN
    void
    throw_config_error(const char *message) const;

    KORA_API
    void
    prepare_message(std::stringstream &stream) const;

    KORA_API
    std::string
    buildup_path() const;

private:
    const std::string &m_root_path;
    std::vector<boost::variant<size_t, std::string>> m_backtrace;
};

} // namespace detail

class config_t {
public:
    KORA_API
    config_t(const std::string &path, const dynamic_t &value);

    KORA_API
    bool
    has(const std::string &name) const;

    KORA_API
    config_t
    at(const std::string &name) const;

    template <typename T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    at(const std::string &name, const T &default_value) const {
        if (!has(name)) {
            return default_value;
        }

        return at(name).to<T>();
    }

    template <typename T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    at(const std::string &name) const {
        return at(name).to<T>();
    }

    KORA_API
    size_t
    size() const;

    KORA_API
    bool
    has(size_t index) const;

    KORA_API
    config_t
    at(size_t index) const;

    template <typename T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    at(size_t index, const T &default_value) const {
        if (!has(index)) {
            return default_value;
        }

        return at(index).to<T>();
    }

    template <typename T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    at(size_t index) const {
        return at(index).to<T>();
    }

    template <typename T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to() const {
        return m_value.to<T>(detail::config_conversion_controller_t(m_path));
    }

    KORA_API
    const std::string&
    path() const;

    KORA_API
    std::string
    to_string() const;

    KORA_API
    void
    assert_array() const;

    KORA_API
    void
    assert_object() const;

protected:
    std::string m_path;
    const dynamic_t &m_value;
};

} // namespace kora

#endif
