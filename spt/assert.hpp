
#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <exception>
#include <string>
#include <sstream>

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
     * @brief Returns the error message associated with the assertion failure.
     * @return A constant reference to the error message string.
     */
    inline std::string msg() const { return _msg; }
};

/**
 * @brief Custom assertion function. If the condition is false, it throws an `assertion_error`.
 * @param cond The condition to check.
 * @param msg The message to include in the `assertion_error` if the condition is false.
 */
extern void assert_true(bool cond, std::string msg);

/**
 * @brief Raise an assertion error if the two values given are not equal
 *
 * @tparam T Type of the values to compare
 * @param actual Value given
 * @param expected Value expected
 * @param msg Message to include in the assertion error if the values are unequal
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
