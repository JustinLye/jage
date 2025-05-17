from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps



class JAGERecipe(ConanFile):
    name = "jage"
    version = "0.0"
    package_type = "application"

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of JAGE package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    build_prefix = "build"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"
    requires = (
        "boost/1.86.0",
        "glad/0.1.36",
        "glfw/3.4",
        "glm/1.0.1",
        "gtest/1.15.0",
        "spdlog/1.14.1",
        "range-v3/0.12.0"
    )



    def layout(self):
        self.folders.build_folder_vars = ["self.build_prefix", "settings.os", "settings.compiler", "settings.build_type"]
        print(dir(self.folders))
        cmake_layout(self, build_folder=".")
        

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
        
        

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
