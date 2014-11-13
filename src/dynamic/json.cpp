/*
    Copyright (c) 2013-2014 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#include "kora/dynamic/constructors.hpp"
#include "kora/dynamic/error.hpp"
#include "kora/dynamic/json.hpp"

#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include <algorithm>
#include <stack>

using namespace kora;

namespace {

struct json_to_dynamic_reader_t {
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
        m_stack.emplace(v);
    }

    void
    Int64(int64_t v) {
        m_stack.emplace(v);
    }

    void
    Uint64(uint64_t v) {
        m_stack.emplace(v);
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

        for (size_t i = 0; i < size; ++i) {
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

        for (size_t i = size; i != 0; --i) {
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

struct rapidjson_istream_t {
    rapidjson_istream_t(std::istream *backend) :
        m_backend(backend),
        m_offset(0)
    { }

    char
    Peek() const {
        int next = m_backend->peek();

        if (next == std::char_traits<char>::eof()) {
            return '\0';
        } else {
            return next;
        }
    }

    char
    Take() {
        int next = m_backend->get();

        if (next == std::char_traits<char>::eof()) {
            return '\0';
        } else {
            ++m_offset;
            return next;
        }
    }

    size_t
    Tell() const {
        return m_offset;
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
    std::istream *m_backend;
    size_t m_offset;
};

struct rapidjson_ostream_t {
    rapidjson_ostream_t(std::ostream *backend) :
        m_backend(backend)
    { }

    char
    Peek() const {
        assert(false);
        return 0;
    }

    char
    Take() {
        assert(false);
        return 0;
    }

    size_t
    Tell() const {
        assert(false);
        return 0;
    }

    char*
    PutBegin() {
        assert(false);
        return 0;
    }

    void
    Put(char c) {
        m_backend->put(c);
    }

    size_t
    PutEnd(char*) {
        assert(false);
        return 0;
    }

private:
    std::ostream *m_backend;
};

typedef rapidjson::Writer<rapidjson_ostream_t> ostream_simple_writer_t;
typedef rapidjson::PrettyWriter<rapidjson_ostream_t> ostream_pretty_writer_t;

template<class Writer>
struct to_stream_visitor:
    public boost::static_visitor<>
{
    to_stream_visitor(Writer *writer) :
        m_writer(writer)
    { }

    void
    operator()(const dynamic_t::null_t&) const {
        m_writer->Null();
    }

    void
    operator()(const dynamic_t::bool_t& v) const {
        m_writer->Bool(v);
    }

    void
    operator()(const dynamic_t::int_t& v) const {
        m_writer->Int64(v);
    }

    void
    operator()(const dynamic_t::uint_t& v) const {
        m_writer->Uint64(v);
    }

    void
    operator()(const dynamic_t::double_t& v) const {
        m_writer->Double(v);
    }

    void
    operator()(const dynamic_t::string_t& v) const {
        m_writer->String(v.data(), v.size());
    }

    void
    operator()(const dynamic_t::array_t& v) const {
        m_writer->StartArray();

        for (auto it = v.begin(); it != v.end(); ++it) {
            it->apply(*this);
        }

        m_writer->EndArray();
    }

    void
    operator()(const dynamic_t::object_t& v) const {
        m_writer->StartObject();

        for (auto it = v.begin(); it != v.end(); ++it) {
            m_writer->String(it->first.data(), it->first.size());
            it->second.apply(*this);
        }

        m_writer->EndObject();
    }

private:
    Writer *m_writer;
};

} // namespace

dynamic_t
kora::dynamic::from_json(std::istream &input) {
    rapidjson::MemoryPoolAllocator<> json_allocator;
    rapidjson::Reader json_reader(&json_allocator);
    rapidjson_istream_t json_stream(&input);

    json_to_dynamic_reader_t configuration_constructor;

    bool parse_success = json_reader.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseStreamFlag>(
        json_stream,
        configuration_constructor
    );

    if (!parse_success) {
        size_t error_offset = std::max<size_t>(1, json_stream.Tell()) - 1;

        if (json_reader.HasParseError()) {
            throw json_parsing_error_t(error_offset, json_reader.GetParseError());
        } else {
            throw json_parsing_error_t(error_offset, "unknown error");
        }
    }

    return configuration_constructor.Result();
}

void
kora::write_json(std::ostream &output, const dynamic_t& value) {
    rapidjson_ostream_t rapidjson_stream = &output;
    ostream_simple_writer_t writer = rapidjson_stream;
    writer.SetFlags(rapidjson::kSerializeAnyValueFlag);
    value.apply(to_stream_visitor<ostream_simple_writer_t>(&writer));
}

void
kora::write_pretty_json(std::ostream &output, const dynamic_t& value, size_t indent) {
    rapidjson_ostream_t rapidjson_stream = &output;
    ostream_pretty_writer_t writer = rapidjson_stream;
    writer.SetFlags(rapidjson::kSerializeAnyValueFlag);
    writer.SetIndent(' ', indent);
    value.apply(to_stream_visitor<ostream_pretty_writer_t>(&writer));
}

namespace {

struct print_vistor_t:
    public boost::static_visitor<>
{
    print_vistor_t(std::ostream& stream) :
        m_stream(stream)
    { }

    template<class T>
    void
    operator()(const T& v) const {
        m_stream << v;
    }

private:
    std::ostream& m_stream;
};

} // namespace

std::ostream&
kora::operator<<(std::ostream& stream, const dynamic_t& value) {
    if (value.is_null() || value.is_array() || value.is_object()) {
        write_json(stream, value);
    } else {
        value.apply(print_vistor_t(stream));
    }

    return stream;
}
