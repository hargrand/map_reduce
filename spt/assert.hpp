
/**
 * @file assert.hpp
 * @brief Defines a custom exception-based assertion framework.
 * @details This header provides an `assertion_error` exception class and two
 *          assertion functions, `assert_true` and `assert_equal`, which throw
 *          this exception upon failure. This allows for more controlled error
 *          handling in tests and applications compared to `assert()`.
 */

#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <exception>
#include <string>
#include <sstream>

/**
 * @class assertion_error
 * @brief An exception thrown when an assertion fails.
 * @details Inherits from `std::exception` and holds a descriptive error message.
 */
class assertion_error : public std::exception
{
private:
    const std::string _msg;

public:
    /**
     * @brief Constructs an assertion_error with a specific message.
     * @param msg The error message describing the failed assertion.
     */
    assertion_error(const std::string &msg) : _msg(msg) {}

    /**
     * @brief Gets the error message associated with the assertion failure.
     * @return The error message string.
     */
    inline std::string msg() const { return _msg; }
};

/**
 * @brief Asserts that a condition is true.
 * @details If the condition is false, it throws an `assertion_error` with the given message.
 * @param cond The condition to check.
 * @param msg The message to include in the `assertion_error` if the condition is false.
 */
extern void assert_true(bool cond, std::string msg);

/**
 * @brief Asserts that two values are equal.
 * @details If the values are not equal, it throws an `assertion_error` that includes
 *          the custom message as well as the actual and expected values.
 * @tparam T The type of the values to compare. Must support `operator!=` and `operator<<`.
 * @param actual The value produced by the code under test.
 * @param expected The value that was expected.
 * @param msg A message to include in the `assertion_error` if the values are not equal.
 */
template <typename T>
void assert_equal(const T &actual, const T &expected, std::string msg)
{
    if (actual != expected)
    {
        std::stringstream ss;
        ss << "Equality test failed: " << msg << "\n"
           << "expected=" << expected << ": " << "actual=" << actual;

        throw assertion_error(ss.str());
    }
}

#endif // ASSERT_HPP
