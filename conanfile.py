
from conans import ConanFile, CMake, tools

class Sym2Conan(ConanFile):
    name = "sym2"
    version = "0.0.0"
    license = "Apache License 2.0"
    author = "Lukas Böger <mail@lboeger.de>"
    url = "https://git.sr.ht/~lubgr/sym2"
    description = "Fast, tiny symbolic library"
    topics = ("cas", "computer algebra")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"

    def requirements(self):
        self.requires("chibi-scheme/0.10-e4766f8c")
        self.requires("boost-headers/1.78.0")

    def source(self):
        git = tools.Git("src")
        git.clone(url="https://git.sr.ht:~lubgr/sym2")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def imports(self):
        self.copy("lib/*", root_package="chibi-scheme", dst="scheme", excludes=["lib/cmake", "lib/pkgconfig"])
        self.copy("share/*", root_package="chibi-scheme", dst="scheme", excludes="share/man")
        self.copy("bin/chibi-scheme", dst="scheme", root_package="chibi-scheme")

    def package_info(self):
        self.cpp_info.libs = ["sym2"]
