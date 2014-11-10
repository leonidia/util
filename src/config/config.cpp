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

#include "kora/config/config.hpp"
#include "kora/config/error.hpp"

KORA_PUSH_VISIBLE
#include <boost/lexical_cast.hpp>
KORA_POP_VISIBILITY

#include <sstream>

using namespace kora;
using namespace kora::detail;

config_conversion_controller_t::config_conversion_controller_t(const std::string &path) :
    m_root_path(path)
{ }

void
config_conversion_controller_t::start_array(const dynamic_t&) {
    // Prepare place for the indices.
    m_backtrace.emplace_back(0);
}

void
config_conversion_controller_t::finish_array(){
    m_backtrace.pop_back();
}

void
config_conversion_controller_t::item(size_t index){
    m_backtrace.back() = index;
}

void
config_conversion_controller_t::start_object(const dynamic_t&){
    // Prepare place for the keys.
    m_backtrace.emplace_back(std::string());
}

void
config_conversion_controller_t::finish_object(){
    m_backtrace.pop_back();
}

void
config_conversion_controller_t::item(const std::string &key){
    m_backtrace.back() = key;
}

KORA_NORETURN
void
config_conversion_controller_t::fail(const expected_tuple_t& e, const dynamic_t&) const {
    std::stringstream error;
    error << "the value must be an array of size " << e.expected_size();

    throw config_cast_error_t(buildup_path(), std::move(error.str()));
}

KORA_NORETURN
void
config_conversion_controller_t::throw_config_error(const char *message) const {
    throw config_cast_error_t(buildup_path(), message);
}

KORA_NORETURN
void
config_conversion_controller_t::throw_integer_overflow_error(const char *min, const char *max) const {
    std::stringstream error;
    error << "the value must be an integer between " << min << " and " << max;

    throw config_cast_error_t(buildup_path(), std::move(error.str()));
}

KORA_NORETURN
void
config_conversion_controller_t::throw_float_overflow_error(const char *min, const char *max) const {
    std::stringstream error;
    error << "the value must be a number between " << min << " and " << max;

    throw config_cast_error_t(buildup_path(), std::move(error.str()));
}

std::string
config_conversion_controller_t::buildup_path() const {
    std::stringstream path;

    path << m_root_path;

    for (auto it = m_backtrace.begin(); it != m_backtrace.end(); ++it) {
        if (boost::get<size_t>(&(*it))) {
            path << "[" << boost::get<size_t>(*it) << "]";
        } else {
            path << "." << boost::get<std::string>(*it);
        }
    }

    return path.str();
}

config_t::config_t(const std::string &path, const dynamic_t &value) :
    m_path(path),
    m_value(value)
{ }

config_t::config_t(config_t&& other) KORA_NOEXCEPT :
    m_path(std::move(other.m_path)),
    m_value(other.m_value)
{ }

bool
config_t::has(const std::string &name) const {
    const auto &object = this->to<dynamic_t::object_t>();

    return object.find(name) != object.end();
}

config_t
config_t::at(const std::string &name) const {
    const std::string path = m_path + "." + name;

    if (!has(name)) {
        throw config_access_error_t(path, "the value is missed");
    }

    return config_t(path, m_value.as_object().find(name)->second);
}

size_t
config_t::size() const {
    if (m_value.is_string()) {
        return m_value.as_string().size();
    } else if (m_value.is_array()) {
        return m_value.as_array().size();
    } else if (m_value.is_object()) {
        return m_value.as_object().size();
    } else {
        throw config_cast_error_t(m_path, "the value expected to be a string, an array or an object");
    }
}

config_t
config_t::at(size_t index) const {
    const auto &array = this->to<dynamic_t::array_t>();

    const std::string path = m_path + "[" + boost::lexical_cast<std::string>(index) + "]";

    if (index >= array.size()) {
        throw config_access_error_t(path, "the value is missed");
    }

    return config_t(path, array[index]);
}

const std::string&
config_t::path() const KORA_NOEXCEPT {
    return m_path;
}

const dynamic_t&
config_t::underlying_object() const KORA_NOEXCEPT {
    return m_value;
}
