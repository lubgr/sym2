
from conans import ConanFile, tools, CMake
import os

class DoctestConan(ConanFile):
    name = "doctest"
    url = "https://github.com/conan-io/conan-center-index"
    version = "2.4.11"
    homepage = "https://github.com/onqtam/doctest"
    description = "C++11/14/17/20 single header testing framework"
    topics = ("doctest", "header-only", "unit-test", "tdd")
    settings = "os", "compiler"
    license = "MIT"

    # This recipe is currently meant for consumption by sym2 only,
    # so hard-wire (and also override externally specified) user/channel:
    user = "sym2"
    channel = "stable"

    def source(self):
        tools.get(f"https://github.com/doctest/doctest/archive/v{self.version}.tar.gz")
        extracted_dir = self.name + "-" + self.version
        os.rename(extracted_dir, "src")

    def package(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_CXX_FLAGS"] = "-Wno-error=redundant-decls"
        cmake.configure(source_folder="src")
        cmake.install()
        self.copy(pattern="LICENSE.txt", dst="licenses", src="src")

    def package_info(self):
        self.cpp_info.builddirs.append("lib/cmake")
        self.cpp_info.builddirs = ["lib/cmake"]
        self.cpp_info.build_modules["cmake"].append("lib/cmake/doctest/doctestConfig.cmake")
        self.cpp_info.build_modules["cmake_find_package"].append("lib/cmake/doctest/doctestConfig.cmake")

    def package_id(self):
        self.info.header_only()
