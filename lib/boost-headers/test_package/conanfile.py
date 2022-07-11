
from conans import ConanFile, CMake

class PackageTestConan(ConanFile):
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        self.run("bin/package-test")
