string(TOUPPER "${CMAKE_BUILD_TYPE}" IMGUI_BUILD_TYPE_UPPER)
set(IMGUI_BINDINGS_DIR
    "${imgui_PACKAGE_FOLDER_${IMGUI_BUILD_TYPE_UPPER}}/res/bindings")

add_library(imgui_backends STATIC
    ${IMGUI_BINDINGS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BINDINGS_DIR}/imgui_impl_opengl3.cpp)
target_include_directories(imgui_backends PUBLIC ${IMGUI_BINDINGS_DIR})
target_link_libraries(imgui_backends PUBLIC imgui::imgui glfw glad::glad)

add_subdirectory(app)
