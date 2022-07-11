
from conans import ConanFile, CMake
import os

class PackageTestConan(ConanFile):
    settings   = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = ["main.scm"]

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("lib", root_package="chibi-scheme", dst="chibi", excludes=["lib/cmake", "lib/pkgconfig"])
        self.copy("share", root_package="chibi-scheme", dst="chibi", excludes="share/man")

    def test(self):
        chibi_test_file = os.path.join(self.source_folder, "main.scm")
        chibi_libdir = os.path.join(self.build_folder, "chibi/lib")
        chibi_sharedir = os.path.join(self.build_folder, "chibi/share")

        self.run("chibi-scheme -I{} -I{} {}".format(chibi_libdir, chibi_sharedir, chibi_test_file),
                run_environment=True) 

        self.run("bin/package-test", run_environment=True)
