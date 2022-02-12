
from conans import ConanFile, CMake, tools

class BoostHeadersConan(ConanFile):
    name = "boost-headers"
    version = "1.78.0"
    license = "BSL-1.0"
    url = "https://www.boost.org"
    description = "Boost provides free peer-reviewed portable C++ source libraries"
    topics = ("c++", "boost", "header-only")
    no_copy_source = True

    # This recipe is currently meant for consumption by sym2 only,
    # so hard-wire (and also override externally specified) user/channel:
    user = "sym2"
    channel = "stable"

    # Actively used libraries, i.e., only those with an actual #include in our sources:
    selected = [
            "callable_traits",
            "container",
            "hana",
            "hof",
            "iterator",
            "logic",
            "multiprecision",
            "range",
            "stacktrace",
            "stl_interfaces",
            ]
    # Dependencies required to make the above work. At this point, this is a trial and error list.
    # For a more sophisticated study of the dependencies, see Boost's own bcp tool
    # (https://www.boost.org/doc/libs/release/tools/bcp/doc/html/index.html). Note that the list
    # below is slightly depressing, since lots of header code will be pulled (and parsed) that is
    # only remotely of interest. In particular, Boost range and Boost iterator seem to pull in quite
    # heavy dependencies like fusion and so on.
    transitive = [
            "array",
            "assert",
            "config",
            "concept",
            "concept_check",
            "core",
            "current_function",
            "cstdint",
            "detail",
            "exception",
            "fusion",
            "integer",
            "intrusive",
            "lexical_cast",
            "limits",
            "math",
            "move",
            "mpl",
            "noncopyable",
            "next_prior",
            "numeric",
            "predef",
            "preprocessor",
            "ref",
            "static_assert",
            "swap",
            "tuple",
            "type_traits",
            "type",
            "throw_exception",
            "utility",
            "version",
            ]

    def source(self):
        base = "https://boostorg.jfrog.io/artifactory/main/release"
        underscored = self.version.replace(".", "_")
        url = f"{base}/{self.version}/source/boost_{underscored}.tar.bz2"
        tools.get(url)

    def package(self):
        underscored = self.version.replace(".", "_")
        for library in self.selected + self.transitive:
            self.copy(f"{library}*", dst="include/boost", src=f"boost_{underscored}/boost")

    def package_info(self):
        self.cpp_info.libs = ["boost-headers"]
        self.info.header_only()
