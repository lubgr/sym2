
#include <boost/container/flat_map.hpp>
#include <string>

int main(int, char**)
{
    boost::container::flat_map<int, std::string> map;

    map[42] = "42";
    map[43] = "43";

    return EXIT_SUCCESS;
}
