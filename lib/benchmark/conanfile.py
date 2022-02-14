
from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
import os

class BenchmarkConan(ConanFile):
    name = "benchmark"
    description = "A microbenchmark support library."
    topics = ("conan", "benchmark", "google", "microbenchmark")
    url = "https://github.com/google/benchmark"
    version = "1.6.1"
    license = "Apache-2.0"
    generators = "cmake"
    settings = "arch", "build_type", "compiler", "os"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "enable_lto": [True, False],
        "enable_exceptions": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "enable_lto": False,
        "enable_exceptions": True,
    }

    # This recipe is currently meant for consumption by sym2 only,
    # so hard-wire (and also override externally specified) user/channel:
    user = "sym2"
    channel = "stable"

    _cmake = None

    def source(self):
        tools.get(f"{self.url}/archive/refs/tags/v{self.version}.tar.gz",
            destination="src", strip_root=True)

    def configure(self):
        if self.options.shared:
            del self.options.fPIC

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)

        self._cmake.definitions["BENCHMARK_ENABLE_TESTING"] = "OFF"
        self._cmake.definitions["BENCHMARK_ENABLE_GTEST_TESTS"] = "OFF"
        self._cmake.definitions["BENCHMARK_ENABLE_LTO"] = "ON" if self.options.enable_lto else "OFF"
        self._cmake.definitions["BENCHMARK_ENABLE_EXCEPTIONS"] = "ON" if self.options.enable_exceptions else "OFF"
        self._cmake.definitions["BENCHMARK_USE_LIBCXX"] = "ON" if self.settings.compiler.get_safe("libcxx") == "libc++" else "OFF"

        self._cmake.configure(source_folder="src")
        return self._cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

        self.copy(pattern="LICENSE", dst="licenses", src="src")
        tools.rmdir(os.path.join(self.package_folder, 'lib', 'pkgconfig'))
        tools.rmdir(os.path.join(self.package_folder, 'share'))

    def package_info(self):
        self.cpp_info.libs = ["benchmark", "benchmark_main"]
        self.cpp_info.builddirs = ["lib/cmake"]
        self.cpp_info.build_modules["cmake"].append("lib/cmake/benchmark/benchmarkConfig.cmake")
        self.cpp_info.build_modules["cmake_find_package"].append("lib/cmake/benchmark/benchmarkConfig.cmake")

        if self.settings.os in ("FreeBSD", "Linux"):
            self.cpp_info.system_libs.extend(["pthread", "rt"])
