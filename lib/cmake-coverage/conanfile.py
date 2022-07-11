
from conans import ConanFile, tools, CMake
import os

class CmakeCoverageConan(ConanFile):
    name = "cmake-coverage"
    url = "https://github.com/bilke/cmake-modules"
    version = "03be0c7"
    description = "Additional CMake modules"
    license = "BSD-3-Clause"

    # This recipe is currently meant for consumption by sym2 only,
    # so hard-wire (and also override externally specified) user/channel:
    user = "sym2"
    channel = "develop"

    def source(self):
        git = tools.Git("src")
        git.clone(url=self.url)
        git.checkout(self.version)

    def package(self):
        self.copy(pattern="CodeCoverage.cmake", src="src")

    def package_id(self):
        self.info.header_only()
