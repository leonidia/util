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

#include <rapidjson/reader.h>
#include <rapidjson/filestream.h>

#include <fstream>
#include <stack>

using namespace leonidia;

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

namespace {

struct dynamic_reader_t {
    void
    Null() {
        m_stack.emplace(dynamic_t::null);
    }

    void
    Bool(bool v) {
        m_stack.emplace(v);
    }

    void
    Int(int v) {
        m_stack.emplace(v);
    }

    void
    Uint(unsigned v) {
        m_stack.emplace(dynamic_t::uint_t(v));
    }

    void
    Int64(int64_t v) {
        m_stack.emplace(v);
    }

    void
    Uint64(uint64_t v) {
        m_stack.emplace(dynamic_t::uint_t(v));
    }

    void
    Double(double v) {
        m_stack.emplace(v);
    }

    void
    String(const char* data, size_t size, bool) {
        m_stack.emplace(dynamic_t::string_t(data, size));
    }

    void
    StartObject() {
        // Empty.
    }

    void
    EndObject(size_t size) {
        dynamic_t::object_t object;

        for(size_t i = 0; i < size; ++i) {
            dynamic_t value = std::move(m_stack.top());
            m_stack.pop();

            std::string key = std::move(m_stack.top().as_string());
            m_stack.pop();

            object[key] = std::move(value);
        }

        m_stack.emplace(std::move(object));
    }

    void
    StartArray() {
        // Empty.
    }

    void
    EndArray(size_t size) {
        dynamic_t::array_t array(size);

        for(size_t i = size; i != 0; --i) {
            array[i - 1] = std::move(m_stack.top());
            m_stack.pop();
        }

        m_stack.emplace(std::move(array));
    }

    dynamic_t
    Result() {
        return m_stack.top();
    }

private:
    std::stack<dynamic_t> m_stack;
};

struct rapidjson_ifstream_t {
    rapidjson_ifstream_t(std::istream& backend) :
        m_backend(&backend)
    { }

    char
    Peek() const {
        int next = m_backend->peek();

        if(next == std::char_traits<char>::eof()) {
            return '\0';
        } else {
            return next;
        }
    }

    char
    Take() {
        int next = m_backend->get();

        if(next == std::char_traits<char>::eof()) {
            return '\0';
        } else {
            return next;
        }
    }

    size_t
    Tell() const {
        return m_backend->gcount();
    }

    char*
    PutBegin() {
        assert(false);
        return 0;
    }

    void
    Put(char) {
        assert(false);
    }

    size_t
    PutEnd(char*) {
        assert(false);
        return 0;
    }

private:
    std::istream* m_backend;
};

} // namespace

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
    rapidjson::MemoryPoolAllocator<> json_allocator;
    rapidjson::Reader json_reader(&json_allocator);
    rapidjson_ifstream_t config_stream(stream);
    dynamic_reader_t config_constructor;

    json_reader.Parse<rapidjson::kParseDefaultFlags>(config_stream, config_constructor);

    if (json_reader.HasParseError()) {
        stream.seekg(0);
        if (stream) {
            size_t offset = json_reader.GetErrorOffset();
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

            for (size_t i = line_offset; i < data.size(); ++i) {
                if (data[i] == '\t') {
                    data.replace(i, 1, std::string(4, ' '));

                    if (offset > i)
                        offset += 3;
                }
            }

            const size_t line_number = std::count(data.begin(), data.end(), '\n') + 1;
            const size_t dash_count = line_offset < offset ? offset - line_offset - 1 : 0;

            std::stringstream error;
            error
                    << "parser error at line " << line_number << ": " << json_reader.GetParseError() << std::endl
                    << data.substr(std::min(line_offset + 1, data.size())) << std::endl
                    << std::string(dash_count, ' ') << '^' << std::endl
                    << std::string(dash_count, '~') << '+' << std::endl;
            throw config_error(error.str());
        }

        throw config_error(std::string("parser error: at unknown line: ") + json_reader.GetParseError());
    }

    m_root = config_constructor.Result();

    if (!m_root.is_object())
        throw config_error("root must be an object");
}

config_t config_parser_t::root() const
{
    return config_t("path", &m_root);
}
