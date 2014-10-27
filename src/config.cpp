/*
Copyright (c) 2014 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#include "leonidia/config.hpp"

#include <fstream>
#include <stack>

using namespace leonidia;

config_error::config_error(std::string message) : m_message(std::move(message))
{
}

config_error::~config_error() LEONIDIA_NOEXCEPT
{
}

const char *config_error::what() const LEONIDIA_NOEXCEPT
{
    return m_message.c_str();
}

config_parser_error::config_parser_error(std::string message, std::string parse_error, size_t line_number, size_t column_number) :
    config_error(std::move(message)),
    m_parse_error(std::move(parse_error)),
    m_line_number(line_number),
    m_column_number(column_number)
{
}

config_parser_error::~config_parser_error() LEONIDIA_NOEXCEPT
{
}

const std::string &config_parser_error::parse_error() const
{
    return m_parse_error;
}

size_t config_parser_error::line_number() const
{
    return m_line_number;
}

size_t config_parser_error::column_number() const
{
    return m_column_number;
}


config_t::config_t(const std::string &path, const dynamic_t *value) :
    m_path(path), m_value(*value)
{
}

bool config_t::has(const std::string &name) const
{
    assert_object();

    const dynamic_t::object_t &object = m_value.as_object();

    return object.find(name) != object.end();
}

config_t config_t::at(const std::string &name) const
{
    const std::string path = m_path + "." + name;
    if (!has(name))
        throw config_error(path + " is missed");

    const dynamic_t::object_t &object = m_value.as_object();
    return config_t(path, &object.find(name)->second);
}

size_t config_t::size() const
{
    assert_array();
    return m_value.as_array().size();
}

bool config_t::has(size_t index) const
{
    assert_array();

    const dynamic_t::array_t &array = m_value.as_array();
    return index < array.size();
}

config_t config_t::at(size_t index) const
{
    const std::string path = m_path + "[" + std::to_string(index) + "]";

    if (!has(index))
        throw config_error(path + " is missed");

    const dynamic_t::array_t &array = m_value.as_array();
    return config_t(path, &array[index]);
}

const std::string &config_t::path() const
{
    return m_path;
}

std::string config_t::to_string() const
{
    assert_valid();

    std::string value_str;

    if (m_value.is_uint())
        value_str = std::to_string(m_value.as_uint());
    else if (m_value.is_int())
        value_str = std::to_string(m_value.as_int());
    else if (m_value.is_double())
        value_str = std::to_string(m_value.as_double());
    else if (m_value.is_string())
        value_str = m_value.to<dynamic_t::string_t>();
    else
        throw config_error(m_path + " has unknown type");

    return value_str;
}

void config_t::assert_valid() const
{
    if (m_value.is_null())
        throw config_error(m_path + " is missed");
}

void config_t::assert_array() const
{
    assert_valid();
    if (!m_value.is_array())
        throw config_error(m_path + " must be an array");
}

void config_t::assert_object() const
{
    assert_valid();
    if (!m_value.is_object())
        throw config_error(m_path + " must be an object");
}

config_parser_t::config_parser_t()
{
}

config_parser_t::~config_parser_t()
{
}

void config_parser_t::open(const std::string &path)
{
    std::ifstream stream(path.c_str());

    if (!stream) {
        throw config_error("failed to open config file: '" + path + "'");
    }

    parse(stream);
}

void config_parser_t::parse(std::istream &stream)
{
    try {
        m_root = leonidia::dynamic_t::from_json(stream);
    } catch (const leonidia::json_parsing_error_t& e) {
        stream.seekg(0);
        if (stream) {
            size_t offset = e.offset();
            std::vector<char> buffer(offset);
            stream.read(buffer.data(), offset);

            std::string data(buffer.begin(), buffer.end());
            std::string line;

            if (std::getline(stream, line))
                data += line;

            /*
             * Produce a pretty output about the error
             * including the line and certain place where
             * the error occured.
             */

            size_t line_offset = data.find_last_of('\n');
            if (line_offset == std::string::npos)
                line_offset = 0;
            else
                line_offset++;

            for (size_t i = line_offset; i < data.size(); ++i) {
                if (data[i] == '\t') {
                    data.replace(i, 1, std::string(4, ' '));

                    if (offset > i)
                        offset += 3;
                }
            }

            const size_t line_number = std::count(data.begin(), data.end(), '\n') + 1;
            const size_t dash_count = line_offset < offset ? offset - line_offset - 1 : 0;

            for (size_t i = 0; line_offset > 1 && i < 2; ++i) {
                line_offset = data.find_last_of('\n', line_offset - 2);
                if (line_offset == std::string::npos) {
                    line_offset = 0;
                } else {
                    line_offset++;
                }
            }

            std::stringstream error;
            error
                    << "parser error at line " << line_number << ": " << e.message() << std::endl
                    << data.substr(std::min(line_offset, data.size())) << std::endl
                    << std::string(dash_count, ' ') << '^' << std::endl
                    << std::string(dash_count, '~') << '+' << std::endl;
            throw config_parser_error(error.str(), e.message(), line_number, dash_count + 1);
        }

        throw config_error(std::string("parser error: at unknown line: ") + e.message());
    }

    if (!m_root.is_object())
        throw config_error("root must be an object");
}

config_t config_parser_t::root() const
{
    return config_t("path", &m_root);
}
