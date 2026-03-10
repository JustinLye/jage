add_executable(jage-editor main.cpp)
target_link_libraries(jage-editor PRIVATE jage::engine::lib imgui_backends)
