/*
    Copyright (c) 2013-2014 Andrey Goryachev <andrey.goryachev@gmail.com>
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

#include "leonidia/dynamic.hpp"

#include <rapidjson/reader.h>
#include <rapidjson/writer.h>

using namespace leonidia;

const dynamic_t dynamic_t::null = dynamic_t::null_t();
const dynamic_t dynamic_t::empty_string = dynamic_t::string_t();
const dynamic_t dynamic_t::empty_array = dynamic_t::array_t();
const dynamic_t dynamic_t::empty_object = dynamic_t::object_t();

dynamic_t&
dynamic_t::object_t::at(const std::string& key, dynamic_t& default_) {
    auto it = find(key);

    if(it == end()) {
        return default_;
    } else {
        return it->second;
    }
}

const dynamic_t&
dynamic_t::object_t::at(const std::string& key, const dynamic_t& default_) const {
    auto it = find(key);

    if(it == end()) {
        return default_;
    } else {
        return it->second;
    }
}

const dynamic_t&
dynamic_t::object_t::operator[](const std::string& key) const {
    return at(key);
}

namespace {

struct move_visitor:
    public boost::static_visitor<>
{
    move_visitor(dynamic_t& destination) :
        m_destination(destination)
    { }

    template<class T>
    void
    operator()(T& v) const {
        m_destination = std::move(v);
    }

private:
    dynamic_t& m_destination;
};

struct assign_visitor:
    public boost::static_visitor<>
{
    assign_visitor(dynamic_t& destination) :
        m_destination(destination)
    { }

    template<class T>
    void
    operator()(T& v) const {
        m_destination = v;
    }

private:
    dynamic_t& m_destination;
};

struct equals_visitor:
    public boost::static_visitor<bool>
{
    equals_visitor(const dynamic_t& other) :
        m_other(other)
    { }

    bool
    operator()(const dynamic_t::null_t&) const {
        return m_other.is_null();
    }

    bool
    operator()(const dynamic_t::bool_t& v) const {
        return m_other.is_bool() && m_other.as_bool() == v;
    }

    bool
    operator()(const dynamic_t::int_t& v) const {
        if (m_other.is_int()) {
            return v == m_other.as_int();
        } else if (m_other.is_uint()) {
            return v >= 0 && static_cast<dynamic_t::uint_t>(v) == m_other.as_uint();
        } else {
            return m_other.is_double() && dynamic_t::double_t(v) == m_other.as_double();
        }
    }

    bool
    operator()(const dynamic_t::uint_t& v) const {
        if (m_other.is_uint()) {
            return v == m_other.as_uint();
        } else if (m_other.is_int()) {
            return m_other.as_int() >= 0 && v == m_other.to<dynamic_t::uint_t>();
        } else {
            return m_other.is_double() && dynamic_t::double_t(v) == m_other.as_double();
        }
    }

    bool
    operator()(const dynamic_t::double_t& v) const {
        if (m_other.is_uint()) {
            return v == dynamic_t::double_t(m_other.as_uint());
        } else if (m_other.is_int()) {
            return v == dynamic_t::double_t(m_other.as_int());
        } else {
            return m_other.is_double() && m_other.as_double() == v;
        }
    }

    bool
    operator()(const dynamic_t::string_t& v) const {
        return m_other.is_string() && m_other.as_string() == v;
    }

    bool
    operator()(const dynamic_t::array_t& v) const {
        return m_other.is_array() && m_other.as_array() == v;
    }

    bool
    operator()(const dynamic_t::object_t& v) const {
        return m_other.is_object() && m_other.as_object() == v;
    }

private:
    const dynamic_t& m_other;
};

} // namespace

dynamic_t::dynamic_t() :
    m_value(null_t())
{ }

dynamic_t::dynamic_t(const dynamic_t& other) :
    m_value(null_t())
{
    other.apply(assign_visitor(*this));
}

dynamic_t::dynamic_t(dynamic_t&& other) :
    m_value(null_t())
{
    other.apply(move_visitor(*this));
}

dynamic_t&
dynamic_t::operator=(const dynamic_t& other) {
    other.apply(assign_visitor(*this));
    return *this;
}

dynamic_t&
dynamic_t::operator=(dynamic_t&& other) {
    other.apply(move_visitor(*this));
    return *this;
}

bool
dynamic_t::operator==(const dynamic_t& other) const {
    return other.apply(equals_visitor(*this));
}

bool
dynamic_t::operator!=(const dynamic_t& other) const {
    return !other.apply(equals_visitor(*this));
}

dynamic_t::bool_t
dynamic_t::as_bool() const {
    return get<bool_t>();
}

dynamic_t::int_t
dynamic_t::as_int() const {
    return get<int_t>();
}

dynamic_t::uint_t
dynamic_t::as_uint() const {
    return get<uint_t>();
}

dynamic_t::double_t
dynamic_t::as_double() const {
    return get<double_t>();
}

const dynamic_t::string_t&
dynamic_t::as_string() const {
    return get<string_t>();
}

const dynamic_t::array_t&
dynamic_t::as_array() const {
    return get<detail::dynamic::incomplete_wrapper<array_t>>().get();
}

const dynamic_t::object_t&
dynamic_t::as_object() const {
    return get<detail::dynamic::incomplete_wrapper<object_t>>().get();
}

dynamic_t::string_t&
dynamic_t::as_string() {
    if(is_null()) {
        *this = string_t();
    }

    return get<string_t>();
}

dynamic_t::array_t&
dynamic_t::as_array() {
    return get<detail::dynamic::incomplete_wrapper<array_t>>().get();
}

dynamic_t::object_t&
dynamic_t::as_object() {
    return get<detail::dynamic::incomplete_wrapper<object_t>>().get();
}

bool
dynamic_t::is_null() const {
    return is<null_t>();
}

bool
dynamic_t::is_bool() const {
    return is<bool_t>();
}

bool
dynamic_t::is_int() const {
    return is<int_t>();
}

bool
dynamic_t::is_uint() const {
    return is<uint_t>();
}

bool
dynamic_t::is_double() const {
    return is<double_t>();
}

bool
dynamic_t::is_string() const {
    return is<string_t>();
}

bool
dynamic_t::is_array() const {
    return is<detail::dynamic::incomplete_wrapper<array_t>>();
}

bool
dynamic_t::is_object() const {
    return is<detail::dynamic::incomplete_wrapper<object_t>>();
}

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

typedef rapidjson::Writer<rapidjson_ostream_t> ostream_writer_t;

struct to_stream_visitor:
    public boost::static_visitor<>
{
    to_stream_visitor(ostream_writer_t *writer) :
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

        for(auto it = v.begin(); it != v.end(); ++it) {
            it->apply(*this);
        }

        m_writer->EndArray();
    }

    void
    operator()(const dynamic_t::object_t& v) const {
        m_writer->StartObject();

        for(auto it = v.begin(); it != v.end(); ++it) {
            m_writer->String(it->first.data(), it->first.size());
            it->second.apply(*this);
        }

        m_writer->EndObject();
    }

private:
    ostream_writer_t *m_writer;
};

} // namespace

dynamic_t
dynamic_t::from_json(std::istream &input) {
    rapidjson::MemoryPoolAllocator<> json_allocator;
    rapidjson::Reader json_reader(&json_allocator);
    rapidjson_istream_t json_stream(&input);

    json_to_dynamic_reader_t configuration_constructor;

    bool parse_success = json_reader.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseStreamFlag>(
        json_stream,
        configuration_constructor
    );

    if (!parse_success) {
        if (json_reader.HasParseError()) {
            throw json_parsing_error_t(json_stream.Tell(), json_reader.GetParseError());
        } else {
            throw json_parsing_error_t(json_stream.Tell(), "unknown error");
        }
    }

    return configuration_constructor.Result();
}

void
dynamic_t::to_json(std::ostream &output) const {
    rapidjson_ostream_t rapidjson_stream = &output;
    ostream_writer_t writer = rapidjson_stream;
    this->apply(to_stream_visitor(&writer));
}

std::ostream&
leonidia::operator<<(std::ostream& stream, const dynamic_t& value) {
    value.to_json(stream);
    return stream;
}
