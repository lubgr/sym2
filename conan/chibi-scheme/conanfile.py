
from conans import ConanFile, CMake, tools

class ChibiSchemeConan(ConanFile):
    name = "chibi-scheme"
    commit = "e4766f8c"
    version = f"0.10-{commit}"
    license = "BSD-3-Clause"
    author = "Alex Shinn <alexshinn@gmail.com>"
    url = "https://synthcode.com/scheme/chibi"
    description = "Minimal Scheme Implementation for use as an Extension Language"
    topics = ("scheme", "lisp")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    generators = "cmake"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        git = tools.Git("src")
        git.clone(url="https://github.com/ashinn/chibi-scheme.git")
        git.checkout(self.commit)

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder="src")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["chibi-scheme"]
        self.cpp_info.builddirs = ["lib/cmake"]
        self.cpp_info.build_modules["cmake"].append("lib/cmake/chibi/chibi-scheme-config.cmake")
        self.cpp_info.build_modules["cmake_find_package"].append("lib/cmake/chibi/chibi-scheme-config.cmake")
