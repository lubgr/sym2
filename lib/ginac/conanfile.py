
from conans import ConanFile, CMake, tools
import shutil
import os

class GinacConan(ConanFile):
    name = "ginac"
    version = "1.8.2"
    url = "https://www.ginac.de"
    license = "GPL"
    description = "C++ library for symbolic mathematical calculations"
    topics = ("computer-algebra", "math")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    generators = "cmake"

    # This recipe is currently meant for consumption by sym2 only,
    # so hard-wire (and also override externally specified) user/channel:
    user = "sym2"
    channel = "stable"

    def system_requirements(self):
        # We should add libgmp (including headers) here at some point. See
        # https://docs.conan.io/en/latest/reference/conanfile/tools/system/package_manager.html
        pass

    def source(self):
        git = tools.Git("src")
        git.clone(url="git://www.ginac.de/ginac.git")
        dashes = self.version.replace(".", "-")
        tag = f"release_{dashes}"
        git.checkout(tag)

        # We don't want to build the interactive interpreter or the docs:
        tools.replace_in_file("src/CMakeLists.txt", "add_subdirectory(ginsh)", "")
        tools.replace_in_file("src/CMakeLists.txt", "add_subdirectory(doc)", "")

        git = tools.Git("src/cln")
        git.clone(url="git://www.ginac.de/cln.git")
        # CLN doesn't include the CMake configuration and related files in its releases, so we work
        # with the master branch for now:
        git.checkout("master")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder="src")
        cmake.definitions["CMAKE_INSTALL_RPATH"] = ""
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ginac", "cln"]
        self.cpp_info.builddirs = ["lib/cmake"]
        self.cpp_info.build_modules["cmake"].append("lib/cmake/ginac/ginac-config.cmake")
        self.cpp_info.build_modules["cmake_find_package"].append("lib/cmake/ginac/ginac-config.cmake")
