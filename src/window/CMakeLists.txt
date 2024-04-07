add_library(src-window STATIC
    Window.cpp
)

target_include_directories(src-window PRIVATE
    ${dep_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(src-window PRIVATE
    daxa::daxa
    glfw
)
