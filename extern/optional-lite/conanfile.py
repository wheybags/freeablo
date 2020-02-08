from conans import ConanFile, CMake

class OptionalLiteConan(ConanFile):
    version = "3.2.0"
    name = "optional-lite"
    description = "A single-file header-only version of a C++17-like optional, a nullable object for C++98, C++11 and later"
    license = "Boost Software License - Version 1.0. http://www.boost.org/LICENSE_1_0.txt"
    url = "https://github.com/martinmoene/optional-lite.git"
    exports_sources = "include/nonstd/*", "CMakeLists.txt", "cmake/*", "LICENSE.txt"
    build_policy = "missing"
    author = "Martin Moene"

    def build(self):
        """Avoid warning on build step"""
        pass

    def package(self):
        """Run CMake install"""
        cmake = CMake(self)
        cmake.definitions["OPTIONAL_LITE_OPT_BUILD_TESTS"] = "OFF"
        cmake.definitions["OPTIONAL_LITE_OPT_BUILD_EXAMPLES"] = "OFF"
        cmake.configure()
        cmake.install()

    def package_info(self):
        self.info.header_only()
