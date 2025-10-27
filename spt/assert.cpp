
#include "assert.hpp"

/**
 * @brief Implements the custom assertion check.
 * @details Throws an `assertion_error` if the provided condition is false.
 *          This provides a consistent exception-based error handling mechanism for tests.
 */
void assert_true(bool cond, std::string msg)
{
    if (!cond)
        throw assertion_error(msg);
}
