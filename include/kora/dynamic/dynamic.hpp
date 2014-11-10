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

#ifndef KORA_DYNAMIC_DYNAMIC_HPP
#define KORA_DYNAMIC_DYNAMIC_HPP

#include "kora/dynamic/detail.hpp"

KORA_PUSH_VISIBLE
#include <boost/variant.hpp>
KORA_POP_VISIBILITY

#include <string>
#include <utility>
#include <vector>

namespace kora {

class dynamic_t;

/*!
 * \brief Trait class to convert data types to \p dynamic_t.
 *
 * It's called from constructors and assignment operators of \p dynamic_t
 * to create the \p dynamic_t object from a value of type \p From.
 * User may specialize this trait to enable conversion from new types.
 *
 * \warning All specializations are required to be exception-safe, i.e. they should leave
 * the dynamic object untouched in case of an exception being thrown.
 *
 * The second template argument may be used to do the SFINAE magic.
 *
 * \tparam From The type being converted to \p dynamic_t.
 */
template<class From, class = void>
struct dynamic_constructor {
    //! This constant should be \p true to enable the specialization.
    static const bool enable = false;

    /*!
     * \brief Performs the conversion.
     * \param[in] from Value to create a \p dynamic_t object from.
     * \param[out] to Variable to store the conversion result.
     */
    static inline
    void
    convert(From from, dynamic_t& to);
};

/*!
 * \brief Trait class to convert \p dynamic_t to other data types.
 *
 * Method \p dynamic_t::to() calls this trait to convert \p dynamic_t to other types.
 * User may specialize it to enable conversion to new types.
 *
 * \note All specializations should define nested type \p result_type which is the resulting type of the conversion.
 *
 * The second template argument may be used to do the SFINAE magic.
 *
 * \tparam To The type passed to method \p dynamic_t::to() as the first template argument.
 * \sa dynamic_converter<bool>
 */
template<class To, class = void>
struct dynamic_converter {
    /*!
     * \brief Performs conversion of \p dynamic_t to the result type.
     *
     * It's provided with a user-defined "controller" which performs error handling.
     * It means that the function shouldn't throw errors itself. Instead it should call
     * controller's \p fail() method with an error of a type derived from \p kora::bad_cast_t.
     * Also if the function traverses complex data structures (arrays and objects), it must
     * let the controller know about it via calling special member functions.
     *
     * \warning None of controller's methods may be called after \p fail().
     * It means that this function shouldn't continue conversion after any call
     * to \p dynamic_t::to() method throws.
     * Probably you don't even want to catch these exceptions.
     *
     * \sa kora::detail::dynamic::default_conversion_controller_t
     * \tparam Controller Type of the controller.
     * \param[in] from The object being converted.
     * \param[in,out] controller Controller provided by the caller.
     * \returns The conversion result.
     */
    template<class Controller>
    static inline
    To
    convert(const dynamic_t& from, Controller& controller);

    /*!
     * \brief Checks whether the conversion of the object to the type is possible.
     *
     * \warning This function shouldn't throw exceptions.
     *
     * \param[in] from The object being converted.
     * \returns \p true if the conversion is possible and \p false otherwise.
     */
    static inline
    bool
    convertible(const dynamic_t& from) KORA_NOEXCEPT;
};

/*!
 * \brief Recursive data structure to store JSON-like data.
 *
 * It can store data of any of the following types:
 *  - Null
 *  - Boolean
 *  - Integer
 *  - Unsigned integer
 *  - Floating point
 *  - String
 *  - Array of dynamic_t objects
 *  - Associative array indexed by strings and storing \p dynamic_t objects
 *
 * The reason why the integer type and unsigned integer type are separated is because the latter
 * can store positive numbers of wider range. This ability may be important in some usecases.
 */
class dynamic_t {
public:
    //! Type used to store null values.
    /*!
     * \typedef null_t;
     * All values of this type are equal.
     */
    typedef boost::blank null_t;

    //! Type used to store boolean values.
    /*!
     * \typedef bool_t;
     * It may be used instead of \p bool in any context.
     */
    typedef bool bool_t;

    //! Arithmetic type used to store integer values.
    /*!
     * \typedef int_t;
     * It's guaranteed to be able to store any value of type \p int64_t.
     */
    typedef int64_t int_t;

    //! Arithmetic type used to store unsigned integer values.
    /*!
     * \typedef uint_t;
     * It's guaranteed to be able to store any value of type \p uint64_t.
     */
    typedef uint64_t uint_t;

    //! Arithmetic type used to store floating point numbers.
    /*!
     * \typedef double_t;
     * It's guaranteed to be able to store any value of type \p double.
     */
    typedef double double_t;

    //! Type used to store strings.
    /*!
     * \typedef string_t;
     * It's guaranteed to have API backward compatible with \p std::string.
     */
    typedef std::string string_t;

    //! Type used to store arrays.
    /*!
     * \typedef array_t;
     * It's guaranteed to have API backward compatible with \p std::vector<dynamic_t>.
     */
    typedef std::vector<dynamic_t> array_t;

    // See comments in the definition of the class.
    class object_t;

    // Just useful constants which may be accessed by reference from any place of the program.
    KORA_API static const dynamic_t null;
    KORA_API static const dynamic_t empty_string;
    KORA_API static const dynamic_t empty_array;
    KORA_API static const dynamic_t empty_object;

public:
    //! \brief Default constructor.
    //! \post <tt>this->is_null() == true</tt>
    KORA_API
    dynamic_t() KORA_NOEXCEPT;

    //! \brief Copy constructor.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t(const dynamic_t& other);

    KORA_API
    dynamic_t(dynamic_t&& other) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(null_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(bool_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(int_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(uint_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t(double_t value) KORA_NOEXCEPT;

    //! \brief Creates dynamic object containing a string value.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t(const string_t& value);

    KORA_API
    dynamic_t(string_t&& value) KORA_NOEXCEPT;

    //! \brief Creates dynamic object containing an array.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t(array_t value);

    //! \brief Creates dynamic_t containing an object.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t(object_t value);

    /*!
     * \brief Construct dynamic object from an arbitrary type.
     *
     * It uses <tt>dynamic_constructor<typename pristine<T>::type></tt> to construct the object.\n
     * This constructor participates in overload resolution only if
     * <tt>dynamic_constructor<typename pristine<T>::type>::enable == true</tt>.
     *
     * \sa dynamic_constructor
     * \throws Any exception thrown by dynamic_constructor.
     */
    template<class T>
    dynamic_t(
        T&& from,
        typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable>::type* = 0
    );

    //! \brief Copy assignment operator.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t&
    operator=(const dynamic_t& other);

    KORA_API
    dynamic_t&
    operator=(dynamic_t&& other) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(null_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(bool_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(int_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(uint_t value) KORA_NOEXCEPT;

    KORA_API
    dynamic_t&
    operator=(double_t value) KORA_NOEXCEPT;

    //! \brief Assigns string value to the object.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t&
    operator=(const string_t& value);

    KORA_API
    dynamic_t&
    operator=(string_t&& value) KORA_NOEXCEPT;

    //! \brief Assigns array to the object.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t&
    operator=(array_t value);

    //! \brief Assigns object to the dynamic_t.
    //! \throws std::bad_alloc
    KORA_API
    dynamic_t&
    operator=(object_t value);

    /*!
     * \fn template<class T> dynamic_t& operator=(T&& from)
     * \brief Assigns value of an arbitrary type to the dynamic object.
     *
     * It uses <tt>dynamic_constructor<typename pristine<T>::type></tt> to perform the conversion.\n
     * This operator participates in overload resolution only if
     * <tt>dynamic_constructor<typename pristine<T>::type>::enable == true</tt>.
     *
     * \sa dynamic_constructor
     * \throws Any exception thrown by dynamic_constructor.
     */
    template<class T>
    typename std::enable_if<dynamic_constructor<typename pristine<T>::type>::enable, dynamic_t&>::type
    operator=(T&& from);


    /*!
     * \brief Calls the visitor with the value stored in the object.
     *
     * Performs function call \visitor(x) where \p x is value stored in the dynamic object and has
     * one of the following types:
     *  - dynamic_t::null_t
     *  - dynamic_t::bool_t
     *  - dynamic_t::int_t
     *  - dynamic_t::uint_t
     *  - dynamic_t::double_t
     *  - dynamic_t::string_t
     *  - dynamic_t::array_t
     *  - dynamic_t::object_t
     *
     * \throws Any exception thrown by the visitor.
     */
    template<class Visitor>
    typename std::decay<Visitor>::type::result_type
    apply(Visitor&& visitor);

    //! \overload
    template<class Visitor>
    typename std::decay<Visitor>::type::result_type
    apply(Visitor&& visitor) const;

    KORA_API
    bool
    is_null() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_bool() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_int() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_uint() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_double() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_string() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_array() const KORA_NOEXCEPT;

    KORA_API
    bool
    is_object() const KORA_NOEXCEPT;

    //! \returns Stored bool value.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::bool_t.
    KORA_API
    bool_t
    as_bool() const;

    //! \returns Stored int value.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::int_t.
    KORA_API
    int_t
    as_int() const;

    //! \returns Stored uint value.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::uint_t.
    KORA_API
    uint_t
    as_uint() const;

    //! \returns Stored double value.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::double_t.
    KORA_API
    double_t
    as_double() const;

    //! \returns Stored string.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::string_t.
    KORA_API
    const string_t&
    as_string() const;

    //! \returns Stored array.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::array_t.
    KORA_API
    const array_t&
    as_array() const;

    //! \returns Stored object.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::object_t.
    KORA_API
    const object_t&
    as_object() const;

    //! \returns Stored string.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::string_t.
    KORA_API
    string_t&
    as_string();

    //! \returns Stored array.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::array_t.
    KORA_API
    array_t&
    as_array();

    //! \returns Stored object.
    //! \throws bad_cast_t if the object doesn't contain value of type dynamic_t::object_t.
    KORA_API
    object_t&
    as_object();

    /*!
     * \brief Checks whether the conversion of the object to a type is possible.
     *
     * It uses \p dynamic_converter::convertible() to perform the check.\n
     * Specialization being used: <tt>dynamic_converter<typename pristine<T>::type></tt>
     *
     * \sa dynamic_converter
     * \tparam T Type determining dynamic_converter.
     * \returns \p true if the conversion is possible and \p false otherwise.
     */
    template<class T>
    bool
    convertible_to() const KORA_NOEXCEPT;

    /*!
     * \brief Converts the object to an arbitrary type.
     *
     * It uses <tt>dynamic_converter<typename pristine<T>::type></tt> to perform the conversion.
     *
     * \sa dynamic_converter
     * \tparam T Type determining dynamic_converter.
     * \tparam Controller Type of the controller.
     * \param controller Object handling conversion errors. Forwarded to the underlying \p dynamic_converter.
     * \returns Result of conversion returned by \p dynamic_converter.
     * \throws Any exceptions thrown by \p dynamic_converter and by the controller.
     */
    template<class T, class Controller>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to(Controller&& controller) const;

    /*!
     * \brief Converts the object to an arbitrary type.
     *
     * It's the same as the previous function, but always uses \p default_conversion_controller_t.
     * This controller just throws errors provided by \p dynamic_converter.
     *
     * \sa dynamic_converter
     * \tparam T Type determining dynamic_converter.
     * \returns Result of conversion returned by \p dynamic_converter.
     * \throws Any exceptions thrown by \p dynamic_converter and errors passed to the controller by \p dynamic_converter.
     */
    template<class T>
    typename dynamic_converter<typename pristine<T>::type>::result_type
    to() const;

public:
    /*!
     * \brief Creates dynamic object from JSON.
     *
     * This function doesn't require the input stream to contain only one JSON object.
     * It reads one JSON object with surrounding spaces and leaves other data untouched.
     *
     * \param input Stream containing the JSON.
     * \returns Constructed dynamic object.
     * \throws json_parsing_error_t
     * \throws std::bad_alloc
     * \throws Any exception thrown by \p input.
     */
    KORA_API
    static
    dynamic_t
    from_json(std::istream &input);

    /*!
     * \brief Serializes the object into JSON format.
     *
     * This method may serialize not only objects and arrays, but any dynamic_t object.
     * It's not like standard JSON, and you should perform additional checks on yourown
     * if you want to receive a JSON object or array.
     *
     * \param output Stream to write the resulting JSON to.
     * \throws std::bad_alloc
     * \throws Any exception thrown by \p output.
     */
    KORA_API
    void
    to_json(std::ostream &output) const;

private:
    template<class T>
    T&
    get();

    template<class T>
    const T&
    get() const;

    template<class T>
    bool
    is() const;

    struct move_visitor;

private:
    typedef boost::variant<
        null_t,
        bool_t,
        int_t,
        uint_t,
        double_t,
        string_t,
        detail::dynamic::incomplete_wrapper<array_t>,
        detail::dynamic::incomplete_wrapper<object_t>
    > value_t;

    value_t m_value;
};

/*!
 * \returns \p true if \p left and \p right store equal values of the same type,
 * or if they store equal numeric values (<tt>5.0 == 5</tt>). Otherwise it returns \p false.
 */
KORA_API
bool
operator==(const dynamic_t& left, const dynamic_t& right) KORA_NOEXCEPT;

/*!
 * \sa operator==(const dynamic_t&, const dynamic_t&)
 * \returns opposite to the <tt>operator==(const dynamic_t&, const dynamic_t&)</tt>.
 */
KORA_API
bool
operator!=(const dynamic_t& left, const dynamic_t& right) KORA_NOEXCEPT;

/*!
 * Prints the value stored in the dynamic object. Null value is printed as "null",
 * boolean as value of type \p bool, array and object are printed in JSON format.
 *
 * \param stream Stream to print to.
 * \param value Object to print.
 * \returns \p stream
 */
KORA_API
std::ostream&
operator<<(std::ostream& stream, const dynamic_t& value);

} // namespace kora

#include "kora/dynamic/dynamic.impl"
#include "kora/dynamic/object.hpp"
#include "kora/dynamic/constructors.hpp"
#include "kora/dynamic/converters.hpp"

#endif
