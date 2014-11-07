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

#include "kora/config.hpp"
#include "kora/config/parser.hpp"

KORA_PUSH_VISIBLE
#include <boost/lexical_cast.hpp>
KORA_POP_VISIBILITY

#include <fstream>
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
    prepare_message(error);
    error << "the value must be an array of size " << e.expected_size();

    throw config_error_t(std::move(error.str()));
}

KORA_NORETURN
void
config_conversion_controller_t::throw_config_error(const char *message) const {
    std::stringstream error;
    prepare_message(error);
    error << message;

    throw config_error_t(std::move(error.str()));
}

KORA_NORETURN
void
config_conversion_controller_t::throw_integer_overflow_error(const char *min, const char *max) const {
    std::stringstream error;
    prepare_message(error);
    error << "the value must be an integer between " << min << " and " << max;

    throw config_error_t(std::move(error.str()));
}

KORA_NORETURN
void
config_conversion_controller_t::throw_float_overflow_error(const char *min, const char *max) const {
    std::stringstream error;
    prepare_message(error);
    error << "the value must be a number between " << min << " and " << max;

    throw config_error_t(std::move(error.str()));
}

void
config_conversion_controller_t::prepare_message(std::stringstream &stream) const {
    stream << "error in item " << buildup_path() << ": ";
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

config_error_t::config_error_t(std::string message) : m_message(std::move(message))
{
}

config_error_t::~config_error_t() KORA_NOEXCEPT
{
}

const char *config_error_t::what() const KORA_NOEXCEPT
{
    return m_message.c_str();
}

config_parser_error_t::config_parser_error_t(std::string message, std::string parse_error, size_t line_number, size_t column_number) :
    config_error_t(std::move(message)),
    m_parse_error(std::move(parse_error)),
    m_line_number(line_number),
    m_column_number(column_number)
{
}

config_parser_error_t::~config_parser_error_t() KORA_NOEXCEPT
{
}

const std::string &config_parser_error_t::parse_error() const
{
    return m_parse_error;
}

size_t config_parser_error_t::line_number() const
{
    return m_line_number;
}

size_t config_parser_error_t::column_number() const
{
    return m_column_number;
}


config_t::config_t(const std::string &path, const dynamic_t &value) :
    m_path(path), m_value(value)
{
}

bool config_t::has(const std::string &name) const
{
    const auto &object = this->to<dynamic_t::object_t>();

    return object.find(name) != object.end();
}

config_t config_t::at(const std::string &name) const
{
    const std::string path = m_path + "." + name;
    if (!has(name))
        throw config_error_t(path + " is missed");

    return config_t(path, m_value.as_object().find(name)->second);
}

namespace {

    class KORA_API expected_nonscalar_t :
        public bad_cast_t
    {
    public:
        ~expected_nonscalar_t() KORA_NOEXCEPT { }

        virtual
        const char*
        what() const KORA_NOEXCEPT {
            return "the value expected to be a string, an array or an object";
        }
    };

    struct size_tag_t { };

} // namespace

namespace kora {

    template<>
    struct dynamic_converter<size_tag_t> {
        typedef size_t result_type;

        template<class Controller>
        static inline
        result_type
        convert(const dynamic_t& from, Controller& controller) {
            if (from.is_string()) {
                return from.as_string().size();
            } else if (from.is_array()) {
                return from.as_array().size();
            } else if (from.is_object()) {
                return from.as_object().size();
            } else {
                controller.fail(expected_nonscalar_t(), from);
            }
        }

        static inline
        bool
        convertible(const dynamic_t& from) {
            return from.is_string() || from.is_array() || from.is_object();
        }
    };

} // namespace kora

size_t config_t::size() const
{
    // Here we have a weird  way to get size of the underlying value,
    // but the type check is performed via the same code as all other ones.
    return this->to<size_tag_t>();
}

config_t config_t::at(size_t index) const
{
    const auto &array = this->to<dynamic_t::array_t>();

    const std::string path = m_path + "[" + boost::lexical_cast<std::string>(index) + "]";

    if (index >= array.size())
        throw config_error_t(path + " is missed");

    return config_t(path, array[index]);
}

const std::string &config_t::path() const
{
    return m_path;
}

const dynamic_t &config_t::underlying_object() const
{
    return m_value;
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
        throw std::runtime_error("failed to open config file: '" + path + "'");
    }

    parse(stream);
}

void config_parser_t::parse(std::istream &stream)
{
    try {
        m_root = kora::dynamic_t::from_json(stream);
    } catch (const kora::json_parsing_error_t& e) {
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
            throw config_parser_error_t(error.str(), e.message(), line_number, dash_count + 1);
        }

        throw config_error_t(std::string("parser error: at unknown line: ") + e.message());
    }

    if (!m_root.is_object())
        throw config_error_t("<root> must be an object");
}

config_t config_parser_t::root() const
{
    return config_t("<root>", m_root);
}

std::ostream&
kora::operator<<(std::ostream& stream, const config_t& value) {
    stream << value.underlying_object();
    return stream;
}
