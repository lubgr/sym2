
#include <stdexcept>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("Doctest test")
{
    const std::string expected{"test-string"};

    SUBCASE("String equality")
    {
        CHECK(expected == "test-string");
    }

    SUBCASE("Integer inequality")
    {
        const int n = 42;

        CHECK_FALSE(43 == n);
    }

    SUBCASE("Throw exception")
    {
        CHECK_THROWS([](){ throw std::logic_error{"Test exception"}; }());
    }
}
