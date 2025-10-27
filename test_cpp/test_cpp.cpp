/**
 * @file collection_test.cpp
 * @brief Test suite for the Collection class and its associated operations.
 */

#include "../spt/natv_collection.hpp"
#include "../test_common/common.hpp"

#include <iostream>

/**
 * @brief Main entry point for the collection test executable.
 * @details This program initializes two `Collection` objects with test data
 *          (Fibonacci numbers and factorials) and then runs a series of tests
 *          to verify the correctness of element-wise operations (+, -, *, /),
 *          reduction operations (sum, prod), and the dot product.
 *          It uses the custom assertion framework from `common.hpp`.
 * @return 0 if all tests pass, 1 if any test fails and throws an `assertion_error`.
 */
int main()
{
    auto fib = [](std::size_t n)
    {
        if (n < 2)
            return (double)n;

        unsigned long a = 1;
        unsigned long b = 1;
        for (unsigned long i = 2; i < n; i++)
        {
            unsigned long c = a + b;
            a = b;
            b = c;
        }

        return (double)b;
    };

    auto fact = [](std::size_t n)
    {
        if (n < 2)
            return (double)1;

        unsigned long result = 1;
        for (unsigned long i = 2; i <= n; i++)
            result *= i;

        return (double)result;
    };

    Collection<double> u(10, fib);
    Collection<double> v(10, fact);

    try
    {
        test_constructor<double>(fib);
        test_access_set<double>(0.0, fib);
        test_access_set(-5.0, fib);
        test_operation(u, v, std::plus<Collection<double>>(), std::plus<double>());
        test_operation(u, v, std::minus<Collection<double>>(), std::minus<double>());
        test_operation(u, v, std::multiplies<Collection<double>>(), std::multiplies<double>());
        test_operation(u, v, std::divides<Collection<double>>(), std::divides<double>());
        test_reduce(v, sum<double>, std::plus<double>());
        test_reduce(v, prod<double>, std::multiplies<double>());
        test_dot(u, v, dot<double>, std::multiplies<Collection<double>>(), sum<double>);
        std::cout << "All tests passed!" << std::endl;
    }
    catch (assertion_error e)
    {
        std::cout << e.msg() << std::endl;
        return 1;
    }

    return 0;
}