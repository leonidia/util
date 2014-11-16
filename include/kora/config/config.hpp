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

#include <memory>
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

    std::string
    buildup_path() const;

private:
    const std::string &m_root_path;
    std::vector<boost::variant<size_t, std::string>> m_backtrace;
};

} // namespace detail

/*! Observer of dynamic_t object which provides nice human readable errors.
 * \warning It's just a view class and it doesn't own the underlying dynamic_t object.
 * You should do the lifetime control on yourown.
 */
class config_t {
    KORA_NONCOPYABLE(config_t)

public:
    //! \param path Path of the object in the original config.
    //! \param value Actually the config data.
    //! \throws std::bad_alloc
    KORA_API
    config_t(const std::string &path, const dynamic_t &value);

    KORA_API
    config_t(config_t&& other) KORA_NOEXCEPT;

    KORA_API
    ~config_t() KORA_NOEXCEPT;

    //! \returns Size of the underlying string, array or object.
    //! \throws config_cast_error_t if the underlying object is not a string, an array, or an object.
    KORA_API
    size_t
    size() const;

    /*! Check whether the underlying object contains given key.
     * \param name The key.
     * \returns \p true if the underlying object contains the key and \p false otherwise.
     * \throws config_cast_error_t if the underlying dynamic_t is not an object.
     */
    KORA_API
    bool
    has(const std::string &name) const;

    /*! Access an item of the underlying object by key.
     * \param name The key.
     * \returns Config stored by the key. Path of the resulting config equals this->path() + "." + \p name.
     * \throws config_cast_error_t if the underlying dynamic_t is not an object.
     * \throws config_access_error_t if the underlying object doesn't contain the key.
     */
    KORA_API
    config_t
    at(const std::string &name) const;

    /*! Access an item of the underlying object by key.
     * \param name The key.
     * \param default_value Value to return if the object doesn't contain the key.
     * \returns Dynamic object stored by the key converted to \p T,
     * or \p default_value if the object doesn't contain the key.
     * \throws config_cast_error_t if the underlying dynamic_t is not an object.
     * \throws config_cast_error_t if the object stored by the key can't be converted to \p T.
     */
    template <typename T>
    typename dynamic::converter<typename pristine<T>::type>::result_type
    at(const std::string &name, const T &default_value) const {
        if (!has(name)) {
            return default_value;
        }

        return at(name).to<T>();
    }

    /*! Access an item of the underlying object by key.
     * \param name The key.
     * \returns Dynamic object stored by the key converted to \p T.
     * \throws config_cast_error_t if the underlying dynamic_t is not an object.
     * \throws config_cast_error_t if the object stored by the key can't be converted to \p T.
     * \throws config_access_error_t if the underlying object doesn't contain the key.
     */
    template <typename T>
    typename dynamic::converter<typename pristine<T>::type>::result_type
    at(const std::string &name) const {
        return at(name).to<T>();
    }

    /*! Access an item of the underlying array by index.
     * \param index The index.
     * \returns Config stored by the index. Path of the resulting config equals this->path() + "[" + \p index + "]".
     * \throws config_cast_error_t if the underlying dynamic_t is not an array.
     * \throws config_access_error_t if <tt>size() >= index</tt>.
     */
    KORA_API
    config_t
    at(size_t index) const;

    /*! Access an item of the underlying array by index.
     * \param index The index.
     * \param default_value Value to return if the object doesn't contain the key.
     * \returns Config stored by the index converted to \p T,
     * or \p default_value if the array doesn't have this index.
     * \throws config_cast_error_t if the underlying dynamic_t is not an array.
     * \throws config_cast_error_t if the object stored by the index can't be converted to \p T.
     */
    template <typename T>
    typename dynamic::converter<typename pristine<T>::type>::result_type
    at(size_t index, const T &default_value) const {
        if (index >= this->to<dynamic_t::array_t>().size()) {
            return default_value;
        } else {
            return at(index).to<T>();
        }
    }

    /*! Access an item of the underlying array by index.
     * \param index The index.
     * \returns Config stored by the index converted to \p T.
     * \throws config_cast_error_t if the underlying dynamic_t is not an array.
     * \throws config_cast_error_t if the object stored by the index can't be converted to \p T.
     * \throws config_access_error_t if <tt>size() >= index</tt>.
     */
    template <typename T>
    typename dynamic::converter<typename pristine<T>::type>::result_type
    at(size_t index) const {
        return at(index).to<T>();
    }

    /*! Converts the config to an arbitrary type.
     *
     * It call dynamic_t::to() method for the underlying object to perform the conversion.
     *
     * \tparam T Type determining dynamic::converter.
     * \returns Result of the conversion returned by dynamic_t::to().
     * \throws config_cast_error_t If the underlying object is not convertible to the result type.
     */
    template <typename T>
    typename dynamic::converter<typename pristine<T>::type>::result_type
    to() const {
        return this->underlying_object().to<T>(detail::config_conversion_controller_t(path()));
    }

    //! \returns Path to the object in the original config.
    KORA_API
    const std::string&
    path() const KORA_NOEXCEPT;

    //! \returns Underlying dynamic_t object which this config_t provides access to.
    KORA_API
    const dynamic_t&
    underlying_object() const KORA_NOEXCEPT;

private:
    class implementation_t;

    std::unique_ptr<implementation_t> m_impl;
};

/*!
 * \relates config_t
 * Compares underlying value of config_t with other dynamic object.
 *
 * \param left The dynamic object.
 * \param right The config.
 * \returns <tt>left == right.underlying_object()</tt>
 */
KORA_API
bool
operator==(const dynamic_t &left, const config_t &right) KORA_NOEXCEPT;

/*!
 * \relates config_t
 * \overload
 *
 * \sa operator==(const dynamic_t &, const config_t &)
 */
KORA_API
bool
operator==(const config_t &left, const dynamic_t &right) KORA_NOEXCEPT;

/*!
 * \relates config_t
 *
 * \sa operator==(const dynamic_t &, const config_t &)
 */
KORA_API
bool
operator!=(const dynamic_t &left, const config_t &right) KORA_NOEXCEPT;

/*!
 * \relates config_t
 *
 * \sa operator==(const dynamic_t &, const config_t &)
 */
KORA_API
bool
operator!=(const config_t &left, const dynamic_t &right) KORA_NOEXCEPT;

/*!
 * \relates config_t
 * Prints the value stored in the underlying dynamic_t object.
 *
 * \param stream Stream to print to.
 * \param value Config to print.
 * \returns \p stream
 *
 * \sa \p operator<<(std::ostream&, const dynamic_t&)
 */
KORA_API
std::ostream&
operator<<(std::ostream& stream, const config_t& value);

} // namespace kora

#endif
