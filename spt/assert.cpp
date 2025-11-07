
/**
 * @file assert.cpp
 * @brief Implements the non-template functions for the custom assertion framework.
 */

#include "assert.hpp"

/**
 * @brief Asserts that a condition is true.
 * @details Throws an `assertion_error` with the given message if the condition is false.
 * @param cond The condition to check.
 * @param msg The message to include in the `assertion_error` if the condition is false.
 */
void assert_true(bool cond, std::string msg)
{
    if (!cond)
        throw assertion_error(msg);
}
