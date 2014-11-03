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

#ifndef KORA_CONFIG_HPP
#define KORA_CONFIG_HPP

#include "kora/dynamic.hpp"
#include "kora/utility.hpp"

KORA_PUSH_VISIBLE
#include <boost/lexical_cast.hpp>
KORA_POP_VISIBILITY

#include <sstream>
#include <vector>

namespace kora {

class KORA_API config_error : public std::exception
{
public:
    explicit config_error(std::string message);

    ~config_error() KORA_NOEXCEPT;

    const char *what() const KORA_NOEXCEPT;

private:
    std::string m_message;
};

class KORA_API config_parser_error : public config_error
{
public:
    config_parser_error(std::string message, std::string parse_error, size_t line_number, size_t column_number);

    ~config_parser_error() KORA_NOEXCEPT;

    const std::string &parse_error() const;

    size_t line_number() const;

    size_t column_number() const;

private:
    std::string m_parse_error;
    size_t m_line_number;
    size_t m_column_number;
};

// TODO: Drop this conversion code in favor of dynamic_converter.
namespace detail
{

struct config_conversion_controller_t {
    config_conversion_controller_t(const std::string &path) :
        m_root_path(path)
    { }

    void
    start_array(const dynamic_t&) {
        // Prepare place for the indices.
        m_backtrace.emplace_back(0);
    }

    void
    finish_array(){
        m_backtrace.pop_back();
    }

    void
    item(size_t index){
        m_backtrace.back() = index;
    }

    void
    start_object(const dynamic_t&){
        // Prepare place for the keys.
        m_backtrace.emplace_back(std::string());
    }

    void
    finish_object(){
        m_backtrace.pop_back();
    }

    void
    item(const std::string &key){
        m_backtrace.back() = key;
    }

    template<class Exception>
    KORA_NORETURN
    void
    fail(const Exception& e, const dynamic_t&) const {
        std::stringstream error;
        prepare_message(error);
        error << e.what();

        throw config_error(std::move(error.str()));
    }

    template<class TargetType>
    KORA_NORETURN
    void
    fail(const numeric_overflow_t<TargetType>&, const dynamic_t&) const {
        std::stringstream error;
        prepare_message(error);
        error << "the value must be an integer between "
              << std::numeric_limits<TargetType>::min() << " and "
              << std::numeric_limits<TargetType>::max();

        throw config_error(std::move(error.str()));
    }

    KORA_NORETURN
    void
    fail(const expected_tuple_t& e, const dynamic_t&) const {
        std::stringstream error;
        prepare_message(error);
        error << "the value must be an array of size " << e.expected_size();

        throw config_error(std::move(error.str()));
    }

private:
    void
    prepare_message(std::stringstream &stream) const {
        stream << "error in item " << buildup_path() << ": ";
    }

    std::string
    buildup_path() const {
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

private:
    const std::string &m_root_path;
    std::vector<boost::variant<size_t, std::string>> m_backtrace;
};

} // namespace detail

class config_t
{
public:
    KORA_API config_t(const std::string &path, const dynamic_t *value);

    KORA_API bool has(const std::string &name) const;
    KORA_API config_t at(const std::string &name) const;

    template <typename T>
    T at(const std::string &name, const T &default_value) const
    {
        if (!has(name))
            return default_value;

        return at(name).to<T>();
    }

    template <typename T>
    T at(const std::string &name) const
    {
        return at(name).to<T>();
    }

    KORA_API size_t size() const;
    KORA_API bool has(size_t index) const;
    KORA_API config_t at(size_t index) const;

    template <typename T>
    T at(size_t index, const T &default_value) const
    {
        if (!has(index))
            return default_value;

        return at(index).to<T>();
    }

    template <typename T>
    T at(size_t index) const
    {
        return at(index).to<T>();
    }

    template <typename T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to() const {
        return m_value.to<T>(detail::config_conversion_controller_t(m_path));
    }

    KORA_API const std::string &path() const;

    KORA_API std::string to_string() const;

    KORA_API void assert_array() const;
    KORA_API void assert_object() const;

protected:
    std::string m_path;
    const dynamic_t &m_value;
};

class config_parser_t
{
public:
    KORA_API config_parser_t();
    KORA_API ~config_parser_t();

    KORA_API void open(const std::string &path);
    KORA_API void parse(std::istream &stream);

    KORA_API config_t root() const;

private:
    dynamic_t m_root;
};

} // namespace kora

#endif
