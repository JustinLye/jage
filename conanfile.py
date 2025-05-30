from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.env import VirtualBuildEnv
from os import environ

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
        "glad/0.1.36",
        "glfw/3.4",
        "glm/1.0.1",
        "gtest/1.15.0",
        "range-v3/0.12.0",
        "assimp/5.4.3"
    )

    def set_sanitizers_(self):
        sanitizers = []
        def try_setting(env_var:str):
            if "1" == VirtualBuildEnv(self).vars().get(env_var, ""):
                sanitizers.append(env_var.lower())
        
        for san_env_var in ["ASAN", "UBSAN", "TSAN", "LSAN"]:
          try_setting(san_env_var)
        
        if len(sanitizers) > 0:
            self.sanitizer = "-".join(sanitizers)
            if "tsan" in self.sanitizer:
                if "asan" in self.sanitizer:
                    raise RuntimeError("address sanitizer cannot be combined with thread sanitizer.")
                elif "lsan" in self.sanitizer:
                    raise RuntimeError("leak sanitizer cannot be combined with thread sanitizer.")

    def layout(self):
        self.set_sanitizers_()
        self.folders.build_folder_vars = ["self.build_prefix", "settings.os", "settings.compiler", "settings.build_type", "self.sanitizer"]
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
