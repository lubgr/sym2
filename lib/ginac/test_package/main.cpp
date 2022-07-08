
#include <algorithm>
#include <ginac/ginac.h>
#include <iostream>
#include <sstream>
#include <string>

int main(int, char**)
{
    const GiNaC::symbol a{"a"};
    const GiNaC::symbol b{"b"};

    if (a + b != b + a) {
        std::cerr << "Unexpected GiNaC result: " << a + b << " != " << b + a << '\n';
        return EXIT_FAILURE;
    }

    const GiNaC::ex c = a + a + 2 * b + 2 * a / 3;
    const GiNaC::ex d = 3 * b * b * c + 2 * a - 4 * b;
    const GiNaC::ex e = 2 * a / 7 * b * c * c - 2 * b;

    std::string str = [&]() {
        std::ostringstream stream;
        stream << e;
        return stream.str();
    }();

    const GiNaC::ex expected = -2 * b + 8 * b / 63 * a * GiNaC::pow(3 * b + 4 * a, 2);

    if (e != expected) {
        std::cerr << "Unexpected GiNaC result: " << e << " != " << expected << " (expected last)\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
