/*
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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "leonidia/dynamic.hpp"
#include <boost/numeric/conversion/cast.hpp>

namespace leonidia {

class LEONIDIA_API config_error : public std::exception
{
public:
    explicit config_error(std::string message) : m_message(std::move(message))
    {
    }

    const char *what() const LEONIDIA_NOEXCEPT
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

class LEONIDIA_API config_parser_error : public config_error
{
public:
    config_parser_error(std::string message, std::string parse_error, size_t line_number, size_t column_number) :
        config_error(std::move(message)),
        m_parse_error(std::move(parse_error)),
        m_line_number(line_number),
        m_column_number(column_number)
    {
    }

    const std::string &parse_error() const
    {
        return m_parse_error;
    }

    size_t line_number() const
    {
        return m_line_number;
    }

    size_t column_number() const
    {
        return m_column_number;
    }

private:
    std::string m_parse_error;
    size_t m_line_number;
    size_t m_column_number;
};


namespace detail
{

enum {
    boolean_type = 1,
    integral_type = 2,
    floating_point_type = 4,
    string_type = 8,
    vector_type = 16
};

template <typename T, int specific>
struct config_value_caster_specific_helper;

template <typename T>
struct is_vector : public std::false_type { };

template <typename T>
struct is_vector<std::vector<T>> : public std::true_type { };


template <typename T>
struct config_value_caster_helper
{
    enum {
        boolean = std::is_same<T, bool>::value ? boolean_type : 0,
        integral = std::is_integral<T>::value && !boolean ? integral_type : 0,
        floating_point = std::is_floating_point<T>::value ? floating_point_type : 0,
        string = std::is_same<T, std::string>::value ? string_type : 0,
        vector = is_vector<T>::value ? vector_type : 0,
        type = integral | floating_point | string | boolean | vector
    };

    static_assert(integral || floating_point || string || boolean || vector, "Unsupported type");
    static_assert((type == integral) || (type == floating_point) || (type == string) || (type == boolean) || (type == vector), "Internal type check error");

    static T cast(const std::string &path, const dynamic_t &value)
    {
        return config_value_caster_specific_helper<T, type>::cast(path, value);
    }
};

template <typename T>
struct config_value_caster : public config_value_caster_helper<typename std::remove_all_extents<T>::type>
{
};

template <typename T>
struct config_value_caster_specific_helper<T, boolean_type>
{
    static T cast(const std::string &path, const dynamic_t &value)
    {
        if (!value.is_bool())
            throw config_error(path + " must be a bool");

        return value.as_bool();
    }
};

template <typename T>
struct config_value_caster_specific_helper<T, integral_type>
{
    static T cast(const std::string &path, const dynamic_t &value)
    {
        try {
            if (value.is_int()) {
                return boost::numeric_cast<T>(value.as_int());
            } else if (value.is_uint()) {
                return boost::numeric_cast<T>(value.as_uint());
            }
        } catch (boost::numeric::bad_numeric_cast &) {
            throw_limits_error(path);
        }

        throw_limits_error(path);
        return T();
    }

    static void throw_limits_error(const std::string &path)
    {
        std::stringstream error;
        error << path << " must be an integer between "
              << std::numeric_limits<T>::min() << " and " << std::numeric_limits<T>::max();
        throw config_error(error.str());
    }
};

template <typename T>
struct config_value_caster_specific_helper<T, floating_point_type>
{
    static T cast(const std::string &path, const dynamic_t &value)
    {
        if (!value.is_double())
            throw config_error(path + " must be a floating point number");

        return value.to<dynamic_t::double_t>();
    }
};

template <typename T>
struct config_value_caster_specific_helper<T, string_type>
{
    static T cast(const std::string &path, const dynamic_t &value)
    {
        if (!value.is_string())
            throw config_error(path + " must be a string");

        return value.as_string();
    }
};

template <typename T>
struct config_value_caster_specific_helper<T, vector_type>
{
    static T cast(const std::string &path, const dynamic_t &value)
    {
        typedef config_value_caster<typename T::value_type> caster;

        if (!value.is_array())
            throw config_error(path + " must be an array");

        const dynamic_t::array_t &array = value.as_array();

        T result;
        for (size_t i = 0; i < array.size(); ++i)
            result.emplace_back(caster::cast(path + "[" + std::to_string(i) + "]", array[i]));
        return result;
    }
};

}

class LEONIDIA_API config_t
{
public:
    config_t(const std::string &path, const dynamic_t *value);

    bool has(const std::string &name) const;
    config_t at(const std::string &name) const;

    template <typename T>
    T at(const std::string &name, const T &default_value) const
    {
        if (!has(name))
            return default_value;

        return at(name).as<T>();
    }

    template <typename T>
    T at(const std::string &name) const
    {
        return at(name).as<T>();
    }

    size_t size() const;
    bool has(size_t index) const;
    config_t at(size_t index) const;

    template <typename T>
    T at(size_t index, const T &default_value) const
    {
        if (!has(index))
            return default_value;

        return at(index).as<T>();
    }

    template <typename T>
    T at(size_t index) const
    {
        return at(index).as<T>();
    }

    template <typename T>
    T as() const
    {
        assert_valid();
        return detail::config_value_caster<T>::cast(m_path, m_value);
    }

    const std::string &path() const;

    std::string to_string() const;

    void assert_valid() const;
    void assert_array() const;
    void assert_object() const;

protected:
    std::string m_path;
    const dynamic_t &m_value;
};

class LEONIDIA_API config_parser_t
{
public:
    config_parser_t();
    ~config_parser_t();

    void open(const std::string &path);
    void parse(std::istream &stream);

    config_t root() const;

private:
    dynamic_t m_root;
};

} // namespace leonidia

#endif // CONFIG_HPP
