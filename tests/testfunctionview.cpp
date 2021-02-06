
#include <functional>
#include "doctest/doctest.h"
#include "functionview.h"

using namespace sym2;

namespace {
    int square(int base)
    {
        return base * base;
    }

    struct Test {
        const int initial;

        int minus(int value) const
        {
            return initial - value;
        }
    };

    int call(FunctionView<int(int)> callback, int arg)
    {
        return callback(arg);
    }

    int call(FunctionView<int(const Test&, int)> callback, const Test& t, int arg)
    {
        return callback(t, arg);
    }
}

TEST_CASE("FunctionView")
{
    SUBCASE("Lambda without capture")
    {
        const int actual = call([](int arg) { return arg - 1; }, 42);

        CHECK(actual == 41);
    }

    SUBCASE("Lambda with capture")
    {
        const int expected = 123;
        const int actual = call([result = expected](int) { return result; }, 42);

        CHECK(actual == expected);
    }

    SUBCASE("Plain function pointer")
    {
        const int base = 42;
        const int actual = call(square, base);

        CHECK(base * base == actual);
    }

    SUBCASE("Member function pointer")
    {
        const int actual = call(std::mem_fn(&Test::minus), Test{42}, 10);

        CHECK(actual == 32);
    }

    SUBCASE("Copy construction")
    {
        FunctionView<int(int)> orig{square};
        FunctionView<int(int)> copy = orig;
        const int base = 123;
        const int actual = copy(base);

        CHECK(actual == base * base);
    }
}
