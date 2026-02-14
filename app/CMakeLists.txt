add_executable(jage main.cpp)
target_link_libraries(jage PRIVATE jage::lib glfw glad::glad)
add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE jage::lib glfw glad::glad)
