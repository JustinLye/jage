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

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"
    build_requires = (
        "boost/1.86.0",
        "glad/0.1.36",
        "glfw/3.4",
        "glm/1.0.1",
        "gtest/1.15.0",
        "spdlog/1.14.1",
    )



    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        print(dir(deps))
        tc = CMakeToolchain(self)
        

        tc.generate()
        
        print(tc.blocks.keys())
        
        for k in tc.blocks.keys():
            print(f"key: {k}\n")
            print(tc.blocks[k].values)

        print(tc.blocks["arch_flags"].values)
        

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
