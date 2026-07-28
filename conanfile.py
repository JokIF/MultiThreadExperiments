from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class MultiThreadExperimentsConan(ConanFile):
    name = "multithread-experiments"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    exports_sources = "CMakeLists.txt", "core/*", "libs/*", "cmake/*"

    def requirements(self):
        self.requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()