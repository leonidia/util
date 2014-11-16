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

KORA_PUSH_VISIBLE
#include <boost/iostreams/char_traits.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/filtering_stream.hpp>
KORA_POP_VISIBILITY

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

using namespace kora;

class config_parser_t::implementation_t {
public:
    dynamic_t root;
};

config_parser_t::config_parser_t() KORA_NOEXCEPT { }

config_parser_t::config_parser_t(const config_parser_t &other) {
    *this = other;
}

config_parser_t::config_parser_t(const std::string &path) {
    open(path);
}

config_parser_t::config_parser_t(std::istream &stream) {
    parse(stream);
}

config_parser_t::~config_parser_t() KORA_NOEXCEPT { }

config_parser_t&
config_parser_t::operator=(const config_parser_t &other) {
    if (other.m_impl) {
        m_impl.reset(new config_parser_t::implementation_t(*other.m_impl));
    } else {
        m_impl.reset();
    }

    return *this;
}

config_t
config_parser_t::open(const std::string &path) {
    std::ifstream stream(path.c_str());

    if (!stream) {
        throw std::runtime_error("failed to open config file: '" + path + "'");
    }

    return parse(stream);
}

namespace {

class logging_filter_t {
public:
    typedef char char_type;

    struct category:
        boost::iostreams::input_filter_tag,
        boost::iostreams::multichar_tag,
        boost::iostreams::optimally_buffered_tag
    { };

    std::string&
    data() {
        return m_log;
    }

    template<typename Source>
    std::streamsize
    read(Source& src, char *buffer, std::streamsize max_size) {
        std::streamsize result = boost::iostreams::read(src, buffer, max_size);

        if (result == -1) {
            return -1;
        } else {
            m_log.append(buffer, result);

            return result;
        }
    }

    std::streamsize
    optimal_buffer_size() const {
        return 0;
    }

private:
    std::string m_log;
};

// Returns the last line and its position in the text.
std::pair<std::string, size_t>
get_last_line(const std::string& text) {
    size_t line_offset = text.find_last_of('\n');

    if (line_offset == std::string::npos) {
        line_offset = 0;
    } else {
        line_offset++;
    }

    return std::pair<std::string, size_t>(text.substr(line_offset), line_offset);
}

std::string
complete_line(std::string log, size_t line_pointer, std::istream& input) {
    auto end_of_error_line = std::find(log.begin() + line_pointer, log.end(), '\n');

    // Read the entire line with the error and make it the last line of the buffer.
    if (end_of_error_line == log.end()) {
        std::string line;

        if (std::getline(input, line)) {
            log += line;
        }
    } else {
        log.erase(end_of_error_line, log.end());
    }

    return log;
}

KORA_NORETURN
void
throw_parser_error(std::string&& config, size_t error_offset, const char *message) {
    /*
     * Produce a pretty output about the error
     * including the line and certain place where
     * the error occured.
     */

    const size_t error_line_number = std::count(config.begin(), config.end(), '\n') + 1;

    std::string error_line; size_t error_line_offset = 0;
    std::tie(error_line, error_line_offset) = get_last_line(config);

    // Let's assume that the tab length is one for simplicity.
    std::replace(error_line.begin(), error_line.end(), '\t', ' ');

    const size_t dash_count = error_offset - error_line_offset;

    std::stringstream error;
    error << "parser error at line " << error_line_number << ": " << message << std::endl
          << error_line << std::endl
          << std::string(dash_count, ' ') << '^' << std::endl
          << std::string(dash_count, '~') << '+' << std::endl;

    throw config_parser_error_t(error.str(), message, error_line_number, dash_count + 1);
}

bool
isspace_predicate(char c) {
    return std::isspace(c);
}

} // namespace

config_t
config_parser_t::parse(std::istream &stream) {
    std::unique_ptr<config_parser_t::implementation_t> new_data(new config_parser_t::implementation_t);

    logging_filter_t filter;
    boost::iostreams::filtering_istream proxy_stream;
    proxy_stream.push(boost::ref(filter));
    proxy_stream.push(boost::ref(stream));

    try {
        new_data->root = kora::dynamic::read_json(proxy_stream);
    } catch (const kora::json_parsing_error_t& e) {
        throw_parser_error(complete_line(std::move(filter.data()), e.offset(), stream),
                           e.offset(),
                           e.message());
    }

    // Because proxy_stream may read from the original stream little more than the parser consumes,
    // checking the original stream on eof() may produce false-negatives.
    // So we should check proxy_stream.
    if (!proxy_stream.eof()) {
        size_t offset = filter.data().size() - 1;

        throw_parser_error(complete_line(std::move(filter.data()), offset, stream),
                           offset,
                           "The input shouldn't contain anything after the root object.");
    }

    if (!new_data->root.is_object()) {
        auto json_start = std::find_if_not(filter.data().begin(), filter.data().end(), &isspace_predicate);
        size_t offset = json_start - filter.data().begin();

        throw_parser_error(complete_line(std::move(filter.data()), offset, stream),
                           offset,
                           "The value must be an object.");
    }

    m_impl = std::move(new_data);

    return root();
}

config_t
config_parser_t::root() const {
    if (m_impl) {
        return config_t("<root>", m_impl->root);
    } else {
        return config_t("<root>", dynamic_t::null);
    }
}
